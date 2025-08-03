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
