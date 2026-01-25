// TODO: debug

#include "base/base.h"
#include "gfx/gfx.h"
#include "render/render.h"
#include "font/font.h"
#include "OpenGL/ogl.h"
#include "audio/audio.h"

#include "base/base.c"
#include "gfx/gfx.c"
#include "render/render.c"
#include "font/font.c"
#include "OpenGL/ogl.c"
#include "audio/audio.c"

typedef struct LissajousStream LissajousStream;
struct LissajousStream
{
  U8 *start;
  U8 *at;
  U8 *end;
  void (*refill)(LissajousStream *stream);
};

typedef struct LissajousState
{
  LissajousStream stream;

  R32 freq_ratio;
  R32 freq1;
  R32 freq2;
  R32 phase1;
  R32 phase2;

  V2 *samples;
  U64 sample_count;
  AlignField(64) volatile U64 read_idx;
  AlignField(64) volatile U64 write_idx;
} LissajousState;

proc void
lissajous_refill(LissajousStream *stream)
{
  LissajousState *state = (LissajousState*)stream;
  U64 samples_available = state->write_idx - state->read_idx;

  // NOTE: drop unread samples
  stream->start = (U8*)(state->samples + (state->read_idx & (state->sample_count - 1)));
  stream->end = stream->start + samples_available*sizeof(*state->samples);
  stream->at = stream->start;

  state->read_idx += samples_available;
}

global const U8 freq_ratio_channel = 0x4a;

void
audio_process(Audio_ProcessData *data)
{
  U32 sample_count = data->sample_count;
  R32 sample_period = data->sample_period;

  LissajousState *state = data->user_data;
  if(state)
  {
    R32 freq_ratio = state->freq_ratio;
    R32 freq1 = state->freq1;
    R32 freq2 = state->freq2;
    R32 phase1 = state->phase1;
    R32 phase2 = state->phase2;

    R32 volume = 0.8f;
    V2 *samples = state->samples + (state->write_idx & (state->sample_count - 1));
    for(U32 sample_idx = 0; sample_idx < sample_count; ++sample_idx)
    {
      R32 sin1 = volume * sinf(phase1);
      R32 sin2 = volume * sinf(phase2);

      samples[sample_idx] = v2(sin1, sin2);
      data->output[0][sample_idx] = 0.5f * (sin1 + sin2);
      data->output[1][sample_idx] = 0.5f * (sin1 + sin2);

      Audio_MidiMessage *midi_msg = audio_next_midi_message(data);
      if(midi_msg)
      {
        if(midi_msg->sample_idx >= sample_idx)
        {
          if(midi_msg->opcode == MidiOpcode_continuous_controller)
          {
            U8 channel = *buf_consume_struct(&midi_msg->body, U8);
            U8 value = *buf_consume_struct(&midi_msg->body, U8);
            if(channel == freq_ratio_channel) freq_ratio = (R32)(value + 1) / 32.f;
          }
          audio_dequeue_midi_message(data);
        }
      }

      freq2 = freq1 * freq_ratio;
      phase1 += TAU32 * freq1 * sample_period;
      phase2 += TAU32 * freq2 * sample_period;
      if(phase1 >= TAU32) { phase1 -= TAU32; }
      if(phase2 >= TAU32) { phase2 -= TAU32; }
    }

    state->freq_ratio = freq_ratio;
    state->freq1 = freq1;
    state->freq2 = freq2;
    state->phase1 = phase1;
    state->phase2 = phase2;
    state->write_idx += sample_count;
  }
}

typedef enum RenderLevel
{
  RenderLevel_front,
  RenderLevel_signal,
  RenderLevel_background,
  RenderLevel_Count,
} RenderLevel;
#define RenderLevel(level) ((R32)(RenderLevel_##level)/(R32)(RenderLevel_Count))

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  Arena *permanent_arena = arena_alloc();
  Arena *frame_arena = arena_alloc();

  if(!os_init()) { goto failure; }
  if(!os_gfx_init()) { goto failure; }
  if(!render_init()) { goto failure; }
  if(!audio_init(Str8Lit("lissajous"))) { goto failure; }

  U64 min_sample_count = RoundUpPow2(48000ULL);
  Os_RingBuffer rb = os_ring_buffer_alloc(min_sample_count * sizeof(V2));
  LissajousState *state = arena_push_struct(permanent_arena, LissajousState);
  state->freq_ratio = 1;
  state->freq1 = 440.f;
  state->freq2 = 440.f;
  state->phase2 = 0.5f*PI32;
  state->samples = (V2*)rb.mem;
  state->sample_count = rb.size / sizeof(V2);
  LissajousStream *stream = &state->stream;
  stream->at = stream->start = stream->end = (U8*)state->samples;
  stream->refill = lissajous_refill;

  Os_Handle window = os_open_window(Str8Lit("Lissajous"), 640, 480);
  render_equip_window(window);

  V4 background_color = color_v4_from_rgba(0x08, 0x0c, 0x1c, 0xff);
  V4 signal_color = color_v4_from_rgba(0xda, 0xa5, 0x20, 0xff);

  audio_set_user_data(state);

  B32 running = 1;
  while(running)
  {
    local U32 frame_idx = 0;
    render_begin_frame();

    Os_EventList events = os_events(frame_arena);
    for(Os_Event *event = events.first; event; event = event->next)
    {
      if(event->kind == Os_EventKind_close)
      {
        running = 0;
      }
    }

    V2S32 window_dim = os_window_get_dim(window);
    V2 window_dim_r32 = v2_from_v2s32(window_dim);
    Rect2 window_rect = rect2(v2(0,0), window_dim_r32);
    fprintf(stdout, "frame %u: window dim: (%d,%d)\n", frame_idx, window_dim.x, window_dim.y);
    render_rect(window_rect, 0, RenderLevel(background), background_color);

    stream->refill(stream);
    V2 *samples_start = (V2*)stream->at;
    V2 *samples_end = (V2*)stream->end;
    if(samples_end - samples_start > 1024)
    {
      samples_end = samples_start + 1024;
    }
    --samples_end;
    for(V2 *samples = samples_start; samples < samples_end; ++samples)
    {
      V2 sample0 = samples[0];
      V2 sample1 = samples[1];
      V2 sample0_pt = v2_hadamard(v2_lmul(0.5f,
                                          v2_add(sample0,
                                                 v2(1, 1))),
                                  window_dim_r32);
      V2 sample1_pt = v2_hadamard(v2_lmul(0.5f,
                                          v2_add(sample1,
                                                 v2(1, 1))),
                                  window_dim_r32);
      render_line_segment(sample0_pt, sample1_pt, 4, RenderLevel(signal), signal_color);
    }

    render_end_frame();
    ++frame_idx;
    arena_clear(frame_arena);
  }

  audio_uninit();

  os_close_window(window);

  os_ring_buffer_free(&rb);

  arena_release(permanent_arena);
  arena_release(frame_arena);

  return(0);
failure:
  return(1);
}
