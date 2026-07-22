#include "base/base.h"
#include "gfx/gfx.h"
#include "render/render.h"
#include "audio/audio.h"
#include "fourier/fourier.h"

#include "base/base.c"
#include "gfx/gfx.c"
#include "render/render.c"
#include "audio/audio.c"
#include "fourier/fourier.c"

// TODO:
// - logging in window
// - toggle draw mode with key press
// - dynamic audio sources
// - plots:
//   - axis labels
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
} DrawMode;

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

  // NOTE: open window
  Gfx_Handle window = gfx_window_open(640, 480, Str8Lit("filter workbench"));
  render_set_backend(R_Backend_opengl);
  render_equip_window(window);
  // TODO: set target frame rate

  // NOTE: audio setup
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
	    {
	      draw_mode = DrawMode_frequency_domain;
	    }break;

	    case Gfx_Key_t:
	    {
	      draw_mode = DrawMode_time_domain;
	    }break;

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
    render_rect(window_rect, 0, 0.9f, background_color);

    // draw samples
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
