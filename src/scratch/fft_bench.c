#include "base/base.h"

#include "base/base.c"

typedef struct FftResult
{
  R32 *out_re;
  R32 *out_im;
} FftResult;

#define FFT_TEST_PROC(name) FftResult name (Arena *arena, R32 *in, U64 count);
typedef FFT_TEST_PROC(FftTestProc);

proc FFT_TEST_PROC(fft_test__recursive_dit_radix2);
proc FFT_TEST_PROC(fft_test__naive);
proc FFT_TEST_PROC(fft_test__iterative_dit_radix2_ss);
proc FFT_TEST_PROC(fft_test__iterative_dit_radix2_ps_sse);
proc FFT_TEST_PROC(fft_test__iterative_dit_radix2_ps_avx2);

// -----------------------------------------------------------------------------
// fft implementations

proc void
fft_re__recursive_dit_radix2(R32 *in, C64 *out, U64 count, U64 stride)
{ProfileFunction(){

  if(count == 1)
  {
    out[0] = c64(in[0], 0);
    return;
  }

  fft_re__recursive_dit_radix2(in,          out,           count/2, 2*stride);
  fft_re__recursive_dit_radix2(in + stride, out + count/2, count/2, 2*stride);

  R32 f = -TAU32 / (R32)count;
  C64 wk = c64(cosf(f), sinf(f));
  C64 w = c64(1, 0);
  for(U64 k = 0; k < count/2; ++k)
  {
    C64 e = out[k];
    C64 o = c64_mul(out[k + count/2], w);

    out[k]           = c64_add(e, o);
    out[k + count/2] = c64_sub(e, o);

    w = c64_mul(w, wk);
  }
}}

proc void
fft_re__naive(R32 *in, C64 *out, U64 count)
{ProfileFunction(){

  for(U64 k = 0; k < count; ++k)
  {
    R32 fk = TAU32 * k / (R32)count;
    C64 wk = c64(cosf(fk), sinf(fk));
    C64 w = c64(1, 0);
    for(U64 t = 0; t < count; ++t)
    {
      C64 out_k = out[k];
      out[k] = c64_add(out_k, c64_mul(w, c64(in[t], 0)));
      w = c64_mul(w, wk);
    }
  }
}}

proc void
fft_re__iterative_dit_radix2_ss(R32 *in, C64 *out, U64 count)
{ProfileFunction(){

  U64 count_log2 = MSB(count);
  for(U64 i = 0; i < count; ++i)
  {
    U64 i_rev = bit_reverse_u64(i) >> (64 - count_log2);
    out[i_rev] = c64(in[i], 0);
  }

  for(U64 s = 2; s <= count; s <<= 1)
  {
    R32 f = -TAU32 / (R32)s;
    C64 ws = c64(cosf(f), sinf(f));
    for(U64 k = 0; k < count; k += s)
    {
      C64 w = c64(1, 0);

      for(U64 j = 0; j < s/2; ++j)
      {
	C64 p = out[k + j];
	C64 q = c64_mul(w, out[k + s/2 + j]);

	out[k + j]       = c64_add(p, q);
	out[k + s/2 + j] = c64_sub(p, q);

	w = c64_mul(w, ws);
      }
    }
  }
}}

#include <immintrin.h>

proc void
fft_re__iterative_dit_radix2_ps_sse(R32 *in, R32 *out_re, R32 *out_im, U64 count)
{ProfileFunction(){

  U64 count_log2 = MSB(count);
  for(U64 i = 0; i < count; ++i)
  {
    U64 i_rev = bit_reverse_u64(i) >> (64 - count_log2);
    out_re[i_rev] = in[i];
  }

  for(U64 s = 2; s <= count; s <<= 1)
  {
    R32 f = -TAU32 / (R32)s;
    if(s/2 >= 4)
    {
      R32 ws1_re = cosf(f);//c64(cosf(f), sinf(f));
      R32 ws1_im = sinf(f);
      R32 ws2_re = ws1_re*ws1_re - ws1_im*ws1_im;
      R32 ws2_im = ws1_re*ws1_im + ws1_im*ws1_re;
      R32 ws3_re = ws2_re*ws1_re - ws2_im*ws1_im;
      R32 ws3_im = ws2_re*ws1_im + ws2_im*ws1_re;
      R32 ws4_re = ws2_re*ws2_re - ws2_im*ws2_im;
      R32 ws4_im = ws2_re*ws2_im + ws2_im*ws2_re;
      __m128 ws_re = _mm_setr_ps(ws4_re, ws4_re, ws4_re, ws4_re);
      __m128 ws_im = _mm_setr_ps(ws4_im, ws4_im, ws4_im, ws4_im);

      for(U64 k = 0; k < count; k += s)
      {
	//C64 w = c64(1, 0);
	__m128 w_re = _mm_setr_ps(1, ws1_re, ws2_re, ws3_re);
	__m128 w_im = _mm_setr_ps(0, ws1_im, ws2_im, ws3_im);

	R32 *out_e_re = out_re + k;
	R32 *out_e_im = out_im + k;
	R32 *out_o_re = out_re + k + s/2;
	R32 *out_o_im = out_im + k + s/2;
	for(U64 j = 0; j < s/2; j += 4)
	{
	  __m128 p_re = _mm_loadu_ps(out_e_re);
	  __m128 p_im = _mm_loadu_ps(out_e_im);
	  __m128 t_re = _mm_loadu_ps(out_o_re);
	  __m128 t_im = _mm_loadu_ps(out_o_im);
	  __m128 q_re = _mm_sub_ps(_mm_mul_ps(w_re, t_re),
				   _mm_mul_ps(w_im, t_im));
	  __m128 q_im = _mm_add_ps(_mm_mul_ps(w_re, t_im),
				   _mm_mul_ps(w_im, t_re));
	  /* C64 p = out[k + j]; */
	  /* C64 q = c64_mul(w, out[k + s/2 + j]); */

	  /* out[k + j]       = c64_add(p, q); */
	  /* out[k + s/2 + j] = c64_sub(p, q); */
	  __m128 dest_e_re = _mm_add_ps(p_re, q_re);
	  __m128 dest_e_im = _mm_add_ps(p_im, q_im);
	  __m128 dest_o_re = _mm_sub_ps(p_re, q_re);
	  __m128 dest_o_im = _mm_sub_ps(p_im, q_im);
	  _mm_storeu_ps(out_e_re, dest_e_re);
	  _mm_storeu_ps(out_e_im, dest_e_im);
	  _mm_storeu_ps(out_o_re, dest_o_re);
	  _mm_storeu_ps(out_o_im, dest_o_im);

	  //w = c64_mul(w, ws);
	  __m128 w_re_old = w_re;
	  w_re = _mm_sub_ps(_mm_mul_ps(w_re, ws_re),
			    _mm_mul_ps(w_im, ws_im));
	  w_im = _mm_add_ps(_mm_mul_ps(w_re_old, ws_im),
			    _mm_mul_ps(w_im, ws_re));

	  out_e_re += 4;
	  out_e_im += 4;
	  out_o_re += 4;
	  out_o_im += 4;
	}
      }
    }
    else
    {
      C64 ws = c64(cosf(f), sinf(f));

      for(U64 k = 0; k < count; k += s)
      {
	C64 w = c64(1, 0);

	for(U64 j = 0; j < s/2; ++j)
	{
	  R32 out_e_re = out_re[k + j];
	  R32 out_e_im = out_im[k + j];
	  R32 out_o_re = out_re[k + s/2 + j];
	  R32 out_o_im = out_im[k + s/2 + j];

	  C64 p = c64(out_e_re, out_e_im);
	  C64 t = c64(out_o_re, out_o_im);
	  C64 q = c64_mul(w, t);

	  C64 dest_e = c64_add(p, q);
	  C64 dest_o = c64_sub(p, q);
	  out_re[k + j]       = dest_e.re;
	  out_im[k + j]       = dest_e.im;
	  out_re[k + s/2 + j] = dest_o.re;
	  out_im[k + s/2 + j] = dest_o.im;

	  w = c64_mul(w, ws);
	}
      }
    }
  }
}}

proc void
fft_re__iterative_dit_radix2_ps_avx2(R32 *in, R32 *out_re, R32 *out_im, U64 count)
{ProfileFunction(){

  U64 count_log2 = MSB(count);
  for(U64 i = 0; i < count; ++i)
  {
    U64 i_rev = bit_reverse_u64(i) >> (64 - count_log2);
    out_re[i_rev] = in[i];
  }

  for(U64 s = 2; s <= count; s <<= 1)
  {
    R32 f = -TAU32 / (R32)s;
    //C64 ws = c64(cosf(f), sinf(f));
    R32 ws1_re = cosf(f);
    R32 ws1_im = sinf(f);
    R32 ws2_re = ws1_re*ws1_re - ws1_im*ws1_im;
    R32 ws2_im = ws1_re*ws1_im + ws1_im*ws1_re;
    R32 ws3_re = ws2_re*ws1_re - ws2_im*ws1_im;
    R32 ws3_im = ws2_re*ws1_im + ws2_im*ws1_re;
    R32 ws4_re = ws2_re*ws2_re - ws2_im*ws2_im;
    R32 ws4_im = ws2_re*ws2_im + ws2_im*ws2_re;
    R32 ws5_re = ws3_re*ws2_re - ws3_im*ws2_im;
    R32 ws5_im = ws3_re*ws2_im + ws3_im*ws2_re;
    R32 ws6_re = ws3_re*ws3_re - ws3_im*ws3_im;
    R32 ws6_im = ws3_re*ws3_im + ws3_im*ws3_re;
    R32 ws7_re = ws4_re*ws3_re - ws4_im*ws3_im;
    R32 ws7_im = ws4_re*ws3_im + ws4_im*ws3_re;
    R32 ws8_re = ws4_re*ws4_re - ws4_im*ws4_im;
    R32 ws8_im = ws4_re*ws4_im + ws4_im*ws4_re;
    __m256 ws_re = _mm256_set1_ps(ws8_re);
    __m256 ws_im = _mm256_set1_ps(ws8_im);

    for(U64 k = 0; k < count; k += s)
    {
      //C64 w = c64(1, 0);
      __m256 w_re = _mm256_setr_ps(1, ws1_re, ws2_re, ws3_re, ws4_re, ws5_re, ws6_re, ws7_re);
      __m256 w_im = _mm256_setr_ps(0, ws1_im, ws2_im, ws3_im, ws4_im, ws5_im, ws6_im, ws7_im);

      R32 *out_e_re = out_re + k;
      R32 *out_e_im = out_im + k;
      R32 *out_o_re = out_re + k + s/2;
      R32 *out_o_im = out_im + k + s/2;

      for(U64 j = 0; j < s/2; j += 8)
      {
	__m256i mask = _mm256_setr_epi32(((j + 0) < s/2) ? 0xFFFFFFFF : 0,
					 ((j + 1) < s/2) ? 0xFFFFFFFF : 0,
					 ((j + 2) < s/2) ? 0xFFFFFFFF : 0,
					 ((j + 3) < s/2) ? 0xFFFFFFFF : 0,
					 ((j + 4) < s/2) ? 0xFFFFFFFF : 0,
					 ((j + 5) < s/2) ? 0xFFFFFFFF : 0,
					 ((j + 6) < s/2) ? 0xFFFFFFFF : 0,
					 ((j + 7) < s/2) ? 0xFFFFFFFF : 0);

	/* C64 p = out[k + j]; */
	/* C64 q = c64_mul(w, out[k + s/2 + j]); */
	__m256 p_re = _mm256_maskload_ps(out_e_re, mask);
	__m256 p_im = _mm256_maskload_ps(out_e_im, mask);
	__m256 t_re = _mm256_maskload_ps(out_o_re, mask);
	__m256 t_im = _mm256_maskload_ps(out_o_im, mask);
	__m256 q_re = _mm256_sub_ps(_mm256_mul_ps(w_re, t_re),
				    _mm256_mul_ps(w_im, t_im));
	__m256 q_im = _mm256_add_ps(_mm256_mul_ps(w_re, t_im),
				    _mm256_mul_ps(w_im, t_re));

	/* out[k + j]       = c64_add(p, q); */
	/* out[k + s/2 + j] = c64_sub(p, q); */
	__m256 dest_e_re = _mm256_add_ps(p_re, q_re);
	__m256 dest_e_im = _mm256_add_ps(p_im, q_im);
	__m256 dest_o_re = _mm256_sub_ps(p_re, q_re);
	__m256 dest_o_im = _mm256_sub_ps(p_im, q_im);
	_mm256_maskstore_ps(out_e_re, mask, dest_e_re);
	_mm256_maskstore_ps(out_e_im, mask, dest_e_im);
	_mm256_maskstore_ps(out_o_re, mask, dest_o_re);
	_mm256_maskstore_ps(out_o_im, mask, dest_o_im);

	//w = c64_mul(w, ws);
	__m256 w_re_old = w_re;
	w_re = _mm256_sub_ps(_mm256_mul_ps(w_re, ws_re),
			     _mm256_mul_ps(w_im, ws_im));
	w_im = _mm256_add_ps(_mm256_mul_ps(w_re_old, ws_im),
			     _mm256_mul_ps(w_im, ws_re));

	out_e_re += 8;
	out_e_im += 8;
	out_o_re += 8;
	out_o_im += 8;
      }
    }
  }
}}

// -----------------------------------------------------------------------------
// test wrappers

proc FftResult
fft_test__recursive_dit_radix2(Arena *arena, R32 *in, U64 count)
{
  R32 *out_re = 0;
  R32 *out_im = 0;
  ProfileScope(recursive_setup)
  {
    ArenaTemp scratch = arena_get_scratch(&arena, 1);

    C64 *out_complex = arena_push_array_z(scratch.arena, C64, count);
    fft_re__recursive_dit_radix2(in, out_complex, count, 1);

    out_re = arena_push_array(arena, R32, 2*count);
    out_im = out_re + count;
    for(U64 i = 0; i < count; ++i)
    {
      out_re[i] = out_complex[i].re;
      out_im[i] = out_complex[i].im;
    }

    arena_release_scratch(scratch);
  }

  FftResult result = {0};
  result.out_re = out_re;
  result.out_im = out_im;
  return(result);
}

proc FftResult
fft_test__naive(Arena *arena, R32 *in, U64 count)
{
  R32 *out_re = 0;
  R32 *out_im = 0;
  ProfileScope(naive_setup)
  {
    ArenaTemp scratch = arena_get_scratch(&arena, 1);

    C64 *out_complex = arena_push_array_z(scratch.arena, C64, count);
    fft_re__naive(in, out_complex, count);

    out_re = arena_push_array(arena, R32, 2*count);
    out_im = out_re + count;
    for(U64 i = 0; i < count; ++i)
    {
      out_re[i] = out_complex[i].re;
      out_im[i] = out_complex[i].im;
    }

    arena_release_scratch(scratch);
  }

  FftResult result = {0};
  result.out_re = out_re;
  result.out_im = out_im;
  return(result);
}

proc FftResult
fft_test__iterative_dit_radix2_ss(Arena *arena, R32 *in, U64 count)
{
  R32 *out_re = 0;
  R32 *out_im = 0;
  ProfileScope(iterative_ss_setup)
  {
    ArenaTemp scratch = arena_get_scratch(&arena, 1);

    C64 *out_complex = arena_push_array_z(scratch.arena, C64, count);
    fft_re__iterative_dit_radix2_ss(in, out_complex, count);

    out_re = arena_push_array(arena, R32, 2*count);
    out_im = out_re + count;
    for(U64 i = 0; i < count; ++i)
    {
      out_re[i] = out_complex[i].re;
      out_im[i] = out_complex[i].im;
    }

    arena_release_scratch(scratch);
  }

  FftResult result = {0};
  result.out_re = out_re;
  result.out_im = out_im;
  return(result);
}

proc FftResult
fft_test__iterative_dit_radix2_ps_sse(Arena *arena, R32 *in, U64 count)
{
  R32 *out_re = 0;
  R32 *out_im = 0;
  ProfileScope(iterative_ps_sse_setup)
  {
    U32 const padding = 4;
    out_re = arena_push_array_z(arena, R32, 2*(count + padding));
    out_im = out_re + count + padding;
    fft_re__iterative_dit_radix2_ps_sse(in, out_re, out_im, count);
  }

  FftResult result = {0};
  result.out_re = out_re;
  result.out_im = out_im;
  return(result);
}

proc FftResult
fft_test__iterative_dit_radix2_ps_avx2(Arena *arena, R32 *in, U64 count)
{
  R32 *out_re = 0;
  R32 *out_im = 0;
  ProfileScope(iterative_ps_avx2_setup)
  {
    U32 const padding = 8;
    out_re = arena_push_array_z(arena, R32, 2*(count + padding));
    out_im = out_re + count + padding;
    fft_re__iterative_dit_radix2_ps_avx2(in, out_re, out_im, count);
  }

  FftResult result = {0};
  result.out_re = out_re;
  result.out_im = out_im;
  return(result);
}

#define FFT_TEST_XLIST\
  X("recursive_dit_radix2", fft_test__recursive_dit_radix2)\
  X("naive", fft_test__naive)\
  X("iterative_dit_radix2_ss", fft_test__iterative_dit_radix2_ss)\
  X("iterative_dit_radix2_ps_sse", fft_test__iterative_dit_radix2_ps_sse)\
  X("iterative_dit_radix2_ps_avx2", fft_test__iterative_dit_radix2_ps_avx2)

typedef struct FftRepetitionTester
{
  String8 name;
  FftTestProc *test_proc;
  FftResult result;
  Profiler *profiler;
} FftRepetitionTester;

FftRepetitionTester fft_testers[] = {
#define X(n, p) (FftRepetitionTester){ .name = Str8Lit(n), .test_proc = p },
  FFT_TEST_XLIST
#undef X
};

#if 0
proc void
init_testers(void)
{
  for(U32 i = 0; i < ArrayCount(fft_testers); ++i)
  {
    fft_testers[i].profiler = profile_alloc();
  }
}

proc void
tester_run(FftRepetitionTester *tester)
{
  profile_select(tester->profiler);
  profile_begin();
  profile_end();
}
#endif

/* FftTestProc *fft_test_procs[] = { */
/*   fft_test__recursive_dit_radix2, */
/*   fft_test__naive, */
/*   fft_test__iterative_dit_radix2, */
/* }; */

/* String8 fft_test_proc_names[] = { */
/*   Str8Lit("recursive_dit_radix2"), */
/*   Str8Lit("naive"), */
/*   Str8Lit("iterative_dit_radix2"), */
/* }; */

// -----------------------------------------------------------------------------
// driving code

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  if(!os_init()) return(1);

  Arena *arena = arena_alloc();

  Log *logger = log_alloc();
  log_select(logger);

  Profiler *profiler = profile_alloc();
  profile_select(profiler);

  U64 signal_count = 1024;
  R32 *signal = arena_push_array(arena, R32, signal_count);
  {
    R32 phasor_freq = 4.f;
    R32 pv = TAU32 * phasor_freq / (R32)signal_count;
    R32 phasor = 0;

    for(U64 i = 0; i < signal_count; ++i)
    {
      signal[i] = cosf(phasor);

      phasor += pv;
      if(phasor >= TAU32) phasor -= TAU32;
    }
  }

  R64 ms_from_os_counter = 1000.0 / (R64)os_counter_freq();
  R64 time_to_run = 500.0;

  ArenaTemp scratch = arena_get_scratch(0, 0);


  for(U32 test_idx = 0; test_idx < ArrayCount(fft_testers); ++test_idx)
  {
    FftRepetitionTester *tester = fft_testers + test_idx;

    log_begin_scope(tester->name);

    U64 time_start = os_counter();
    U64 time_elapsed = ms_from_os_counter*(os_counter() - time_start);

    profile_begin();
    //while(time_elapsed < time_to_run)
    //{
      FftResult result = tester->test_proc(scratch.arena, signal, signal_count);
      time_elapsed = ms_from_os_counter*(os_counter() - time_start);
      //}
    profile_end();

    LogScopeResult log_scope = log_end_scope();
    printf("\n%.*s\n", Str8F(log_scope.msgs[LogMessageKind_info]));

#if 0
    printf("\n%.*s:\n", Str8F(tester->name));
    for(U64 i = 0; i < signal_count; ++i)
    {
      printf("[%2lu] = (%8.4f, %8.4f)\n", i, result.out_re[i], result.out_im[i]);
    }
#endif

    arena_clear(scratch.arena);
  }

  arena_release_scratch(scratch);

  return(0);
}
