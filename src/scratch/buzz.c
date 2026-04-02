#include "base/base.h"
#include "file_formats/wav.h"

#include "base/base.c"
#include "file_formats/wav.c"

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  ArenaTemp scratch = arena_get_scratch(0, 0);

  // parameters & constants
  R32 const eps = 1e-3;

  R32 const freq_base = 440.f;
  R32 const freq_multiplier = 1.f;
  R32 const amp = 0.3f;

  R32 const duration_sec = 3.f;

  R32 const reson_r = 0.8f;
  R32 const reson_freq_base = freq_base;
  R32 const reson_freq_multiplier = 16.f;

  U32 const sr = 48000;

  // derived constants
  R32 const freq0 = freq_base;
  R32 const freq1 = freq_base * freq_multiplier;

  R32 const reson_freq0 = reson_freq_base;
  R32 const reson_freq1 = reson_freq_base * reson_freq_multiplier;
  R32 const reson_rsq = reson_r * reson_r;

  U32 const nyquist = sr / 2;
  U32 const harmonic_count = (U32)((R32)nyquist / Max(freq0, freq1));
  U32 const duration_samples = (U32)(duration_sec * (R32)sr);
  R32 const timestep = 1.f / (R32)sr;

  // memory
  WavWriter *wav = begin_wav(sr, 1, WavSampleKind_R32);
  R32 *samples = arena_push_array(scratch.arena, R32, duration_samples);

  // audio loop
  R32 in_buffer[2] = {0};
  R32 out_buffer[2] = {0};
  R32 phasor = 0.f;
  for(U32 t = 0; t < duration_samples; ++t)
  {
    R32 const t_n = (R32)t/(R32)duration_samples;
    R32 const freq = lerp(freq0, freq1, t_n);
    R32 const w = 0.5f * TAU32 * freq * timestep;
    R32 sample_in_0 = (fabsf(phasor) < eps) ?
      amp :
      amp * sinf(phasor*(2*harmonic_count + 1)) / ((2*harmonic_count + 1) * sinf(phasor));

    R32 const reson_freq = lerp(reson_freq0, reson_freq1, t_n);
    R32 const reson_a = (reson_freq * timestep) * TAU32;
    R32 const reson_2rcosa = 2.f*reson_r*cosf(reson_a);

    R32 sample_in_2 = in_buffer[1];
    R32 sample_out_1 = out_buffer[0];
    R32 sample_out_2 = out_buffer[1];

    R32 sample_out_0 =
      sample_in_0 - sample_in_2
      + reson_2rcosa*sample_out_1 - reson_rsq*sample_out_2;

    samples[t] = sample_out_0;

    in_buffer[1] = in_buffer[0];
    in_buffer[0] = sample_in_0;

    out_buffer[1] = out_buffer[0];
    out_buffer[0] = sample_out_0;

    phasor += w;
    if(phasor >= TAU32) phasor -= TAU32;
  }
  wav_push_chunk(wav, duration_samples, samples);

  end_wav(wav, Str8Lit(DATA_DIR"/test/buzz.wav"));

  arena_release_scratch(scratch);

  return(0);
}
