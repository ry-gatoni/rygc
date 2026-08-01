#include "base/base.h"
#include "fourier/fourier.h"
#include "file_formats/wav.h"

#include "base/base.c"
#include "fourier/fourier.c"
#include "file_formats/wav.c"

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  if(!os_init()) return 1;

  ArenaTemp scratch = arena_get_scratch(0, 0);

  LoadedWav fingertips = wav_load(scratch.arena, Str8Lit(DATA_DIR "/wav/fingertips.wav"));
  U64 fft_count = RoundUpPow2(2*fingertips.sample_count);

  R32 *fingertips_samples_zeropad = arena_push_array_z(scratch.arena, R32, fft_count);
  CopyArray(fingertips_samples_zeropad, fingertips.samples[0], R32, fingertips.sample_count);
  C64 *fingertips_spectrum = arena_push_array(scratch.arena, C64, fft_count/2);
  fft_re(fingertips_spectrum, fingertips_samples_zeropad, fft_count);

  // NOTE: zero-phase spectrum
  for(U64 i = 1; i < fft_count/2; ++i)
  {
    R32 spec_mag = c64_mag(fingertips_spectrum[i]);
    fingertips_spectrum[i] = c64(spec_mag, 0);
  }

  // NOTE: write to file
  WavWriter *fingertips_zero_phase = wav_begin(scratch.arena, fingertips.sample_rate, 1, WavSampleKind_R32);
  R32 *fingertips_zero_phase_samples = wav_push_chunk(fingertips_zero_phase, fft_count);
  ifft_re(fingertips_zero_phase_samples, fingertips_spectrum, fft_count);
  wav_end(fingertips_zero_phase, Str8Lit(DATA_DIR "/wav/fingertips_zero_phase.wav"));

  arena_release_scratch(scratch);

  return 0;
}
