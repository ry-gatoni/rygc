proc void fft(C64 *out, C64 *in, U64 count);
proc void ifft(C64 *out, C64 *in, U64 count);

proc void fft_re(C64 *out, R32 *in, U64 count);
proc void ifft_re(R32 *out, C64 *in, U64 count);

#if 0
typedef struct FloatBuffer
{
  U64 count;
  R32 *mem;
} FloatBuffer;

typedef struct ComplexBuffer
{
  U64 count;
  R32 *re;
  R32 *im;
} ComplexBuffer;

proc ComplexBuffer fft_re(Arena *arena, FloatBuffer in);
proc FloatBuffer ifft_re(Arena *arena, ComplexBuffer in);
#endif
