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
//   - spectrogram plot

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
#if 1
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

    // TODO: update time domain
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
      { /* TODO: */ }break;
    }

    os_ring_buffer_read_end(&shared_samples_l, R32, sample_count);
    os_ring_buffer_read_end(&shared_samples_r, R32, sample_count);

#else
    V2 sample_region_dim = v2(window_dimf.x, 0.5f*window_dimf.y);
    Rect2 sample_region_l = rect2_min_dim(v2(0, 0), sample_region_dim);
    Rect2 sample_region_r = rect2_min_dim(v2(0, 0.5f*window_dimf.y), sample_region_dim);
    V2 sample_region_l_center = rect2_center(sample_region_l);
    V2 sample_region_r_center = rect2_center(sample_region_r);

    if(draw_mode == DrawMode_time_domain)
    {
      R32 sample_region_middle_bar_thickness = 4;
      V2 sample_region_middle_bar_dim = v2(sample_region_dim.x, sample_region_middle_bar_thickness);
      V4 sample_region_middle_bar_color = color_v4_from_rgba(0x50, 0x50, 0x50, 0xFF);
      Rect2 sample_region_l_middle_bar = rect2_center_dim(sample_region_l_center,
							  sample_region_middle_bar_dim);
      Rect2 sample_region_r_middle_bar = rect2_center_dim(sample_region_r_center,
							  sample_region_middle_bar_dim);
      render_rect(sample_region_l_middle_bar, 0, 0.8f, sample_region_middle_bar_color);
      render_rect(sample_region_r_middle_bar, 0, 0.8f, sample_region_middle_bar_color);
    }
    else if(draw_mode == DrawMode_frequency_domain)
    {
      V4 spectrum_baseline_color = color_v4_from_rgba(0x50, 0x50, 0x50, 0xFF);
      R32 spectrum_baseline_thickness = 4.f;
      V2 spectrum_baseline_dim = v2(sample_region_dim.x, spectrum_baseline_thickness);
      Rect2 spectrum_baseline_l = rect2_min_dim(sample_region_l.min, spectrum_baseline_dim);
      Rect2 spectrum_baseline_r = rect2_min_dim(sample_region_r.min, spectrum_baseline_dim);
      render_rect(spectrum_baseline_l, 0, 0.8f, spectrum_baseline_color);
      render_rect(spectrum_baseline_r, 0, 0.8f, spectrum_baseline_color);
    }
    else
    { Assert(0); }

    SpanU8 span_samples_l = os_ring_buffer_read_span(&shared_samples_l);
    SpanU8 span_samples_r = os_ring_buffer_read_span(&shared_samples_r);
    U64 samples_to_read_l = span_count(span_samples_l, R32);
    U64 samples_to_read_r = span_count(span_samples_r, R32);
    U64 samples_to_read = Min(samples_to_read_l, samples_to_read_r);
    printf("%llu samples available\n", samples_to_read);
    if(samples_to_read >= buffer_draw_sample_count)
    {
      R32 *samples_l = (R32*)span_samples_l.start;
      R32 *samples_r = (R32*)span_samples_r.start;
      if(draw_mode == DrawMode_time_domain)
      {
	V4 sample_color = color_v4_from_rgba(0xFF, 0xC1, 0x25, 0xFF);
	R32 sample_rect_width = sample_region_dim.x / (R32)samples_to_read;
	R32 sample_range = 0.3f;
	R32 sample_line_thickness = 2.f;
	V2 last_sample_pos_l = v2(0, sample_region_l_center.y);
	V2 last_sample_pos_r = v2(0, sample_region_r_center.y);

	for(U64 sample_idx = 0; sample_idx < samples_to_read; ++sample_idx)
	{
	  R32 sample_pos_x = ((R32)sample_idx / (R32)samples_to_read) * sample_region_dim.x;

	  R32 sample_l = samples_l[sample_idx];
	  R32 sample_r = samples_r[sample_idx];

	  R32 sample_height_l = sample_region_l_center.y + sample_l / sample_range * sample_region_dim.y * 0.5f;
	  R32 sample_height_r = sample_region_r_center.y + sample_r / sample_range * sample_region_dim.y * 0.5f;

	  V2 sample_pos_l = v2(sample_pos_x, sample_height_l);
	  render_line_segment(last_sample_pos_l, sample_pos_l, sample_line_thickness, 0, sample_color);

	  V2 sample_pos_r = v2(sample_pos_x + sample_rect_width, sample_height_r);
	  render_line_segment(last_sample_pos_r, sample_pos_r, sample_line_thickness, 0, sample_color);

	  last_sample_pos_l = sample_pos_l;
	  last_sample_pos_r = sample_pos_r;
	}
      }
      else if(draw_mode == DrawMode_frequency_domain)
      {
	V4 freq_color = color_v4_from_rgba(0xFF, 0xC1, 0x25, 0xFF);
	R32 freq_rect_width = sample_region_dim.x / (R32)samples_to_read;
	R32 freq_mag_sq_range = 0.1f*(R32)buffer_draw_sample_count; // TODO: make dynamic

	// DEBUG:
	static R32 max_mag_sq = 0.f;

	C64 *freqs = arena_push_array_z(frame_arena, C64, 2*buffer_draw_sample_count);
	C64 *freqs_l = freqs + 0*buffer_draw_sample_count;
	C64 *freqs_r = freqs + 1*buffer_draw_sample_count;
	fft_re(freqs_l, samples_l, buffer_draw_sample_count);
	fft_re(freqs_r, samples_r, buffer_draw_sample_count);
	for(U64 sample_idx = 0; sample_idx < buffer_draw_sample_count; ++sample_idx)
	{
	  // TODO: lin/log freq scale
	  R32 freq_pos_x = 2.f*((R32)sample_idx / (R32)buffer_draw_sample_count) * sample_region_dim.x;

	  C64 freq_l = freqs_l[sample_idx];
	  C64 freq_r = freqs_r[sample_idx];

	  R32 freq_mag_sq_l = c64_mag_sq(freq_l);
	  R32 freq_mag_sq_r = c64_mag_sq(freq_r);

	  max_mag_sq = Max(max_mag_sq, freq_mag_sq_l);
	  max_mag_sq = Max(max_mag_sq, freq_mag_sq_r);

	  // TODO: lin/db, power or magnitude amplitude scale
	  R32 freq_height_l = freq_mag_sq_l / freq_mag_sq_range * sample_region_dim.y;
	  R32 freq_height_r = freq_mag_sq_r / freq_mag_sq_range * sample_region_dim.y;

	  Rect2 freq_rect_l = rect2_min_dim(v2(freq_pos_x, sample_region_l.min.y),
					    v2(freq_rect_width, freq_height_l));
	  render_rect(freq_rect_l, 0, 0, freq_color);

	  Rect2 freq_rect_r = rect2_min_dim(v2(freq_pos_x, sample_region_r.min.y),
					    v2(freq_rect_width, freq_height_r));
	  render_rect(freq_rect_r, 0, 0, freq_color);
	}

	printf("max mag sq: %.4f\n", max_mag_sq);
      }
      else
      { Assert(0); }

      os_ring_buffer_read_end(&shared_samples_l, R32, buffer_draw_sample_count);
      os_ring_buffer_read_end(&shared_samples_r, R32, buffer_draw_sample_count);
    }
    else
    { printf("not drawing samples\n"); }
#endif

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
