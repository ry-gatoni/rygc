#include "base/base.h"
#include "fourier/fourier.h"
#include "file_formats/wav.h"
#include "file_formats/bmp.h"

#include "base/base.c"
#include "fourier/fourier.c"
#include "file_formats/wav.c"
#include "file_formats/bmp.c"

/** TODO
 * render aliased/unaliased spectra to images
 */

proc void
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

proc void
draw_background(U32 *pixels, S32 width, S32 height, U32 color)
{
  for(S32 y = 0; y < height; ++y)
  {
    for(S32 x = 0; x < width; ++x)
    {
      *pixels++ = color;
    }
  }
}

proc void
draw_spectrum_pixels(U32 *pixels, S32 width, S32 height, ComplexBuffer spectrum)
{
  RangeR32 const db_range = range_r32(-120.f, 0.f);
  // TODO: fix byte order
  /* U32 const signal_color = color_u32_from_rgba(0xFF, 0xC1, 0x25, 0xFF); */
  U32 const signal_color = color_u32_from_rgba(0x25, 0xC1, 0xFF, 0xFF);

  R32 norm_factor = 1.f / ((R32)spectrum.count * (R32)spectrum.count);

  for(U32 bin_idx = 0; bin_idx < spectrum.count / 2; ++bin_idx)
  {
    R32 bin_re = spectrum.re[bin_idx];
    R32 bin_im = spectrum.im[bin_idx];
    R32 bin_power = bin_re*bin_re + bin_im*bin_im;
    R32 bin_db = 10.f*log10f(bin_power * norm_factor);

    U32 draw_height = (U32)((R32)height * range_r32_map_01(bin_db, db_range));
    U32 draw_width = (U32)(((R32)bin_idx / (R32)(spectrum.count / 2)) * (R32)width);
    pixels[draw_height * width + draw_width] = signal_color;
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

  // -----------------------------------------------------------------------------
  // synthesize wavs

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

  // -----------------------------------------------------------------------------
  // draw spectra

  S32 const image_width = 640;
  S32 const image_height = 480;
  U32 const background_color = color_u32_from_rgba(0x08, 0x0C, 0x1C, 0xFF);

  String8 aliased_bmp_name = Str8Lit("../data/test/aliased_square_spectrum.bmp");

  U32 aliased_duration_samples_pow_2 = RoundUpPow2(aliased_duration_samples);
  R32 *aliased_fft_samples = arena_push_array(arena, R32, aliased_duration_samples_pow_2);
  CopyArray(aliased_fft_samples, aliased_samples, R32, aliased_duration_samples);

  FloatBuffer aliased_in = { .count = aliased_duration_samples_pow_2, .mem = aliased_fft_samples };
  ComplexBuffer aliased_spectrum = fft_re(arena, aliased_in);

  LoadedBmp aliased_bitmap = bmp_alloc(arena, image_width, image_height);
  draw_background(aliased_bitmap.pixels, image_width, image_height, background_color);
  draw_spectrum_pixels(aliased_bitmap.pixels, image_width, image_height, aliased_spectrum);
  bmp_write(aliased_bitmap, aliased_bmp_name);

  String8 unaliased_bmp_name = Str8Lit("../data/test/unaliased_square_spectrum.bmp");

  U32 unaliased_duration_samples_pow_2 = RoundUpPow2(aliased_duration_samples);
  R32 *unaliased_fft_samples = arena_push_array(arena, R32, unaliased_duration_samples_pow_2);
  CopyArray(unaliased_fft_samples, unaliased_samples, R32, unaliased_duration_samples);

  FloatBuffer unaliased_in = { .count = unaliased_duration_samples_pow_2, .mem = unaliased_fft_samples };
  ComplexBuffer unaliased_spectrum = fft_re(arena, unaliased_in);

  LoadedBmp unaliased_bitmap = bmp_alloc(arena, image_width, image_height);
  draw_background(unaliased_bitmap.pixels, image_width, image_height, background_color);
  draw_spectrum_pixels(unaliased_bitmap.pixels, image_width, image_height, unaliased_spectrum);
  bmp_write(unaliased_bitmap, unaliased_bmp_name);

  return(0);
}
