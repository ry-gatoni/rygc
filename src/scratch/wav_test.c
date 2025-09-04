#include "base/base.h"
#include "os/os.h"
#include "file_formats/wav.h"

#include "base/base.c"
#include "os/os.c"
#include "file_formats/wav.c"

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  ArenaTemp scratch = arena_get_scratch(0, 0);

  U32 sample_rate = 48000;
  U32 channel_count = 2;
  WavSampleKind sample_kind = WavSampleKind_R32;

  WavWriter *wav_writer = begin_wav(sample_rate, channel_count, sample_kind);

  R32 freq = 440.f;
  R32 vol = 0.1f;
  R32 sample_period = 1.f/(R32)sample_rate;
  U32 sample_count = 441;
  R32 *samples = arena_push_array(scratch.arena, R32, channel_count * sample_count);

  U32 push_count = 200;
  for(U32 push_idx = 0; push_idx < push_count; ++push_idx) {
    U32 sample_idx_offset = push_idx * sample_count;
    for(U32 sample_idx = 0; sample_idx < sample_count; ++sample_idx) {
      R32 sine_val_l = vol*sinf((R32)(sample_idx_offset + sample_idx + 0)*freq*TAU32*sample_period);
      R32 sine_val_r = vol*sinf((R32)(sample_idx_offset + sample_idx + 0)*freq*TAU32*sample_period);
      samples[channel_count * sample_idx + 0] = sine_val_l;
      samples[channel_count * sample_idx + 1] = sine_val_r;
    }

    wav_push_chunk(wav_writer, sample_count, samples);
  }

  end_wav(wav_writer, Str8Lit(DATA_DIR"/test/test.wav"));

  arena_release_scratch(scratch);
  return(0);
}
