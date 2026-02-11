#include "base/base.h"
#include "file_formats/wav.h"

#include "base/base.c"
#include "file_formats/wav.c"

/** TODO
 * render aliased/unaliased spectra to images
 */

static void
write_square_wave_samples(R32 *samples, R32 freq, R32 volume, U32 sample_rate, U32 duration)
{
  R32 sample_period = 1.f / (R32)sample_rate;
  R32 phase_increment = freq * sample_period;

  R32 phasor = 0.f;
  for(U32 sample_idx = 0; sample_idx < duration; ++sample_idx)
  {
    if(phasor < 0.5f)
    { samples[sample_idx] = volume; }
    else
    { samples[sample_idx] = -volume; }

    phasor += phase_increment;
    if(phasor >= 1.f)
    { phasor -= 1.f; }
  }
}

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  Arena *arena = arena_alloc();

  R32 const duration_sec = 2.0;
  R32 const volume = 0.1f;
  R32 const freq = 700.f;

  U32 const aliased_sample_rate = 48000;
  String8 aliased_wav_name = Str8Lit("../data/test/aliased_square.wav");
  WavWriter *aliased_wav = begin_wav(aliased_sample_rate, 1, WavSampleKind_R32);

  U32 aliased_duration_samples = (U32)(aliased_sample_rate * duration_sec);
  R32 *aliased_samples = arena_push_array(arena, R32, aliased_duration_samples);
  write_square_wave_samples(
    aliased_samples,
    freq,
    volume,
    aliased_sample_rate,
    aliased_duration_samples);

  wav_push_chunk(aliased_wav, aliased_duration_samples, aliased_samples);
  end_wav(aliased_wav, aliased_wav_name);

  U32 const unaliased_sample_rate = 44100;
  String8 unaliased_wav_name = Str8Lit("../data/test/unaliased_square.wav");
  WavWriter *unaliased_wav = begin_wav(unaliased_sample_rate, 1, WavSampleKind_R32);

  U32 unaliased_duration_samples = (U32)(unaliased_sample_rate * duration_sec);
  R32 *unaliased_samples = arena_push_array(arena, R32, unaliased_duration_samples);
  write_square_wave_samples(
    unaliased_samples,
    freq,
    volume,
    unaliased_sample_rate,
    unaliased_duration_samples);

  wav_push_chunk(unaliased_wav, unaliased_duration_samples, unaliased_samples);
  end_wav(unaliased_wav, unaliased_wav_name);

  return(0);
}
