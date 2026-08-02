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
  U64 sample_rate = fingertips.sample_rate;
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
  {
    WavWriter *fingertips_zero_phase = wav_begin(scratch.arena, fingertips.sample_rate, 1, WavSampleKind_R32);

    // NOTE: synthesize zero-phase spectrum
    R32 *fingertips_zero_phase_samples = wav_push_chunk(fingertips_zero_phase, fft_count);
    ifft_re(fingertips_zero_phase_samples, fingertips_spectrum, fft_count);

    // NOTE: gain compensation, fade-in/out
    {
      R32 const eps = 0.05f;
      R32 fade_time_sec = 0.1f;
      U64 fade_time_samples = (U64)(fade_time_sec * (R32)sample_rate);

      for(U64 i = 0; i < fft_count; ++i)
      {
	R32 ramp = (i < fft_count/2) ?
	  lerp(1, 0, 2.f*(R32)i/(R32)fft_count) :
	  lerp(0, 1, 2.f*(R32)(i - fft_count/2)/(R32)fft_count);

	R32 gain_factor = 1.f / rygc_sqrt(ramp + eps);
	if(i <= fade_time_samples)
	{
	  R32 sine = rygc_sin(TAU32 * (R32)i/(R32)(4*fade_time_samples));
	  gain_factor *= sine;
	}
	else if(i >= fft_count - fade_time_samples)
	{
	  R32 cosine = rygc_cos(TAU32 * (R32)(i - (fft_count - fade_time_samples))/(R32)(4*fade_time_samples));
	  gain_factor *= cosine;
	}

	fingertips_zero_phase_samples[i] *= gain_factor;
      }
    }

    wav_end(fingertips_zero_phase, Str8Lit(DATA_DIR "/wav/fingertips_zero_phase.wav"));
  }

  arena_release_scratch(scratch);

  return 0;
}
