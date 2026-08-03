// NOTE: implemented on each instruction set/extension (eventually), internal
global U64 g_initial_level = 1;
proc U64 fft_initial(C64 *out, C64 *in, U64 count);
proc void fft_kernel(C64 *io, U64 count, U64 level);

proc void fft_re_convert(C64 *io, U64 real_count);

proc U64 ifft_initial(C64 *out, C64 *in, U64 count);
proc void ifft_kernel(C64 *io, U64 count, U64 level);

proc void ifft_re_convert(C64 *io, U64 real_count);

// NOTE: backend agnostic, internal
proc void fft_leaf(C64 *io, U64 count);
proc void fft_rec(C64 *io, U64 count);

proc void ifft_leaf(C64 *io, U64 count);
proc void ifft_rec(C64 *io, U64 count);

#define FFT_MAX_COUNT 4096
global R32 fft_sine_table[4*FFT_MAX_COUNT];
// NOTE:
// fft_sine_table[2N:4N] stores the N twiddle factors for each of the forward and inverse DFT
// fft_sine_table[2N + k] = sin(2pi*k/2N)
// fft_sine_table[2N + k + N/2] = sin(2pi*(k + N/2)/2N)
//                              = sin(2pi*k/2N + pi/2)
//                              = cos(2pi*k/2N)
// fft_sine_table[2N + k + N] = sin(2pi*(k + N)/2N)
//                            = sin(2pi*k/2N + pi)
//                            = -sin(2pi*k/2N)
BeforeMain__Named(fft_sine_table_init)
{
  for(U32 n = 2; n <= FFT_MAX_COUNT; n *= 2)
  {
    R32 step = TAU32 / (R32)(2*n);
    R32 *twiddles = fft_sine_table + 2*n;
    for(U32 k = 0; k < n; ++k)
    {
      R32 phase = step * (R32)k;
      twiddles[k] = rygc_sin(phase);
    }

    twiddles += n/2;
    for(U32 k = 0; k < n; ++k)
    {
      R32 phase = step * (R32)k;
      twiddles[k] = rygc_cos(phase);
    }

    twiddles += n/2;
    for(U32 k = 0; k < n; ++k)
    {
      R32 phase = step * (R32)k;
      twiddles[k] = -rygc_sin(phase);
    }
  }
}

proc void
fft_bit_reverse_copy_forward(C64 *dest, C64 *src, U64 count) {ProfileFunction()
{
  U64 count_log2 = MSB(count);
  for(U64 i = 0; i < count; ++i)
  {
    U64 i_rev = bit_reverse_u64(i) >> (64 - count_log2);
    dest[i_rev] = src[i];
  }
}}

proc void
fft_bit_reverse_copy_inverse(C64 *dest, C64 *src, U64 count) {ProfileFunction()
{
  U64 count_log2 = MSB(count);
  R32 scale = 1.f/(R32)count;
  for(U64 i = 0; i < count; ++i)
  {
    U64 i_rev = bit_reverse_u64(i) >> (64 - count_log2);
    dest[i_rev] = c64(scale * src[i].re, scale * src[i].im);
  }
}}

proc U64
fft_initial(C64 *dest, C64 *src, U64 count)
{
  ProfileFunction()
  {
    fft_bit_reverse_copy_forward(dest, src, count);

    // NOTE: radix-2 step for level 1
    C64 *io0 = dest + 0;
    C64 *io1 = dest + 1;
    for(U64 k = 0; k < count; k += 2)
    {
      C64 in0 = io0[k];
      C64 in1 = io1[k];

      io0[k] = c64_add(in0, in1);
      io1[k] = c64_sub(in0, in1);
    }
  }

  return 2;
}

proc U64
ifft_initial(C64 *dest, C64 *src, U64 count)
{
  ProfileFunction()
  {
    fft_bit_reverse_copy_inverse(dest, src, count);

    // NOTE: radix-2 step for level 1
    C64 *io0 = dest + 0;
    C64 *io1 = dest + 1;
    for(U64 k = 0; k < count; k += 2)
    {
      C64 in0 = io0[k];
      C64 in1 = io1[k];

      io0[k] = c64_add(in0, in1);
      io1[k] = c64_sub(in0, in1);
    }
  }

  return 2;
}

proc void
fft_re_convert(C64 *io, U64 real_count) {ProfileFunction()
{
  U64 m = real_count / 2;

  R32 *twiddles_im = fft_sine_table + 2*m;
  R32 *twiddles_re = twiddles_im + m/2;
  twiddles_im += m;

  for(U64 k = 0; k < m/2; ++k)
  {
    if(k == 0)
    {
      R32 r = io[0].re;
      R32 i = io[0].im;
      io[0].re = r + i; // DC
      io[0].im = r - i; // Nyquist
      continue;
    }

    // TODO: find a better solution for when real_count is large
    C64 w;
    if(4*m < ArrayCount(fft_sine_table))
    {
      w = c64(twiddles_re[k], twiddles_im[k]);
    }
    else
    {
      w = c64_polar(1, -TAU32 * (R32)k / (R32)(2*m));
    }
    C64 l = io[k];
    C64 r = io[m - k];

    C64 in0 = c64_lscale(0.5f, c64_add_conj(l, r));
    C64 in1 = c64_lscalei(-0.5f, c64_sub_conj(l, r));

    C64 t = c64_mul(w, in1);
    C64 out0 = c64_add(in0, t);
    C64 out1 = c64_sub(in0, t);

    io[k] = out0;
    io[m - k] = out1;
  }
}}

proc void
ifft_re_convert(C64 *io, U64 real_count) {ProfileFunction()
{
  U64 m = real_count / 2;

  R32 *twiddles_im = fft_sine_table + 2*m;
  R32 *twiddles_re = twiddles_im + m/2;
  twiddles_im += m;

  for(U64 k = 0; k < m/2; ++k)
  {
    if(k == 0)
    {
      R32 dc = io[0].re;
      R32 nq = io[0].im;
      io[0].re = 0.5f*(dc + nq);
      io[0].im = 0.5f*(dc - nq);
      continue;
    }

    // TODO: find a better solution for when real_count is large
    C64 w;
    if(4*m < ArrayCount(fft_sine_table))
    {
      w = c64(twiddles_re[k], twiddles_im[k]);
    }
    else
    {
      w = c64_polar(1, -TAU32 * (R32)k / (R32)(2*m));
    }
    C64 l = io[k];
    C64 r = io[m - k];

    C64 in0 = c64_lscale(0.5f, c64_add_conj(l, r));
    C64 u = c64_lscale(0.5f, c64_sub_conj(l, r));
    C64 in1 = c64_mul_conj(u, w);

    C64 out0 = c64_addi(in0, in1);
    C64 out1 = c64_conj(c64_subi(in0, in1));

    io[k] = out0;
    io[m - k] = out1;
  }
}}

proc void
fft_re(C64 *out, R32 *in, U64 count) {ProfileFunction()
{
  Assert(IsPow2(count));
  fft(out, (C64*)in, count/2);
  fft_re_convert(out, count);
}}

proc void
ifft_re(R32 *out, C64 *in, U64 count) {ProfileFunction()
{
  Assert(IsPow2(count));
  ifft_re_convert(in, count);
  ifft((C64*)out, in, count/2);
}}

proc void
fft(C64 *out, C64 *in, U64 count) {ProfileFunction()
{
  Assert(IsPow2(count));
  U64 initial_level = fft_initial(out, in, count);
  Assert(initial_level >= 2);

  g_initial_level = initial_level;
  fft_rec(out, count);
}}

proc void
ifft(C64 *out, C64 *in, U64 count) {ProfileFunction()
{
  Assert(IsPow2(count));
  U64 initial_level = ifft_initial(out, in, count);
  Assert(initial_level >= 2);

  g_initial_level = initial_level;
  ifft_rec(out, count);
}}

proc void
fft_kernel(C64 *io, U64 count, U64 s) {ProfileFunction()
{
  Assert(s >= 2);

  R32 *twiddles_sin = fft_sine_table + 3*s;
  R32 *twiddles_cos = fft_sine_table + 2*s + s/2;

  for(U64 k = 0; k < count; k += 2*s)
  {
    C64 *io0 = io + 0*s + k;
    C64 *io1 = io + 1*s + k;

    for(U64 j = 0; j < s; ++j)
    {
      // TODO: find a better solution for when s is large
      C64 w;
      if(4*s < ArrayCount(fft_sine_table))
      {
	w = c64(twiddles_cos[j], twiddles_sin[j]);
      }
      else
      {
	w = c64_polar(1, -TAU32 * (R32)j / (R32)(2*s));
      }

      C64 in0 = io0[j];
      C64 in1 = io1[j];

      C64 t = c64_mul(in1, w);
      C64 out0 = c64_add(in0, t);
      C64 out1 = c64_sub(in0, t);

      io0[j] = out0;
      io1[j] = out1;
    }
  }
}}

proc void
ifft_kernel(C64 *io, U64 count, U64 s) {ProfileFunction()
{
  for(U64 k = 0; k < count; k += 2*s)
  {
    R32 *twiddles_sin = fft_sine_table + 2*s;
    R32 *twiddles_cos = fft_sine_table + 2*s + s/2;

    C64 *io0 = io + 0*s + k;
    C64 *io1 = io + 1*s + k;

    for(U64 j = 0; j < s; ++j)
    {
      // TODO: find a better solution for when s is large
      C64 w;
      if(4*s < ArrayCount(fft_sine_table))
      {
	w = c64(twiddles_cos[j], twiddles_sin[j]);
      }
      else
      {
	w = c64_polar(1, TAU32 * (R32)j / (R32)(2*s));
      }

      C64 in0 = io0[j];
      C64 in1 = io1[j];

      C64 t = c64_mul(in1, w);
      C64 out0 = c64_add(in0, t);
      C64 out1 = c64_sub(in0, t);

      io0[j] = out0;
      io1[j] = out1;
    }
  }
}}

proc void
fft_leaf(C64 *io, U64 count) {ProfileFunction()
{
  for(U64 s = g_initial_level; s < count; s *= 2)
  {
    fft_kernel(io, count, s);
  }
}}

proc void
ifft_leaf(C64 *io, U64 count) {ProfileFunction()
{
  for(U64 s = g_initial_level; s < count; s *= 2)
  {
    ifft_kernel(io, count, s);
  }
}}

#define FFT_LEAF_LEVEL 1024
proc void
fft_rec(C64 *io, U64 count) {ProfileFunction()
{
  if(count <= FFT_LEAF_LEVEL)
  {
    fft_leaf(io, count);
    return;
  }

  fft_rec(io + 0*count/2, count/2);
  fft_rec(io + 1*count/2, count/2);

  fft_kernel(io, count, count/2);
}}

proc void
ifft_rec(C64 *io, U64 count) {ProfileFunction()
{
  if(count <= FFT_LEAF_LEVEL)
  {
    ifft_leaf(io, count);
    return;
  }

  ifft_rec(io + 0*count/2, count/2);
  ifft_rec(io + 1*count/2, count/2);

  ifft_kernel(io, count, count/2);
}}

proc B32
fft_re_test(Arena *arena, String8List *log, C64 *expected, R32 *in, U64 count)
{
  B32 result = 1;
  R32 err_tol = 0.001f;
  ArenaTemp scratch = arena_get_scratch(&arena, 1);

  C64 *out = arena_push_array(scratch.arena, C64, count/2);
  fft_re(out, in, count);
  for(U64 bin_idx = 0; bin_idx < count/2; ++bin_idx)
  {
    C64 out_bin = out[bin_idx];
    C64 expected_bin = expected[bin_idx];

    R32 out_bin_mag_sq = c64_mag_sq(out_bin);
    R32 expected_bin_mag_sq = c64_mag_sq(expected_bin);

    R32 err = rygc_abs(out_bin_mag_sq - expected_bin_mag_sq) / expected_bin_mag_sq;
    if(err >= err_tol)
    {
      str8_list_push_f(arena, log,
		       "discrepancy at bin [%llu]:\n"
		       "  expected: %10.4f + %10.4f i,\n"
		       "  got:      %10.4f + %10.4f i\n",
		       bin_idx,
		       expected_bin.re, expected_bin.im,
		       out_bin.re, out_bin.im);
      result = 0;
    }
  }

  arena_release_scratch(scratch);

  return result;
}

proc B32
ifft_re_test(Arena *arena, String8List *log, R32 *expected, C64 *in, U64 count)
{
  B32 result = 1;
  R32 err_tol = 0.01;
  ArenaTemp scratch = arena_get_scratch(&arena, 1);

  R32 *out = arena_push_array(scratch.arena, R32, count);
  ifft_re(out, in, count);
  for(U64 sample_idx = 0; sample_idx < count; ++sample_idx)
  {
    R32 out_sample = out[sample_idx];
    R32 expected_sample = expected[sample_idx];

    R32 err = rygc_abs(out_sample - expected_sample);
    if(err >= err_tol)
    {
      str8_list_push_f(arena, log,
		       "discrepancy at sample [%llu]:\n"
		       "  expected: %10.4f,\n"
		       "       got: %10.4f,\n",
		       sample_idx,
		       expected_sample,
		       out_sample);
      result = 0;
    }
  }

  arena_release_scratch(scratch);

  return result;
}

#include "test/test.h"
TEST_FN_DEF(fft_re)
{
  ArenaTemp scratch = arena_get_scratch(&arena, 1);

  Buffer in_buf = os_read_entire_file(scratch.arena, Str8Lit(DATA_DIR "/test/fft_test_signal.float"));
  Buffer out_buf = os_read_entire_file(scratch.arena, Str8Lit(DATA_DIR "/test/fft_test_result.float"));

  R32 *in = (R32*)in_buf.mem;
  C64 *expected = (C64*)out_buf.mem;
  U64 count = in_buf.size/sizeof(*in);
  expected[0].im = expected[count/2].re; // NOTE: nyquist
  B32 result = fft_re_test(arena, log, expected, in, count);

  arena_release_scratch(scratch);

  return result;
}

TEST_FN_DEF(ifft_re)
{
  ArenaTemp scratch = arena_get_scratch(&arena, 1);

  Buffer in_buf = os_read_entire_file(scratch.arena, Str8Lit(DATA_DIR "/test/fft_test_result.float"));
  Buffer out_buf = os_read_entire_file(scratch.arena, Str8Lit(DATA_DIR "/test/fft_test_signal.float"));

  C64 *in = (C64*)in_buf.mem;
  R32 *expected = (R32*)out_buf.mem;
  U64 count = out_buf.size/sizeof(*expected);
  in[0].im = in[count/2].re; // NOTE: nyquist
  B32 result = ifft_re_test(arena, log, expected, in, count);

  arena_release_scratch(scratch);

  return result;
}

#if 0
proc ComplexBuffer
fft_re(Arena *arena, FloatBuffer in)
{
  // NOTE: DIT radix-2
  U64 count = in.count;

  R32 *re = arena_push_array_z(arena, R32, count);
  R32 *im = arena_push_array_z(arena, R32, count);

  // NOTE: input permutation
  {
    Assert(is_pow_2((U32)count));
    U64 count_log2 = (U64)log2((R64)count);
    R32 *src = in.mem;
    for(U32 i = 0; i < count; ++i) {

      U32 rev_i = bit_reverse_u32(i) >> (sizeof(U32)*8 - count_log2);
      re[rev_i] = src[i];
    }
  }

  // NOTE: twiddles
  {
    for(U32 m = 2; m <= count; m <<= 1) {

      R32 theta = -2.f*PI32/(R32)m;
      R32 wm_re = cosf(theta);
      R32 wm_im = sinf(theta);
      for(U32 k = 0; k < count; k += m) {

	R32 w_re = 1.f;
	R32 w_im = 0.f;
	R32 *at0_re = re + k;
	R32 *at0_im = im + k;
	R32 *at1_re = re + k + m/2;
	R32 *at1_im = im + k + m/2;
	for(U32 j = 0; j < m/2; ++j) {

	  R32 in0_re = *at0_re;
	  R32 in0_im = *at0_im;
	  R32 in1_re = *at1_re;
	  R32 in1_im = *at1_im;

	  R32 t_re = w_re * in1_re - w_im * in1_im;
	  R32 t_im = w_re * in1_im + w_im * in1_re;

	  R32 out0_re = in0_re + t_re;
	  R32 out0_im = in0_im + t_im;
	  R32 out1_re = in0_re - t_re;
	  R32 out1_im = in0_im - t_im;

	  *at0_re++ = out0_re;
	  *at0_im++ = out0_im;
	  *at1_re++ = out1_re;
	  *at1_im++ = out1_im;

	  R32 w_old_re = w_re;
	  R32 w_old_im = w_im;
	  w_re = w_old_re * wm_re - w_old_im * wm_im;
	  w_im = w_old_re * wm_im + w_old_im * wm_re;
	}
      }
    }
  }

  ComplexBuffer result = {0};
  result.count = count;
  result.re = re;
  result.im = im;
  return(result);
}

proc FloatBuffer
ifft_re(Arena *arena, ComplexBuffer in)
{
  // NOTE: DIT radix-2
  U64 count = in.count;

  R32 *re = arena_push_array_z(arena, R32, count);
  R32 *im = arena_push_array_z(arena, R32, count);

  // NOTE: input permutation
  {
    Assert(is_pow_2((U32)count));
    U64 count_log2 = (U64)log2((R64)count);
    R32 inv_count = 1.f/(R32)count;
    R32 *src_re = in.re;
    R32 *src_im = in.im;
    for(U32 i = 0; i < count; ++i) {

      U32 rev_i = bit_reverse_u32(i) >> (sizeof(U32)*8 - count_log2);
      re[rev_i] = inv_count * src_re[i];
      im[rev_i] = inv_count * src_im[i];
    }
  }

  // NOTE: twiddles
  {
    for(U32 m = 2; m <= count; m <<= 1) {

      R32 theta = 2.f*PI32/(R32)m;
      R32 wm_re = cosf(theta);
      R32 wm_im = sinf(theta);
      for(U32 k = 0; k < count; k += m) {

	R32 w_re = 1.f;
	R32 w_im = 0.f;
	R32 *at0_re = re + k;
	R32 *at0_im = im + k;
	R32 *at1_re = re + k + m/2;
	R32 *at1_im = im + k + m/2;
	for(U32 j = 0; j < m/2; ++j) {

	  R32 in0_re = *at0_re;
	  R32 in0_im = *at0_im;
	  R32 in1_re = *at1_re;
	  R32 in1_im = *at1_im;

	  R32 t_re = w_re * in1_re - w_im * in1_im;
	  R32 t_im = w_re * in1_im + w_im * in1_re;

	  R32 out0_re = in0_re + t_re;
	  R32 out0_im = in0_im + t_im;
	  R32 out1_re = in0_re - t_re;
	  R32 out1_im = in0_im - t_im;

	  *at0_re++ = out0_re;
	  *at0_im++ = out0_im;
	  *at1_re++ = out1_re;
	  *at1_im++ = out1_im;

	  R32 w_old_re = w_re;
	  R32 w_old_im = w_im;
	  w_re = w_old_re * wm_re - w_old_im * wm_im;
	  w_im = w_old_re * wm_im + w_old_im * wm_re;
	}
      }
    }
  }

  FloatBuffer result = {0};
  result.count = count;
  result.mem = re;
  return(result);
}
#endif
