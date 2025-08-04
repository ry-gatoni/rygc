proc ComplexBuffer
fft_re(Arena *arena, FloatBuffer in)
{
  // NOTE: DIT radix-2
  U64 count = in.count;

  R32 *re = arena_push_array_z(arena, R32, count);
  R32 *im = arena_push_array_z(arena, R32, count);

  // NOTE: input permutation
  {
    Assert(is_pow_2(count));
    U64 count_log2 = log2(count);
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
      R32 wm_re = cos(theta);
      R32 wm_im = sin(theta);
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
    Assert(is_pow_2(count));
    U64 count_log2 = log2(count);
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
      R32 wm_re = cos(theta);
      R32 wm_im = sin(theta);
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
