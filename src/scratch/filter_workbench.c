#include "base/base.h"
#include "gfx/gfx.h"
#include "render/render.h"
#include "audio/audio.h"

#include "base/base.c"
#include "gfx/gfx.c"
#include "render/render.c"
#include "audio/audio.c"

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

  Os_RingBufferSpan write_span = os_ring_buffer_write_span(buffer);
  U64 buffer_samples_to_write = (write_span.end - write_span.start)/sizeof(R32);

  R32 *src = source->sample_cursor;
  R32 *out_dest = caller->sample_cursor;
  R32 *buffer_dest = (R32*)write_span.start;
  if(samples_available == 0)
  {
    Assert(buffer_samples_to_write >= output_samples_to_write);
    ZeroArray(out_dest, R32, output_samples_to_write);
    ZeroArray(buffer_dest, R32, output_samples_to_write);
    caller->sample_cursor = caller->samples_end;
    os_ring_buffer_write_end(buffer, output_samples_to_write*sizeof(R32));
    result = Audio_StreamStatus_zero_output;
  }
  else
  {
    U64 samples_to_write = Min(output_samples_to_write, samples_available);
    Assert(buffer_samples_to_write >= samples_to_write);
    CopyArray(out_dest, src, R32, samples_to_write);
    CopyArray(buffer_dest, src, R32, samples_to_write);
    caller->sample_cursor += samples_to_write;
    source->sample_cursor += samples_to_write;
    os_ring_buffer_write_end(buffer, samples_to_write*sizeof(R32));
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

  // NOTE: audio setup
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

    R32 sample_region_middle_bar_thickness = 4;
    V2 sample_region_middle_bar_dim = v2(sample_region_dim.x, sample_region_middle_bar_thickness);
    V4 sample_region_middle_bar_color = color_v4_from_rgba(0x50, 0x50, 0x50, 0xFF);
    V2 sample_region_l_center = rect2_center(sample_region_l);
    Rect2 sample_region_l_middle_bar = rect2_center_dim(sample_region_l_center,
							sample_region_middle_bar_dim);
    V2 sample_region_r_center = rect2_center(sample_region_r);
    Rect2 sample_region_r_middle_bar = rect2_center_dim(sample_region_r_center,
							sample_region_middle_bar_dim);
    render_rect(sample_region_l_middle_bar, 0, 0.8f, sample_region_middle_bar_color);
    render_rect(sample_region_r_middle_bar, 0, 0.8f, sample_region_middle_bar_color);

    Os_RingBufferSpan span_samples_l = os_ring_buffer_read_span(&shared_samples_l);
    Os_RingBufferSpan span_samples_r = os_ring_buffer_read_span(&shared_samples_r);
    U64 samples_to_read_l = (span_samples_l.end - span_samples_l.start)/sizeof(R32);
    U64 samples_to_read_r = (span_samples_r.end - span_samples_r.start)/sizeof(R32);
    U64 samples_to_read = Min(samples_to_read_l, samples_to_read_r);
    printf("drawing %llu samples\n", samples_to_read);

    V4 sample_color = color_v4_from_rgba(0xFF, 0xC1, 0x25, 0xFF);
    R32 sample_rect_width = sample_region_dim.x / (R32)samples_to_read;
    R32 sample_range = 0.3f;
    V2 last_sample_pos_l = v2(0, sample_region_l_center.y);
    V2 last_sample_pos_r = v2(0, sample_region_r_center.y);

    R32 *samples_l = (R32*)span_samples_l.start;
    R32 *samples_r = (R32*)span_samples_r.start;
    for(U64 sample_idx = 0; sample_idx < samples_to_read; ++sample_idx)
    {
      R32 sample_pos_x = ((R32)sample_idx / (R32)samples_to_read) * sample_region_dim.x;

      R32 sample_l = samples_l[sample_idx];
      R32 sample_r = samples_r[sample_idx];

      R32 sample_height_l = sample_region_l_center.y + sample_l / sample_range * sample_region_dim.y * 0.5f;
      R32 sample_height_r = sample_region_r_center.y + sample_r / sample_range * sample_region_dim.y * 0.5f;

      V2 sample_pos_l = v2(sample_pos_x, sample_height_l);
      Rect2 sample_rect_l = rect2(last_sample_pos_l, sample_pos_l);
      render_rect(sample_rect_l, 0, 0, sample_color);

      V2 sample_pos_r = v2(sample_pos_x + sample_rect_width, sample_height_r);
      Rect2 sample_rect_r = rect2(last_sample_pos_r, sample_pos_r);
      render_rect(sample_rect_r, 0, 0, sample_color);

      last_sample_pos_l = sample_pos_l;
      last_sample_pos_r = sample_pos_r;
    }

    os_ring_buffer_read_end(&shared_samples_l, samples_to_read*sizeof(R32));
    os_ring_buffer_read_end(&shared_samples_r, samples_to_read*sizeof(R32));

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
