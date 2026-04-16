#include "base/base.h"
#include "bench/bench.h"

#include "base/base.c"
#include "bench/bench.c"

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
// benchmark wrappers

typedef struct Fft_Args
{
  R32 *in;
  C64 *out;
  U64 count;
} Fft_Args;

typedef struct Fft_ArgsDeinterleaved
{
  R32 *in;
  R32 *out_re;
  R32 *out_im;
  U64 count;
} Fft_ArgsDeinterleaved;

proc void
fft_bench__recursive(void *args)
{
  Fft_Args *fft_args = args;
  R32 *in = fft_args->in;
  C64 *out = fft_args->out;
  U64 count = fft_args->count;

  fft_re__recursive_dit_radix2(in, out, count, 1);
}

proc void
fft_bench__naive(void *args)
{
  Fft_Args *fft_args = args;
  R32 *in = fft_args->in;
  C64 *out = fft_args->out;
  U64 count = fft_args->count;

  fft_re__naive(in, out, count);
}

proc void
fft_bench__iterative_radix2_scalar(void *args)
{
  Fft_Args *fft_args = args;
  R32 *in = fft_args->in;
  C64 *out = fft_args->out;
  U64 count = fft_args->count;

  fft_re__iterative_dit_radix2_ss(in, out, count);
}

proc void
fft_bench__iterative_radix2_sse(void *args)
{
  Fft_ArgsDeinterleaved *fft_args = args;
  R32 *in = fft_args->in;
  R32 *out_re = fft_args->out_re;
  R32 *out_im = fft_args->out_im;
  U64 count = fft_args->count;

  fft_re__iterative_dit_radix2_ps_sse(in, out_re, out_im, count);
}

proc void
fft_bench__iterative_radix2_avx2(void *args)
{
  Fft_ArgsDeinterleaved *fft_args = args;
  R32 *in = fft_args->in;
  R32 *out_re = fft_args->out_re;
  R32 *out_im = fft_args->out_im;
  U64 count = fft_args->count;

  fft_re__iterative_dit_radix2_ps_avx2(in, out_re, out_im, count);
}

typedef struct Fft_Bench
{
  Bench_Proc *fft_proc;
  void *fft_args;
  String8 proc_name;
} Fft_Bench;

// (name, arg_kind)
#define FFT_BENCH_XLIST\
  X(naive, interleaved)\
  X(recursive, interleaved)\
  X(iterative_radix2_scalar, interleaved)\
  X(iterative_radix2_sse, deinterleaved)\
  X(iterative_radix2_avx2, deinterleaved)\

// -----------------------------------------------------------------------------
// driving code

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  if(!os_init()) return(1);
  if(!bench_init()) return(1);

  Bench_Counter counters[] = {
    Bench_Counter_cpu_cycles,
    Bench_Counter_cache_references,
    Bench_Counter_cache_misses,
    Bench_Counter_branch_instructions,
    Bench_Counter_branch_misses,
  };
  if(!bench_use_counters(counters, ArrayCount(counters))) return 1;

  bench_repetition_time_ms(500);

  Arena *arena = arena_alloc();
  U64 const fft_count = 2048;
  R32 *signal = arena_push_array(arena, R32, fft_count);
  {
    R32 const freq = 4;
    R32 const dp = TAU32 * freq / (R32)fft_count;
    R32 phasor = 0.f;
    for(U32 i = 0; i < fft_count; ++i)
    {
      signal[i] = sinf(phasor);
      phasor += dp;
      if(phasor >= TAU32) phasor -= TAU32;
    }
  }
  C64 *out = arena_push_array(arena, C64, fft_count);

  U64 const padding = 16;
  R32 *out_re = arena_push_array(arena, R32, fft_count + padding);
  R32 *out_im = arena_push_array(arena, R32, fft_count + padding);

  Fft_Args fft_bench_args__interleaved = {
    .in = signal,
    .out = out,
    .count = fft_count,
  };

  Fft_ArgsDeinterleaved fft_bench_args__deinterleaved = {
    .in = signal,
    .out_re = out_re,
    .out_im = out_im,
    .count = fft_count,
  };

  Fft_Bench fft_benches[] = {
#define X(name, arg_kind) (Fft_Bench){			\
      .fft_proc  = Glue(fft_bench__, name),		\
      .fft_args  = &Glue(fft_bench_args__, arg_kind),	\
      .proc_name = Str8Lit(Stringify(name)),		\
    },
    FFT_BENCH_XLIST
#undef X
  };

  printf("fft: %lu samples\n", fft_count);
  for(U32 p_idx = 0; p_idx < ArrayCount(fft_benches); ++p_idx)
  {
    Fft_Bench *bench = fft_benches + p_idx;
    Bench_Proc *fft_proc = bench->fft_proc;
    void *args = bench->fft_args;
    String8 proc_name = bench->proc_name;

#if 0
    fft_proc(args);
    printf("\n%.*s:\n", Str8F(proc_name));
    if(IntFromPtr(args) == IntFromPtr(&fft_bench_args__interleaved))
    {
      for(U64 i = 0; i < fft_count; ++i)
      {
	printf("[%4lu] = (%8.4f, %8.4f)\n", i, out[i].re, out[i].im);
      }
    }
    else
    {
      for(U64 i = 0; i < fft_count; ++i)
      {
	printf("[%4lu] = (%8.4f, %8.4f)\n", i, out_re[i], out_im[i]);
      }
    }
#else
    Bench_Result res = bench_run_proc(fft_proc, args);
    printf("\n%.*s: %*lu iterations\n",
	   Str8F(proc_name), (int)(32 - proc_name.count - 1), res.run_count);
    printf("  cycle count:\n"
	   "    avg: %24lu,\n"
	   "    min: %24lu,\n"
	   "    max: %24lu,\n",
	   res.counters[0].avg,
	   res.counters[0].min,
	   res.counters[0].max);

    printf("  cache misses:\n"
	   "    avg: %24lu,\n"
	   "    min: %24lu/%lu,\n"
	   "    max: %24lu/%lu,\n",
	   res.counters[2].avg,
	   res.counters[2].min, res.counters[1].min,
	   res.counters[2].max, res.counters[1].max);

    printf("  branch misses:\n"
	   "    avg: %24lu,\n"
	   "    min: %24lu/%lu,\n"
	   "    max: %24lu/%lu,\n",
	   res.counters[4].avg,
	   res.counters[4].min, res.counters[3].min,
	   res.counters[4].max, res.counters[3].max);
#endif
    ZeroArray(out, C64, fft_count);
    ZeroArray(out_re, R32, fft_count);
    ZeroArray(out_im, R32, fft_count);
  }

  return(0);
}
