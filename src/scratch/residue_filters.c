#include "base/base.h"
#include "file_formats/wav.h"
//#include "audio/audio.h"

#include "base/base.c"
#include "file_formats/wav.c"
//#include "audio/audio.c"

global R32 residues_mod_2[] = {
  0, 1,
};

global R32 residues_mod_3[] = {
  0, 1, 1,
};

global R32 residues_mod_4[] = {
  0, 1,
};

global R32 residues_mod_5[] = {
  0, 1, 4, 4, 1,
};

global R32 residues_mod_6[] = {
  0, 1, 4, 3, 4, 1,
};

global R32 residues_mod_7[] = {
  0, 1, 4, 2, 2, 4, 1,
};

global R32 residues_mod_8[] = {
  0, 1, 4, 1,
};

global R32 residues_mod_9[] = {
  0, 1, 4, 0, 7, 7, 0, 4, 1,
};

global R32 residues_mod_10[] = {
  0, 1, 4, 9, 6, 5, 6, 9, 4, 1,
};

global R32 residues_mod_11[] = {
  0, 1, 4, 9, 5, 3, 3, 5, 9, 4, 1,
};

global R32 residues_mod_12[] = {
  0, 1, 4, 9, 4, 1,
};

global R32 residues_mod_13[] = {
  0, 1, 4, 9, 3, 12, 10, 10, 12, 3, 9, 4, 1,
};

global R32 *residues[] = {
  residues_mod_2,
  residues_mod_3,
  residues_mod_4,
  residues_mod_5,
  residues_mod_6,
  residues_mod_7,
  residues_mod_8,
  residues_mod_9,
  residues_mod_10,
  residues_mod_11,
  residues_mod_12,
  residues_mod_13,
};

typedef struct FilterState
{
  R32 gain;
  R32 *coeffs;
  U64 coeff_count;
  Os_RingBuffer samples;
} FilterState;

proc void
filter_init(FilterState *fs, R32 *coeffs, U64 coeff_count)
{
  os_ring_buffer_init(&fs->samples, KB(16));
  fs->coeffs = coeffs;
  fs->coeff_count = coeff_count;
  os_ring_buffer_write_end(&fs->samples, (coeff_count - 1)*sizeof(*coeffs)); // NOTE: "write" zeros
  R32 coeff_sum = 0.f;
  for(U64 coeff_idx = 0; coeff_idx < coeff_count; ++coeff_idx)
  {
    coeff_sum += coeffs[coeff_idx];
  }
  fs->gain = 1.f / coeff_sum;
}

proc void
filter_process(FilterState *fs, R32 *out_samples, R32 const *in_samples, U64 sample_count)
{
  Os_RingBuffer *rb_samples = &fs->samples;

  // NOTE: fill ring buffer
  {
    U64 rb_bytes_available = os_ring_buffer_free(rb_samples);
    U64 rb_samples_available = rb_bytes_available / sizeof(*in_samples);
    Assert(sample_count <= rb_samples_available);
    Os_RingBufferSpan rb_write_span = os_ring_buffer_write_span(rb_samples);
    CopyArray(rb_write_span.start, in_samples, R32, sample_count);
    os_ring_buffer_write_end(rb_samples, sample_count * sizeof(*in_samples));
  }

  // NOTE: filter samples, write to output
  {
    // NOTE: if not enough samples available to process, zero output and return
    U64 rb_bytes_available = os_ring_buffer_used(rb_samples);
    U64 rb_samples_available = rb_bytes_available / sizeof(*out_samples);
    if(rb_samples_available < sample_count + fs->coeff_count - 1)
    { ZeroArray(out_samples, R32, sample_count); return; }

    Os_RingBufferSpan rb_read_span = os_ring_buffer_read_span(rb_samples);
    R32 *prev_samples = (R32*)rb_read_span.start;
    for(U64 sample_idx = 0; sample_idx < sample_count; ++sample_idx)
    {
      R32 out_sample = 0;
      for(U64 coeff_idx = 0; coeff_idx < fs->coeff_count; ++coeff_idx)
      {
	R32 coeff = fs->coeffs[coeff_idx];
	R32 sample = prev_samples[sample_idx + coeff_idx];
	out_sample += coeff * sample;
      }
      out_sample *= fs->gain;
      printf("sample %4lu: %.4f\n",
	     sample_idx, out_sample);

      out_samples[sample_idx] = out_sample;
    }
    os_ring_buffer_read_end(rb_samples, sample_count*sizeof(*out_samples));
  }
}

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  Unused(residues);

  os_init();
  Arena *arena = arena_alloc();

  // NOTE: init filter
  FilterState fs;
  filter_init(&fs, residues_mod_7, ArrayCount(residues_mod_7));

  // NOTE: synthesize saw wave
  U64 sample_rate = 44100;
  R32 freq_hz = 440.f;
  R32 vol = 0.5f;
  U64 duration_s = 2;

  U64 duration_samples = duration_s * sample_rate;
  R32 sample_period = 1.f / (R32)sample_rate;
  R32 freq_rad = TAU32 * freq_hz * sample_period;

  R32 *saw_samples = arena_push_array(arena, R32, duration_samples);
  {
    R32 phasor = 0.f;
    for(U64 sample_idx = 0; sample_idx < duration_samples; ++sample_idx)
    {
      phasor += freq_rad;
      if(phasor >= TAU32)
      { phasor -= TAU32; }

      saw_samples[sample_idx] = vol * phasor / TAU32;
    }
  }

  // NOTE: write filter output to wav
  WavWriter *wav = wav_begin(arena, sample_rate, 1, WavSampleKind_R32);
  R32 *wav_samples = wav_push_chunk(wav, duration_samples);

  // NOTE: process blocks
  {
    U64 block_size = 1024;
    U64 samples_remaining = duration_samples;
    R32 *out_samples = wav_samples;
    while(samples_remaining)
    {
      printf("new block\n");
      U64 sample_count = Min(samples_remaining, block_size);
      filter_process(&fs, out_samples, saw_samples, sample_count);

      saw_samples += sample_count;
      out_samples += sample_count;
      samples_remaining -= sample_count;
    }
  }

  wav_end(wav, Str8Lit(DATA_DIR "/test/residue_filter.wav"));

  return(0);
}
