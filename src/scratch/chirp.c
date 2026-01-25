#include "base/base.h"
#include "file_formats/wav.h"

#include "base/base.c"
#include "file_formats/wav.c"

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  Arena *arena = arena_alloc();

  R32 volume = 0.8f;

  U32 sample_rate = 48000;
  R32 sample_period = 1.f / (R32)sample_rate;
  WavWriter *wav = begin_wav(sample_rate, 1, WavSampleKind_R32);

  R32 w1 = 440.f;
  R32 w2 = 4.f * w1;

  R32 w1_duration__s = 1.f;
  R32 ramp_duration__s = 2.f;
  R32 w2_duration__s = 1.f;

  String8 filename;
#define CHIRP_BAD 1
#if CHIRP_BAD
  filename = Str8Lit("../data/test/chirp_bad.wav");

  U32 w1_duration__samples = (U32)(w1_duration__s * (R32)sample_rate);
  U32 ramp_duration__samples = (U32)(ramp_duration__s * (R32)sample_rate);
  U32 w2_duration__samples = (U32)(w2_duration__s * (R32)sample_rate);
  U32 duration = w1_duration__samples + ramp_duration__samples + w2_duration__samples;
  R32 *samples = arena_push_array(arena, R32, duration);
  for(U32 sample_idx = 0; sample_idx < duration; ++sample_idx)
  {
    R32 w = 0.f;
    if(sample_idx < w1_duration__samples)
    {
      w = w1;
    }
    else if(sample_idx < w1_duration__samples + ramp_duration__samples)
    {
      w = w1 + ((R32)(sample_idx - w1_duration__samples)/(R32)ramp_duration__samples)*(w2 - w1);
    }
    else
    {
      w = w2;
    }

    samples[sample_idx] = volume * sinf(TAU32 * w * (R32)sample_idx * sample_period);
  }
  wav_push_chunk(wav, duration, samples);
#else
  filename = Str8Lit("../data/test/chirp_good.wav");

  R32 phasor = 0;
  {
    U32 w1_duration__samples = (U32)(w1_duration__s * (R32)sample_rate);
    R32 *w1_samples = arena_push_array(arena, R32, w1_duration__samples);
    for(U32 sample_idx = 0; sample_idx < w1_duration__samples; ++sample_idx)
    {
      w1_samples[sample_idx] = volume * sinf(phasor);
      phasor += TAU32 * w1 * sample_period;
      if(phasor > TAU32) { phasor -= TAU32; }
    }
    wav_push_chunk(wav, w1_duration__samples, w1_samples);
  }

  {
    U32 ramp_duration__samples = (U32)(ramp_duration__s * (R32)sample_rate);
    R32 *ramp_samples = arena_push_array(arena, R32, ramp_duration__samples);
    for(U32 sample_idx = 0; sample_idx < ramp_duration__samples; ++sample_idx)
    {
      R32 freq = w1 + ((R32)sample_idx/(R32)ramp_duration__samples)*(w2 - w1);
      phasor += TAU32 * freq * sample_period;
      if(phasor > TAU32) { phasor -= TAU32; }
      ramp_samples[sample_idx] = volume * sinf(phasor);
    }
    wav_push_chunk(wav, ramp_duration__samples, ramp_samples);
  }

  {
    U32 w2_duration__samples = (U32)(w2_duration__s * (R32)sample_rate);
    R32 *w2_samples = arena_push_array(arena, R32, w2_duration__samples);
    for(U32 sample_idx = 0; sample_idx < w2_duration__samples; ++sample_idx)
    {
      phasor += TAU32 * w2 * sample_period;
      if(phasor > TAU32) { phasor -= TAU32; }
      w2_samples[sample_idx] = volume * sinf(phasor);
    }
    wav_push_chunk(wav, w2_duration__samples, w2_samples);
  }
#endif

  end_wav(wav, filename);

  return(0);
}
