#include "base/base.h"
#include "gfx/gfx.h"
#include "font/font.h"
#include "render/render.h"
#include "audio/audio.h"
#include "fourier/fourier.h"

#include "base/base.c"
#include "gfx/gfx.c"
#include "font/font.c"
#include "render/render.c"
#include "audio/audio.c"
#include "fourier/fourier.c"

// TODO:
// - logging in window
// - dynamic audio sources
// - plots:
//   - make text look less shit (on coretext)
//   - dynamic axis ranges
//   - linear or log frequency axis
//   - linear or db spectrum amplitude axis
//   - magnitude or power spectrum

typedef struct ScopeStream
{
  Audio_Stream self;
  Audio_Stream *source;
  Os_RingBuffer *shared_samples;
} ScopeStream;

proc Audio_StreamStatus
scope_stream_refill(Audio_Stream *self, Audio_Stream *caller)
{
  ScopeStream *stream = (ScopeStream*)self;
  Audio_Stream *source = stream->source;
  Os_RingBuffer *buffer = stream->shared_samples;

  Audio_StreamStatus result = Audio_StreamStatus_ok;

  // NOTE: refill
  source->refill(source, self);
  U64 samples_available = source->samples_end - source->sample_cursor;

  // NOTE: fill buffer, passthru
  Assert(caller->refill == 0); // caller must be an output stream
  U64 output_samples_to_write = caller->samples_end - caller->sample_cursor;

  SpanU8 write_span = os_ring_buffer_write_span(buffer);
  U64 buffer_samples_to_write = span_count(write_span, R32);

  R32 *src = source->sample_cursor;
  R32 *out_dest = caller->sample_cursor;
  R32 *buffer_dest = (R32*)write_span.start;
  if(samples_available == 0)
  {
    Assert(buffer_samples_to_write >= output_samples_to_write);
    ZeroArray(out_dest, R32, output_samples_to_write);
    ZeroArray(buffer_dest, R32, output_samples_to_write);

    caller->sample_cursor = caller->samples_end;
    os_ring_buffer_write_end(buffer, R32, output_samples_to_write);

    result = Audio_StreamStatus_zero_output;
  }
  else
  {
    U64 samples_to_write = Min(output_samples_to_write, samples_available);
    CopyArray(out_dest, src, R32, samples_to_write);

    caller->sample_cursor += samples_to_write;
    source->sample_cursor += samples_to_write;

    if(buffer_samples_to_write >= samples_to_write)
    {
      CopyArray(buffer_dest, src, R32, samples_to_write);
      os_ring_buffer_write_end(buffer, R32, samples_to_write);
    }
  }

  return result;
}

proc void
scope_stream_init(ScopeStream *stream, Audio_Stream *source, Os_RingBuffer *buffer)
{
  stream->self.refill = scope_stream_refill;
  stream->source = source;
  stream->shared_samples = buffer;
}

typedef enum DrawMode
{
  DrawMode_time_domain,
  DrawMode_frequency_domain,
  DrawMode_spectrogram,
} DrawMode;

#define RenderLevel(l)\
  ((R32)Glue(RenderLevel_, l)/(R32)RenderLevel_Count)
typedef enum RenderLevel
{
  RenderLevel_front,
  RenderLevel_signal,
  RenderLevel_label,
  RenderLevel_axis,
  RenderLevel_line,
  RenderLevel_background,
  RenderLevel_Count,
} RenderLevel;

proc void
draw_spectrum(R_Font *font, Rect2 region, R32 *samples, U64 sample_count, U64 sample_rate)
{
  ArenaTemp scratch = arena_get_scratch(0, 0);

  R32 min_freq = 0;
  R32 max_freq = sample_rate / 2;

  U32 freq_label_count = 9;
  U32 amp_label_count = 5;

  R32 min_amp = 0;
  R32 max_amp = 0.125f*512.f;

  // NOTE: draw axis labels
  {
    V2 region_dim = rect2_dim(region);

    // frequency
    Rect2 freqs_rect = rect2_invalid();
    for(U32 freq_label_idx = 1; freq_label_idx < freq_label_count; ++freq_label_idx)
    {
      R32 freq_frac = (R32)freq_label_idx / (R32)(freq_label_count - 1);
      R32 freq = freq_frac * (max_freq - min_freq);
      String8 freq_str = str8_push_f(scratch.arena, "%u", (U32)freq);
      V2 freq_pos = v2(region.min.x + freq_frac * region_dim.x, region.min.y);
      Rect2 freq_rect = render_string(font, freq_str, freq_pos, RenderLevel(label), v4(1, 1, 1, 1));
      freqs_rect = rect2_union(freqs_rect, freq_rect);
    }

    // amplitude
    Rect2 amps_rect = rect2_invalid();
    for(U32 amp_label_idx = 1; amp_label_idx < amp_label_count; ++amp_label_idx)
    {
      R32 amp_frac = (R32)amp_label_idx / (R32)(amp_label_count - 1);
      R32 amp = amp_frac * (max_amp - min_amp);
      String8 amp_str = str8_push_f(scratch.arena, "%u", (U32)amp);
      V2 amp_pos = v2(region.min.x, region.min.y + amp_frac * region_dim.y);
      Rect2 amp_rect = render_string(font, amp_str, amp_pos, RenderLevel(label), v4(1, 1, 1, 1));
      amps_rect = rect2_union(amps_rect, amp_rect);
    }

    region.min.x = amps_rect.max.x;
    region.min.y = freqs_rect.max.y;
  }

  // NOTE: draw axes
  {
    V2 region_dim = rect2_dim(region);
    V4 axis_color = color_v4_from_rgba(0xA8, 0xA8, 0xA8, 0xFF);
    R32 axis_thickness = 3.f;

    Rect2 freq_axis = rect2_min_dim(region.min, v2(region_dim.x, axis_thickness));
    render_rect(freq_axis, 0, RenderLevel(axis), axis_color);

    Rect2 amp_axis = rect2_min_dim(region.min, v2(axis_thickness, region_dim.y));
    render_rect(amp_axis, 0, RenderLevel(axis), axis_color);

    region.min.x = amp_axis.max.x;
    region.min.y = freq_axis.max.y;
  }

  // NOTE: draw lines
  {
    V2 region_dim = rect2_dim(region);
    V4 line_color = color_v4_from_rgba(0x55, 0x55, 0x55, 0xFF);
    R32 line_thickness = 2.f;

    // freq lines
    U32 freq_line_count = freq_label_count;
    for(U32 line_idx = 1; line_idx < freq_line_count; ++line_idx)
    {
      R32 line_frac = (R32)line_idx / (R32)(freq_line_count - 1);
      R32 line_x = line_frac * region_dim.x; // TODO: discrepancy w/ above?
      Rect2 line = rect2_min_dim(v2(region.min.x + line_x, region.min.y), v2(line_thickness, region_dim.y));
      render_rect(line, 0, RenderLevel(line), line_color);
    }

    // amp lines
    U32 amp_line_count = amp_label_count;
    for(U32 line_idx = 1; line_idx < amp_line_count; ++line_idx)
    {
      R32 line_frac = (R32)line_idx / (R32)(amp_line_count - 1);
      R32 line_y = line_frac * region_dim.y; // TODO: discrepancy w/ above?
      Rect2 line = rect2_min_dim(v2(region.min.x, region.min.y + line_y), v2(region_dim.x, line_thickness));
      render_rect(line, 0, RenderLevel(line), line_color);
    }
  }

  if(sample_count)
  {
    Assert(IsPow2(sample_count));
    U32 bin_count = sample_count / 2;
    C64 *spectrum = arena_push_array(scratch.arena, C64, bin_count);
    fft_re(spectrum, samples, sample_count);

    V2 region_dim = rect2_dim(region);
    V4 bar_color = color_v4_from_rgba(0xFF, 0xC1, 0x25, 0xFF);
    R32 bin_width = region_dim.x / (R32)bin_count;

    for(U32 bin_idx = 0; bin_idx < bin_count; ++bin_idx)
    {
      V2 bin_pos = v2(region.min.x + (R32)bin_idx * bin_width, region.min.y);
      C64 bin_val = spectrum[bin_idx];
      R32 bin_mag_sq = c64_mag_sq(bin_val);
      R32 bar_height = bin_mag_sq / max_amp * region_dim.y;
      Rect2 bar = rect2_min_dim(bin_pos, v2(bin_width, bar_height));
      render_rect(bar, 0, RenderLevel(signal), bar_color);
    }
  }

  arena_release_scratch(scratch);
}

proc void
draw_samples(R_Font *font, Rect2 region, R32 *samples, U64 sample_count)
{
  ArenaTemp scratch = arena_get_scratch(0, 0);

  R32 time_min = 0;
  R32 time_max = 512.f;

  R32 amp_min = -1.f;
  R32 amp_max = 1.f;

  U32 time_label_count = 5;
  U32 amp_label_count = 3;

  // NOTE: draw axis labels
  {
    V2 region_dim = rect2_dim(region);

    // time
    Rect2 times_rect = rect2_invalid();
    for(U32 time_label_idx = 0; time_label_idx < time_label_count; ++time_label_idx)
    {
      R32 time_frac = (R32)time_label_idx / (R32)(time_label_count - 1);
      R32 time = time_frac * (time_max - time_min) + time_min;
      String8 time_str = str8_push_f(scratch.arena, "%u", (U32)time);
      V2 time_pos = v2(region.min.x + time_frac*region_dim.x, region.min.y);
      Rect2 time_rect = render_string(font, time_str, time_pos, RenderLevel(label), v4(1, 1, 1, 1));
      times_rect = rect2_union(times_rect, time_rect);
    }

    // amplitude
    Rect2 amps_rect = rect2_invalid();
    for(U32 amp_label_idx = 0; amp_label_idx < amp_label_count; ++amp_label_idx)
    {
      R32 amp_frac = (R32)amp_label_idx / (R32)(amp_label_count - 1);
      R32 amp = amp_frac * (amp_max - amp_min) + amp_min;
      String8 amp_str = str8_push_f(scratch.arena, "%u", (U32)amp);
      V2 amp_pos = v2(region.min.x, region.min.y + amp_frac*region_dim.y);
      Rect2 amp_rect = render_string(font, amp_str, amp_pos, RenderLevel(label), v4(1, 1, 1, 1));
      amps_rect = rect2_union(amps_rect, amp_rect);
    }

    region.min.x = amps_rect.max.x;
    region.min.y = times_rect.max.y;
  }

  // NOTE: draw axes
  {
    V2 region_dim = rect2_dim(region);
    V4 axis_color = color_v4_from_rgba(0xA8, 0xA8, 0xA8, 0xFF);
    R32 axis_thickness = 3.f;

    Rect2 time_axis = rect2_min_dim(region.min, v2(region_dim.x, axis_thickness));
    render_rect(time_axis, 0, RenderLevel(axis), axis_color);

    Rect2 amp_axis = rect2_min_dim(region.min, v2(axis_thickness, region_dim.y));
    render_rect(amp_axis, 0, RenderLevel(axis), axis_color);

    region.min.x = amp_axis.max.x;
    region.min.y = time_axis.max.y;
  }

  // NOTE: draw lines
  {
    V2 region_dim = rect2_dim(region);
    V4 line_color = color_v4_from_rgba(0x55, 0x55, 0x55, 0xFF);
    R32 line_thickness = 2.f;

    // time lines
    U32 time_line_count = time_label_count;
    for(U32 line_idx = 1; line_idx < time_line_count; ++line_idx)
    {
      R32 line_frac = (R32)line_idx / (R32)(time_line_count - 1);
      R32 line_x = line_frac*region_dim.x; // TODO: discrepancy w/ above?
      Rect2 line = rect2_min_dim (v2(region.min.x + line_x, region.min.y), v2(line_thickness, region_dim.y));
      render_rect(line, 0, RenderLevel(line), line_color);
    }

    // amp lines
    U32 amp_line_count = amp_label_count;
    for(U32 line_idx = 1; line_idx < amp_line_count; ++line_idx)
    {
      R32 line_frac = (R32)line_idx / (R32)(amp_line_count - 1);
      R32 line_y = line_frac*region_dim.y; // TODO: discrepancy w/ above?
      Rect2 line = rect2_min_dim(v2(region.min.x, region.min.y + line_y), v2(region_dim.x, line_thickness));
      render_rect(line, 0, RenderLevel(line), line_color);
    }
  }

  if(sample_count)
  {
    V2 region_dim = rect2_dim(region);
    V2 region_center = rect2_center(region);
    V4 sample_color = color_v4_from_rgba(0x00, 0xFF, 0x7f, 0xFF);
    //R32 sample_rect_width = region_dim.x / (R32)sample_count;
    R32 sample_line_thickness = 2.f;
    V2 last_sample_pos = v2(0, region_center.y);

    for(U64 sample_idx = 0; sample_idx < sample_count; ++sample_idx)
    {
      R32 sample_pos_x = ((R32)sample_idx/(R32)sample_count)*region_dim.x;
      R32 sample = samples[sample_idx];
      R32 sample_height = region_center.y + ((sample - amp_min)/(amp_max - amp_min) - 0.5f)*region_dim.y;
      V2 sample_pos = v2(sample_pos_x, sample_height);
      render_line_segment(last_sample_pos, sample_pos, sample_line_thickness, 0, sample_color);

      last_sample_pos = sample_pos;
    }
  }

  arena_release_scratch(scratch);
}

typedef struct SpectrogramState
{
  S32 cursor;
  R_Texture tex;
} SpectrogramState;

proc void
spectrogram_init(SpectrogramState *spec_state, U64 buffer_size, U64 frame_count)
{
  Assert(IsPow2(buffer_size));
  Assert(IsPow2(frame_count));
  spec_state->cursor = 0;
  spec_state->tex = render_create_texture(buffer_size/2, frame_count, 0, .wrap_y = 1);
}

proc void
update_spectrogram(SpectrogramState *spec, R32 *samples, U64 sample_count)
{
  RangeR32 bin_mag_sq_range = range_r32(0, 32.f);

  if(sample_count)
  {
    Assert(IsPow2(sample_count));
    ArenaTemp scratch = arena_get_scratch(0, 0);

    U64 bin_count = sample_count/2;
    C64 *spectrum = arena_push_array(scratch.arena, C64, bin_count);
    fft_re(spectrum, samples, sample_count);

    V4 low_color = color_v4_from_rgba(0x08, 0x0C, 0x1C, 0x00);
    V4 high_color = color_v4_from_rgba(0xFF, 0xC1, 0x25, 0xFF);
    U32 *spec_pixels = arena_push_array(scratch.arena, U32, bin_count);
    for(U64 bin_idx = 0; bin_idx < bin_count; ++bin_idx)
    {
      C64 bin_val = spectrum[bin_idx];
      R32 bin_mag_sq = c64_mag_sq(bin_val);

      R32 bin_mag_sq_01 = range_r32_map_01(bin_mag_sq, bin_mag_sq_range);
      V4 color_v4 = color_blend_v4(low_color, high_color, bin_mag_sq_01);
      U32 color = color_u32_from_v4(color_v4);
      spec_pixels[bin_idx] = color;
    }

    render_update_texture(&spec->tex, 0, spec->cursor, spec->tex.dim.width, 1, R_PixelFormat_rgba, spec_pixels);
    ++spec->cursor;
    spec->cursor &= (spec->tex.dim.height - 1);

    arena_release_scratch(scratch);
  }
}

proc void
draw_spectrogram(R_Font *font, SpectrogramState *spec, Rect2 region, U64 sample_rate)
{
  RangeR32 freq_range = range_r32(0, (R32)(sample_rate/2));
  RangeR32 time_range = range_r32(-(R32)spec->tex.dim.height*spec->tex.dim.width*2/(R32)sample_rate, 0);

  U32 freq_label_count = 4;
  U32 time_label_count = 5;

  // NOTE: draw labels
  {
    V2 region_dim = rect2_dim(region);

    ArenaTemp scratch = arena_get_scratch(0, 0);

    // freq
    Rect2 freqs_rect = rect2_invalid();
    for(U32 freq_label_idx = 0; freq_label_idx < freq_label_count; ++freq_label_idx)
    {
      R32 freq_frac = (R32)freq_label_idx / (R32)(freq_label_count - 1);
      U32 freq = (U32)range_r32_map(freq_frac, freq_range);
      String8 freq_label = str8_push_f(scratch.arena, "%u", freq);
      V2 freq_pos = v2_add_x(region.min, freq_frac * region_dim.x);
      Rect2 freq_rect = render_string(font, freq_label, freq_pos, RenderLevel(label), v4(1, 1, 1, 1));
      freqs_rect = rect2_union(freqs_rect, freq_rect);
    }

    // time
    Rect2 times_rect = rect2_invalid();
    for(U32 time_label_idx = 0; time_label_idx < time_label_count; ++time_label_idx)
    {
      R32 time_frac = (R32)time_label_idx / (R32)(time_label_count - 1);
      R32 time = range_r32_map(time_frac, time_range);
      String8 time_label = str8_push_f(scratch.arena, "%.2f", time);
      V2 time_pos = v2_add_y(region.min, time_frac * region_dim.y);
      Rect2 time_rect = render_string(font, time_label, time_pos, RenderLevel(label), v4(1, 1, 1, 1));
      times_rect = rect2_union(times_rect, time_rect);
    }

    arena_release_scratch(scratch);

    region.min.x = times_rect.max.x;
    region.min.y = freqs_rect.max.y;
  }

  // NOTE: draw axes
  {
    V2 region_dim = rect2_dim(region);
    R32 axis_thickness = 3.f;
    V4 axis_color = color_v4_from_rgba(0xA8, 0xA8, 0xA8, 0xFF);

    Rect2 freq_axis = rect2_min_dim(region.min, v2(region_dim.x, axis_thickness));
    render_rect(freq_axis, 0, RenderLevel(axis), axis_color);

    Rect2 time_axis = rect2_min_dim(region.min, v2(axis_thickness, region_dim.y));
    render_rect(time_axis, 0, RenderLevel(axis), axis_color);

    region.min.x = time_axis.max.x;
    region.min.y = freq_axis.max.y;
  }

  // NOTE: draw gridlines
  {
    V2 region_dim = rect2_dim(region);

    R32 line_thickness = 2.f;
    V4 line_color = color_v4_from_rgba(0x55, 0x55, 0x55, 0xFF);

    U32 freq_line_count = freq_label_count;
    U32 time_line_count = time_label_count;

    // freq
    for(U32 freq_line_idx = 1; freq_line_idx < freq_line_count; ++freq_line_idx)
    {
      R32 freq_line_frac = (R32)freq_line_idx / (R32)(freq_line_count - 1);
      V2 freq_pos = v2_add_x(region.min, freq_line_frac * region_dim.x); // TODO: discrepancy w/ above?
      Rect2 freq_line = rect2_min_dim(freq_pos, v2(line_thickness, region_dim.y));
      render_rect(freq_line, 0, RenderLevel(line), line_color);
    }

    // time
    for(U32 time_line_idx = 1; time_line_idx < time_line_count; ++time_line_idx)
    {
      R32 time_line_frac = (R32)time_line_idx / (R32)(time_line_count - 1);
      V2 time_pos = v2_add_y(region.min, time_line_frac * region_dim.y); // TODO: discrepancy w/ above?
      Rect2 time_line = rect2_min_dim(time_pos, v2(region_dim.x, line_thickness));
      render_rect(time_line, 0, RenderLevel(line), line_color);
    }
  }

  // NOTE: draw spectrogram
  {
    R32 cursor_pos_frac = (R32)spec->cursor / (R32)spec->tex.dim.height;
    V2 uv_min = v2(0, cursor_pos_frac);
    V2 uv_max = v2_add(uv_min, v2(1, 1));
    render_texture(&spec->tex, region, rect2(uv_min, uv_max), 0, RenderLevel(signal), v4(1, 1, 1, 1));
  }
}

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  if(!os_init())
  { return 1; }

  if(!gfx_init())
  { return 1; }

  if(!render_init())
  { return 1; }

  if(!audio_init())
  { return 1; }

  Arena *permanent_arena = arena_alloc();

  // NOTE: open window
  Gfx_Handle window = gfx_window_open(640, 480, Str8Lit("filter workbench"));
  render_set_backend(R_Backend_opengl);
  render_equip_window(window);
  // TODO: set target frame rate

  // NOTE: load font
  R_Font font;
  {
    ArenaTemp scratch = arena_get_scratch(0, 0);

    String8 font_path = Str8Lit("../data/font/LiberationMono-Regular.ttf"); // TODO: don't hardcode path
    LooseFont font_loose = font_parse(scratch.arena, font_path, 16);
    PackedFont *font_packed = font_pack(permanent_arena, &font_loose);
    font = render_alloc_font(font_packed);

    arena_release_scratch(scratch);
  }

  // NOTE: audio setup
  U32 audio_sample_rate = 48000; // TODO: pass this to the audio backend
  U32 audio_buffer_sample_count = 512; // TODO: pass this to the audio backend

  U64 shared_rb_size = KB(32);
  Os_RingBuffer shared_samples_l, shared_samples_r;
  os_ring_buffer_init(&shared_samples_l, shared_rb_size);
  os_ring_buffer_init(&shared_samples_r, shared_rb_size);

  ScopeStream scope_l, scope_r;
  scope_stream_init(&scope_l, audio_stream_get_input(0), &shared_samples_l);
  scope_stream_init(&scope_r, audio_stream_get_input(1), &shared_samples_r);

  audio_stream_connect_output(&scope_l.self, 0);
  audio_stream_connect_output(&scope_r.self, 1);

  // NOTE: spectrogram state init
  SpectrogramState spec_state_l, spec_state_r;
  spectrogram_init(&spec_state_l, audio_buffer_sample_count, 128);
  spectrogram_init(&spec_state_r, audio_buffer_sample_count, 128);

  audio_start();

  // NOTE: main loop
  Arena *frame_arena = arena_alloc();
  B32 running = 1;
  U32 buffer_draw_sample_count = audio_buffer_sample_count;
  DrawMode draw_mode = DrawMode_frequency_domain;
  U64 last_frame_end_tsc = 0;
  while(running)
  {
    U64 frame_start_tsc = cpu_counter_fixed();
    U64 frame_wait_tsc = frame_start_tsc - last_frame_end_tsc;
    R64 frame_wait_ms = 1000.0 * (R64)frame_wait_tsc / (R64)cpu_counter_fixed_freq();
    printf("frame wait: %.4f ms\n", frame_wait_ms);

    // NOTE: handle events
    Gfx_EventSpan events = gfx_events();
    for(Gfx_Event *event = events.first; event != events.last; ++event)
    {
      if(!gfx_windows_are_equal(window, event->window))
      { continue; }

      switch(event->kind)
      {
	case Gfx_EventKind_close:
	{ running = 0; goto loop_done; }break;

	case Gfx_EventKind_press:
	{
	  switch(event->key)
	  {
	    case Gfx_Key_f:
	    { draw_mode = DrawMode_frequency_domain; }break;
	    case Gfx_Key_t:
	    { draw_mode = DrawMode_time_domain; }break;
	    case Gfx_Key_s:
	    { draw_mode = DrawMode_spectrogram; }break;

	    default: {}break;
	  }
	}break;

	default: {}break;
      }
    }

    V2S32 window_dim = gfx_window_dim(window);
    V2 window_dimf = v2_from_v2s32(window_dim);

    // NOTE: draw
    render_begin_frame();

    // draw background
    V4 background_color = color_v4_from_rgba(0x08, 0x0C, 0x1C, 0xFF);
    Rect2 window_rect = rect2_min_dim(v2(0, 0), window_dimf);
    render_rect(window_rect, 0, RenderLevel(background), background_color);

    // DEBUG:
#if 0
    {
      V4 text_color = color_v4_from_rgba(0xFF, 0xFF, 0xFF, 0xFF);
      V2 text_pos = v2(0, rect2_center(window_rect).y);
      render_string(&font, Str8Lit("Testing Testing 12 12..."), text_pos, 0, text_color);
    }
#endif

    // draw samples
    V2 sample_region_dim = v2(window_dimf.x, 0.5f*window_dimf.y);
    Rect2 sample_region_l = rect2_min_dim(v2(0, 0), sample_region_dim);
    Rect2 sample_region_r = rect2_min_dim(v2(0, 0.5f*window_dimf.y), sample_region_dim);

    SpanU8 span_samples_l = os_ring_buffer_read_span(&shared_samples_l);
    SpanU8 span_samples_r = os_ring_buffer_read_span(&shared_samples_r);
    R32 *samples_l = (R32*)span_samples_l.start;
    R32 *samples_r = (R32*)span_samples_r.start;
    U64 samples_to_read_l = span_count(span_samples_l, R32);
    U64 samples_to_read_r = span_count(span_samples_r, R32);
    U64 samples_to_read = Min(samples_to_read_l, samples_to_read_r);
    printf("%llu samples available\n", samples_to_read);
    U64 sample_count = (samples_to_read >= buffer_draw_sample_count) ? buffer_draw_sample_count : 0;

    switch(draw_mode)
    {
      case DrawMode_frequency_domain:
      {
	draw_spectrum(&font, sample_region_l, samples_l, sample_count, audio_sample_rate);
	draw_spectrum(&font, sample_region_r, samples_r, sample_count, audio_sample_rate);
      }break;
      case DrawMode_time_domain:
      {
	draw_samples(&font, sample_region_l, samples_l, sample_count);
	draw_samples(&font, sample_region_r, samples_r, sample_count);
      }break;
      case DrawMode_spectrogram:
      {
	update_spectrogram(&spec_state_l, samples_l, sample_count);
	update_spectrogram(&spec_state_r, samples_r, sample_count);

	draw_spectrogram(&font, &spec_state_l, sample_region_l, audio_sample_rate);
	draw_spectrogram(&font, &spec_state_r, sample_region_r, audio_sample_rate);
      }break;
    }

    os_ring_buffer_read_end(&shared_samples_l, R32, sample_count);
    os_ring_buffer_read_end(&shared_samples_r, R32, sample_count);

    render_end_frame();

    arena_clear(frame_arena);

    U64 frame_end_tsc = cpu_counter_fixed();
    U64 frame_elapsed_tsc = frame_end_tsc - frame_start_tsc;
    R64 frame_elapsed_ms = 1000.0 * (R64)frame_elapsed_tsc / (R64)cpu_counter_fixed_freq();
    printf("frame elapsed: %.4f ms\n", frame_elapsed_ms);
    last_frame_end_tsc = frame_end_tsc;
  }

loop_done:
  audio_stop();

  gfx_window_close(window);

  audio_uninit();

  //render_uninit();

  gfx_uninit();

  //os_uninit();

  return 0;
}
