/**
 * TODO:
 * accumulate spectrum data over time to present as spectrogram, with option of viewing each window as a spectrum
 * visualize phase data
 * draw lines instead of rectangles
 */

#define OS_FEATURE_GFX
#include "base/base.h"
#include "os/os.h"
#include "render/render.h"
#include "font/font.h"
#include "OpenGL/ogl.h"
#include "audio/audio.h"
#include "fourier/fourier.h"
#include "ui/ui.h"

#include "base/base.c"
#include "os/os.c"
#include "render/render.c"
#include "font/font.c"
#include "OpenGL/ogl.c"
#include "audio/audio.c"
#include "fourier/fourier.c"
#include "ui/ui.c"

//
// audio
//
typedef struct SineOscState
{
  R32 phasor;
  R32 base_freq;
  R32 pitch_bend;
  R32 volume;
} SineOscState;

typedef struct AudioBuffer
{
  U64 sample_count;
  R32 *samples[2];
} AudioBuffer;

typedef struct AudioBufferNode AudioBufferNode;
struct AudioBufferNode
{
  AudioBufferNode *next;
  AudioBuffer buf;
};

typedef struct AudioBufferList
{
  Arena *arena;

  U32 lock;

  AudioBufferNode *first;
  AudioBufferNode *last;
  U64 count;

  AudioBufferNode *freelist;
} AudioBufferList;

global AudioBufferList audio_buffer_list = {0};

void
audio_process(Audio_ProcessData *data)
{
  ArenaTemp scratch = arena_get_scratch(0, 0);

  SineOscState *sine_state = data->user_data;
  U32 sample_count = data->sample_count;

  if(sine_state) {
    // NOTE: react to midi messages
    R32 base_freq = sine_state->base_freq;
    R32 pitch_bend = sine_state->pitch_bend;
    R32 *freq_buffer = arena_push_array(scratch.arena, R32, sample_count);
    for(U32 sample_idx = 0; sample_idx < sample_count; ++sample_idx) {

      Audio_MidiMessage *midi_msg = audio_next_midi_message(data);
      if(midi_msg) {
	if(midi_msg->sample_idx >= sample_idx) {

	  switch(midi_msg->opcode) {
	  
	  case MidiOpcode_note_on: {
	    U32 midi_note = audio_midi_get_note_number(midi_msg);
	    base_freq = audio_hertz_from_midi_note(midi_note);
	  } break;
	  
	  case MidiOpcode_pitch_bend: {
	    pitch_bend = audio_midi_get_pitch_bend(midi_msg);
	  } break;
	  
	  default: {} break;
	  }

	  audio_dequeue_midi_message(data);
	}
      }

      // TODO: smooth out pitch bend changes
      freq_buffer[sample_idx] = base_freq * pitch_bend;
    }

    sine_state->base_freq = base_freq;
    sine_state->pitch_bend = pitch_bend;
  
    // NOTE: compute audio samples
    R32 *mix_buffer = arena_push_array_z(scratch.arena, R32, sample_count);
    for(U32 sample_idx = 0; sample_idx < sample_count; ++sample_idx) {      

      R32 sine_sample = sine_state->volume * sinf(sine_state->phasor);
      mix_buffer[sample_idx] += sine_sample;

      R32 step = TAU32 * freq_buffer[sample_idx] * data->sample_period;
      sine_state->phasor += step;
      if(sine_state->phasor > TAU32) {
	sine_state->phasor -= TAU32;
      }
    }

    // NOTE: output samples
    CopyArray(data->output[0], mix_buffer, R32, sample_count);
    CopyArray(data->output[1], mix_buffer, R32, sample_count);

    // NOTE: queue audio buffer
    // NOTE: try to pull a node off the freelist, otherwise allocate a new one
    AudioBufferNode *node = 0;    
    while(!AtomicCompareAndSwap(&audio_buffer_list.lock, 0, 1)) {}
    {
      U32 sample_count_pow2 = RoundUpPow2(sample_count);
      if(audio_buffer_list.freelist) {

	node = audio_buffer_list.freelist;
	Assert(node->buf.sample_count == sample_count_pow2);
	SLLStackPop(audio_buffer_list.freelist);
      }else {

	node = arena_push_struct(audio_buffer_list.arena, AudioBufferNode);	
	node->buf.sample_count = sample_count_pow2;
	node->buf.samples[0] = arena_push_array_z(audio_buffer_list.arena, R32, sample_count_pow2);
	node->buf.samples[1] = arena_push_array_z(audio_buffer_list.arena, R32, sample_count_pow2);
      }
    }
    while(!AtomicCompareAndSwap(&audio_buffer_list.lock, 1, 0)) {}

    // NOTE: copy samples to audio buffer
    Assert(node);
    CopyArray(node->buf.samples[0], mix_buffer, R32, sample_count);
    CopyArray(node->buf.samples[1], mix_buffer, R32, sample_count);

    // NOTE: push audio buffer onto the queue
    while(!AtomicCompareAndSwap(&audio_buffer_list.lock, 0, 1)) {}
    {
      SLLQueuePush(audio_buffer_list.first, audio_buffer_list.last, node);
      ++audio_buffer_list.count;
    }
    while(!AtomicCompareAndSwap(&audio_buffer_list.lock, 1, 0)) {}
  }
  
  arena_release_scratch(scratch);
}

//
// rendering
//
typedef enum RenderLevel
{
  RenderLevel_top,
  RenderLevel_label,
  RenderLevel_grid,
  RenderLevel_signal,
  RenderLevel_background,
  RenderLevel_count,
} RenderLevel;

#define RenderLevel(level) ((R32)(RenderLevel_##level)/(R32)(RenderLevel_count))

//
// application
//
typedef struct SpectrogramState
{
  Arena *arena;
  
  SineOscState *sine_osc_state;

  RangeR32 freq_rng;
  RangeR32 db_rng;

  U32 sample_rate;
  U32 spectrum_sample_count;

  ComplexBuffer cached_spectrum;
} SpectrogramState;

proc SpectrogramState*
spectrogram_state_alloc(Arena *arena)
{
  SpectrogramState *result = arena_push_struct(arena, SpectrogramState);
  result->arena = arena;
  
  result->sine_osc_state = arena_push_struct(arena, SineOscState);
  result->sine_osc_state->phasor = 0.f;
  result->sine_osc_state->base_freq = 440.f;
  result->sine_osc_state->pitch_bend = 1.f;
  result->sine_osc_state->volume = 0.1f;

  result->freq_rng.min = 2.f;
  result->freq_rng.max = 20000.f;

  result->db_rng.min = -90.f;
  result->db_rng.max = 0.f;

  result->sample_rate = 48000;

  // TODO: make size less hacky, and handle caching multiple spectrum sizes
  //U32 cached_spectrum_count = 1024;
  result->spectrum_sample_count = 2048;
  result->cached_spectrum.count = result->spectrum_sample_count;
  result->cached_spectrum.re = arena_push_array(arena, R32, result->spectrum_sample_count);
  result->cached_spectrum.im = arena_push_array(arena, R32, result->spectrum_sample_count);

  return(result);
}

// TODO: make the commands and font globally accessible, or accessible off the spectrogram state
proc void
draw_spectrum_grid_log_db(SpectrogramState *spec_state, V2S32 window_dim, R_Font *font)
{
  ArenaTemp scratch = arena_get_scratch(0, 0);
  
  U32 freq_line_count = (U32)log10f(spec_state->freq_rng.max/spec_state->freq_rng.min) + 1;
  U32 db_line_count = (U32)((spec_state->db_rng.max - spec_state->db_rng.min)/6.f) + 1;

  R32 freq_line_major_advance = (R32)window_dim.width/(R32)(freq_line_count - 1);
  R32 db_line_major_advance = (R32)window_dim.height/(R32)(db_line_count - 1);

  V2 window_dimf = v2((R32)window_dim.x, (R32)window_dim.y);

  U32 current_freq = (U32)spec_state->freq_rng.min;
  V2 freq_label_pos = v2_hadamard(v2(0.005f, 0.025f), window_dimf);
  U32 freq_line_thickness_px = 5;
  U32 freq_line_minor_thickness_px = 2;
  V2 freq_line_pos = v2(0, 0.5f*(R32)window_dim.height);
  V2 freq_line_dim = v2((R32)freq_line_thickness_px, (R32)window_dim.height);
  V2 freq_line_minor_dim = v2((R32)freq_line_minor_thickness_px, (R32)window_dim.height);
  for(U32 freq_line_idx = 0; freq_line_idx < freq_line_count; ++freq_line_idx) {

    Rect2 freq_line = rect2_center_dim(freq_line_pos, freq_line_dim);
    render_rect(0, freq_line, rect2_invalid(), RenderLevel(grid), v4(1, 1, 1, 1));

    if(freq_line_idx < freq_line_count - 1) {

      U32 next_freq = current_freq*10;
      R32 sep = ((R32)next_freq - (R32)current_freq) * 0.1f;
      for(U32 minor_line_idx = 1; minor_line_idx < 10; ++minor_line_idx) {

	R32 offset = freq_line_major_advance*log10f(((R32)current_freq + sep*(R32)minor_line_idx)/(R32)current_freq);
	V2 minor_line_pos = v2(freq_line_pos.x + offset, freq_line_pos.y);
	Rect2 minor_line = rect2_center_dim(minor_line_pos, freq_line_minor_dim);
	render_rect(0, minor_line, rect2_invalid(), RenderLevel(grid), v4(1, 1, 1, 1));
      }
    }
    
    String8 freq_label = str8_push_f(scratch.arena, "%u", current_freq);
    render_string(font, freq_label, freq_label_pos, RenderLevel(label), v4(1, 1, 1, 1));

    current_freq *= 10;
    freq_line_pos.x += freq_line_major_advance;
    freq_label_pos.x += freq_line_major_advance;
  }

  S32 current_db = (S32)spec_state->db_rng.min;
  V2 db_label_pos = v2_hadamard(v2(0.025f, 0.005f), window_dimf);
  U32 db_line_thickness_px = 5;
  V2 db_line_pos = v2(0.5f*(R32)window_dim.width, 0);
  V2 db_line_dim = v2((R32)window_dim.width, (R32)db_line_thickness_px); 
  for(U32 db_line_idx = 0; db_line_idx < db_line_count; ++db_line_idx) {

    Rect2 db_line = rect2_center_dim(db_line_pos, db_line_dim);
    render_rect(0, db_line, rect2_invalid(), RenderLevel(grid), v4(1, 1, 1, 1));

    String8 db_label = str8_push_f(scratch.arena, "%d", current_db);
    render_string(font, db_label, db_label_pos, RenderLevel(label), v4(1, 1, 1, 1));

    current_db += 6;
    db_line_pos.y += db_line_major_advance;
    db_label_pos.y += db_line_major_advance;    
  }

  arena_release_scratch(scratch);
}

proc void
draw_spectrum_grid_lin(SpectrogramState *spec_state, R_Font *font)
{
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U32 freq_space = 6000;
  U32 amp_space = 400;

  /* U32 freq_line_count = 4; */
  /* U32 amp_line_count = 6; */
  U32 freq_line_count = (U32)(ranger32_len(spec_state->freq_rng) + 1) / freq_space;
  U32 amp_line_count = 2400 / amp_space;

  U32 current_freq = 0;
  U32 freq_line_thickness_px = 5;
  //R32 freq_line_thickness = render_commands->ndc_scale.x * freq_line_thickness_px;
  R32 freq_line_space = 2.f/(R32)freq_line_count;
  V2 freq_line_pos = v2(-1, 0);
  V2 freq_label_pos = v2(-0.99f, -0.95f);
  for(U32 freq_line_idx = 0; freq_line_idx < freq_line_count; ++freq_line_idx) {

    Rect2 freq_line = rect2_center_dim(freq_line_pos, v2((R32)freq_line_thickness_px, 2.f));
    render_rect(0, freq_line, rect2_invalid(), RenderLevel(grid), v4(1, 1, 1, 1));

    String8 freq_label = str8_push_f(scratch.arena, "%u", current_freq);
    render_string(font, freq_label, freq_label_pos, RenderLevel(label), v4(1, 1, 1, 1));

    current_freq += freq_space;
    freq_line_pos.x += freq_line_space;
    freq_label_pos.x += freq_line_space;
  }

  U32 current_amp = 0;
  U32 amp_line_thickness_px = 5;
  //R32 amp_line_thickness = render_commands->ndc_scale.y * amp_line_thickness_px;
  R32 amp_line_space = 2.f/(R32)amp_line_count;
  V2 amp_line_pos = v2(0, -1);
  V2 amp_label_pos = v2( -0.95f, -0.99f);
  for(U32 amp_line_idx = 0; amp_line_idx < amp_line_count; ++amp_line_idx) {

    Rect2 amp_line = rect2_center_dim(amp_line_pos, v2(2.f, (R32)amp_line_thickness_px));
    render_rect(0, amp_line, rect2_invalid(), RenderLevel(grid), v4(1, 1, 1, 1));

    String8 amp_label = str8_push_f(scratch.arena, "%u", current_amp);
    render_string(font, amp_label, amp_label_pos, RenderLevel(label), v4(1, 1, 1, 1));

    current_amp += amp_space;
    amp_line_pos.y += amp_line_space;
    amp_label_pos.y += amp_line_space;
  }

  arena_release_scratch(scratch);
}

proc void
draw_spectrum_log_db(SpectrogramState *spec_state, V2S32 window_dim, ComplexBuffer spec_buf)
{
#if 1
  U32 bin_count = (U32)spec_buf.count/2;
  R32 norm_coeff = 1.f/(R32)(spec_buf.count*spec_buf.count);
  R32 nyquist = 0.5f * (R32)spec_state->sample_rate;
  RangeR32 log_freq_rng = {.min = log10f(spec_state->freq_rng.min), .max = log10f(spec_state->freq_rng.max)};

  R32 window_width  = (R32)window_dim.width;
  R32 window_height = (R32)window_dim.height;

  V2 pos = v2(0, 0);
  for(U32 bin_idx = 0; bin_idx < bin_count - 1; ++bin_idx) {

    R32 bin_re = spec_buf.re[bin_idx];
    R32 bin_im = spec_buf.im[bin_idx];
    R32 bin_power = bin_re*bin_re + bin_im*bin_im;
    R32 bin_db = 10.f*log10f(bin_power * norm_coeff);
    R32 bin_height = window_height * ranger32_map_01(bin_db, spec_state->db_rng);
    
    R32 next_bin_freq = ((R32)(bin_idx + 1)/(R32)bin_count) * nyquist;
    R32 log_next_bin_freq = log10f(next_bin_freq);

    R32 next_pos_x = window_width * ranger32_map_01(log_next_bin_freq, log_freq_rng) - 1.f;
    R32 width = next_pos_x - pos.x;

    Rect2 bin = rect2_min_dim(pos, v2(width, bin_height));
    render_rect(0, bin, rect2_invalid(), RenderLevel(signal), v4(0, 0.5f, 0.5f, 1));

#  if 0
    // NOTE: debug
    fprintf(stderr, "bin_idx=%u, next_freq=%.4f, log_next_freq=%.4f, pos.x=%.2f\n",
	    bin_idx, next_bin_freq, log_next_bin_freq, pos.x);
#  endif

    pos.x = next_pos_x;
  }
#else
  U32 bin_count = spec_buf.count/2;
  R32 norm_coeff = 1.f/(R32)(spec_buf.count*spec_buf.count);
  R32 width = 2.f/(R32)bin_count;	    
  R32 pos_x = -1.f;

#  if 0
  fprintf(stderr, "log power spectrum:\n"); // DEBUG
#  endif

  R32 step = log10f(spec_state->freq_rng.max/spec_state->freq_rng.min)/(R32)bin_count;
  R32 exp = log10f(spec_state->freq_rng.min);
  for(U32 i = 0; i < bin_count; ++i) {

    R32 freq = powf(10.f, exp);

    R32 freq_bin_pos = (freq*(R32)spec_buf.count)/(R32)spec_state->sample_rate;
    U32 freq_bin_idx = (U32)freq_bin_pos;
    R32 freq_bin_frac = freq_bin_pos - (R32)freq_bin_idx;
	      
    R32 bin_re_0 = spec_buf.re[freq_bin_idx];
    R32 bin_re_1 = spec_buf.re[freq_bin_idx + 1];
	      
    R32 bin_im_0 = spec_buf.im[freq_bin_idx];
    R32 bin_im_1 = spec_buf.im[freq_bin_idx + 1];

    R32 bin_mag_sq_0 = bin_re_0*bin_re_0 + bin_im_0*bin_im_0;
    R32 bin_mag_sq_1 = bin_re_1*bin_re_1 + bin_im_1*bin_im_1;
    R32 bin_mag_sq = lerp(bin_mag_sq_0, bin_mag_sq_1, freq_bin_frac);

    R32 bin_mag_db = 10.f*log10f(bin_mag_sq * norm_coeff);
    R32 bin_height = ranger32_map_01(bin_mag_db, spec_state->db_rng);
    R32 bin_height_ndc = 2.f * bin_height;

    Rect2 bin_rect = rect2_min_dim(v2(pos_x, -1), v2(width, bin_height_ndc));
    render_rect(render_commands, 0, bin_rect, rect2_invalid(), render_level(RenderLevel_signal), v4(0, 0.5f, 0.5f, 1));

#  if 0
    // DEBUG:
    fprintf(stderr, "%3u (%.2f): %.4f\n", i, freq, bin_mag_sq);
    fprintf(stderr, "  access (idx, frac): %3u, %.2f\n", freq_bin_idx, freq_bin_frac);
    fprintf(stderr, "  bin_mag_sq(0, 1) = %.4f, %.4f\n", bin_mag_sq_0, bin_mag_sq_1);
#  endif
    exp += step;
    pos_x += width;
  }
#endif
}

proc void
draw_spectrum_lin(SpectrogramState *spec_state, ComplexBuffer spec_buf)
{
  U32 bin_count = (U32)spec_buf.count/2;
  R32 width = 2.f/(R32)bin_count;
  Unused(spec_state);

  V2 pos = v2(-1, -1);
  for(U32 bin_idx = 0; bin_idx < bin_count; ++bin_idx) {

    R32 bin_re = spec_buf.re[bin_idx];
    R32 bin_im = spec_buf.im[bin_idx];

    R32 bin_power = bin_re*bin_re + bin_im*bin_im;
    R32 bin_height = 2.f*bin_power/2400.f;
    
    Rect2 bin_rect = rect2_min_dim(pos, v2(width, bin_height));
    render_rect(0, bin_rect, rect2_invalid(), RenderLevel(signal), v4(0, 0.5f, 0.5f, 1));
    
    pos.x += width;
  }
}

proc void
draw_spectrum_grid(SpectrogramState *spec_state, V2S32 window_dim, R_Font *font)
{
  draw_spectrum_grid_log_db(spec_state, window_dim, font);
  //draw_spectrum_grid_lin(spec_state, window_dim, font);
}

proc void
draw_spectrum(SpectrogramState *spec_state, V2S32 window_dim, ComplexBuffer spec_buf)
{
#if 0
  // DEBUG:
  U32 bin_count = spec_buf.count/2;
  fprintf(stderr, "linear power spectrum:\n");
  for(U32 bin_idx = 0; bin_idx < bin_count; ++bin_idx) {

    R32 bin_freq = bin_idx * 24000.f / (R32)bin_count;
    R32 bin_re = spec_buf.re[bin_idx];
    R32 bin_im = spec_buf.im[bin_idx];
    R32 bin_power = bin_re*bin_re + bin_im*bin_im;

    fprintf(stderr, "%3u (%.2f): %.4f\n", bin_idx, bin_freq, bin_power);
  }
#endif

  draw_spectrum_log_db(spec_state, window_dim, spec_buf);
  //draw_spectrum_lin(spec_state, window_dim, spec_buf);
}

//
// entry point
//
#if OS_WINDOWS
int
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
#else
int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);
#endif

  int result = 0;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  if(os_init()) {
    if(os_gfx_init()) {

      Os_Handle window = os_open_window(Str8Lit("spectrogram"), 640, 480);
      if(window.handle) {      

	Arena *state_arena = arena_alloc();
	SpectrogramState *spec_state = spectrogram_state_alloc(state_arena);

	if(audio_init(Str8Lit("spectrogram audio"))) {

	  audio_buffer_list.arena = arena_alloc();
	  audio_set_user_data(spec_state->sine_osc_state);
	  spec_state->sample_rate = audio_get_sample_rate();
	}

	render_init(); // TODO: should check success
	String8 font_file_path = Str8Lit(DATA_DIR"/font/LiberationMono-Regular.ttf");
	U32 font_pt_size = 32;
	LooseFont loose_font = font_parse(scratch.arena, font_file_path, font_pt_size);
	R_Font *font = render_alloc_font(&loose_font);

	// DEBUG: print glyph info
#if 0
	{
	  String8List glyph_data_list = {0};
	  str8_list_push_f(scratch.arena, &glyph_data_list, "glyph count: %u\n\n", font->glyph_count);

	  U32 glyph_idx = 0;
	  PackedGlyph *opl = font->glyphs + font->glyph_count;
	  for(PackedGlyph *glyph = font->glyphs; glyph < opl; ++glyph, ++glyph_idx) {

	    U32 cp = font_codepoint_from_glyph_index(font, glyph_idx);
	    str8_list_push_f(scratch.arena, &glyph_data_list,
			     "glyph %u:\n  codepoint: %u\n  "
			     "rect_min: (%.4f, %.4f)\n  rect_max: (%.4f, %.4f)\n  "
			     "uv_min: (%.4f, %.4f)\n  uv_max: (%.4f, %.4f)\n\n",
			     glyph_idx, cp,
			     glyph->rect.min.x, glyph->rect.min.y,
			     glyph->rect.max.x, glyph->rect.max.y,
			     glyph->uv.min.x, glyph->uv.min.y,
			     glyph->uv.max.x, glyph->uv.max.y);
	  }
	  String8 glyph_data = str8_join(scratch.arena, &glyph_data_list);
	  os_write_entire_file(glyph_data,
			       Str8Lit(DATA_DIR"/test/liberation_mono_glyph_data__dwrite.txt"));
	}
#endif

	B32 running = 1;
	Arena *frame_arena = arena_alloc();
	while(running) {
	
	  //Os_EventList events = os_events_from_window(window);
	  //for(Os_EventNode *event_node = events.first; event_node; event_node = event_node->next) {
	  Os_EventList events = os_events(frame_arena);
	  for(Os_Event *event = events.first; event; event = event->next) {
	    if(event->window.handle == window.handle) {
	      //switch(event_node->event.kind) {
	      switch(event->kind) {
	      case Os_EventKind_close:
		{ running = 0; } break;
	      default:
		{} break;
	      }
	    }
	  }
	
	  render_equip_window(window);
	  render_begin_frame();

	  V2S32 window_dim = os_window_get_dim(window);
	  Rect2 screen_rect = rect2_min_dim(v2(0, 0), v2_from_v2s32(window_dim));
	  V4 background_color = v4(0.09411f, 0.10196f, 0.14902f, 1);
	  render_rect(0, screen_rect, rect2_invalid(), RenderLevel(background), background_color);

	  render_string(font, Str8Lit("Testing testing 1 2 1 2 ..."), rect2_center(screen_rect), RenderLevel(label), v4(1, 1, 1, 1));

	  draw_spectrum_grid(spec_state, window_dim, font);
	  /* render_rect(&font->atlas, screen_rect, rect2(v2(0, 0), v2(1, 1)), */
	  /* 	    RenderLevel(label), v4(1, 1, 1, 1)); */
	

#if 1
	  // NOTE: drawing the spectrum
	  {
	    // NOTE: try to dequeue buffers
	    AudioBufferNode *first_node = 0;
	    AudioBufferNode *last_node = 0;
	    U32 buffer_count = 0;
	    while(!AtomicCompareAndSwap(&audio_buffer_list.lock, 0, 1)) {}
	    {
	      first_node = audio_buffer_list.first;
	      last_node = audio_buffer_list.last;
	      buffer_count = (U32)audio_buffer_list.count;
	      audio_buffer_list.first = 0;
	      audio_buffer_list.last = 0;
	      audio_buffer_list.count = 0;
	    }
	    while(!AtomicCompareAndSwap(&audio_buffer_list.lock, 1, 0)) {}

	    fprintf(stderr, "dequeued %u buffers\n", buffer_count);

	    // NOTE: if we dequeued buffers, draw their spectra.
	    //       otherwise draw the cached spectrum
	    if(first_node) {
	      // TODO: how to handle multiple buffer nodes?
	      //for(AudioBufferNode *node = first_node; node; node = node->next) {
	      for(AudioBufferNode *node = first_node; node; node = 0) {

		AudioBuffer *buf = &node->buf;
		FloatBuffer sample_buf = {.count = buf->sample_count, .mem = buf->samples[0]};

#if 0
		// DEBUG:
		for(U32 sample_idx = 0; sample_idx < buf->sample_count; ++sample_idx) {

		  fprintf(stderr, "%4u: %.4f\n",
			  sample_idx,
			  buf->samples[0][sample_idx]);
		}
#endif
		// TODO: enable windowing in ui
#if 0
		// NOTE: window_input
		// TODO: maybe don't overwrite the buffer?
		for(U32 sample_idx = 0; sample_idx < sample_buf.count; ++sample_idx) {

		  R32 window_val = 0.5f*(cosf(TAU32*((R32)sample_idx/(R32)sample_buf.count - 0.5f)) + 1.f);
		  sample_buf.mem[sample_idx] *= window_val;
		}
#endif

		Assert(sample_buf.count <= spec_state->cached_spectrum.count);
		FloatBuffer spectrum_in = {0};
		spectrum_in.count = spec_state->cached_spectrum.count;
		spectrum_in.mem = arena_push_array_z(frame_arena, R32, spec_state->cached_spectrum.count);
		CopyArray(spectrum_in.mem, sample_buf.mem, R32, sample_buf.count);
		//ComplexBuffer spectrum_buf = fft_re(frame_arena, sample_buf);
		ComplexBuffer spectrum_buf = fft_re(frame_arena, spectrum_in);
		draw_spectrum(spec_state, window_dim, spectrum_buf);

		// NOTE: cache spectrum
		// TODO: handle the case when these have different lengths
		//Assert(spec_state->cached_spectrum.count >= spectrum_buf.count);
		CopyArray(spec_state->cached_spectrum.re, spectrum_buf.re, R32, spectrum_buf.count);
		CopyArray(spec_state->cached_spectrum.im, spectrum_buf.im, R32, spectrum_buf.count);
	      }
	    }else {

	      draw_spectrum(spec_state, window_dim, spec_state->cached_spectrum);
	    }

	    // NOTE: if we dequeued buffers, put them on the freelist
	    if(last_node) {
	      while(!AtomicCompareAndSwap(&audio_buffer_list.lock, 0, 1)) {}
	      {
		last_node->next = audio_buffer_list.freelist;
		audio_buffer_list.freelist = last_node;	  
	      }
	      while(!AtomicCompareAndSwap(&audio_buffer_list.lock, 1, 0)) {}
	    }
	  }
#endif

	  render_end_frame();
	  arena_clear(frame_arena);
	}

	audio_uninit();
	os_close_window(window);
      }
    }
  }

  arena_release_scratch(scratch);
  return(result);
}
