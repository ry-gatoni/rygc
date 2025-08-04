#include "base/base.h"
#include "os/os.h"
#include "fourier/fourier.h"

#include "base/base.c"
#include "os/os.c"
#include "fourier/fourier.c"

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);
  
  ArenaTemp scratch = arena_get_scratch(0, 0);

  // NOTE: load signal and result files
  String8 test_signal_file_path = Str8Lit("../data/test/fft_test_signal.float");
  String8 test_signal_file = os_read_entire_file(scratch.arena, test_signal_file_path);

  String8 model_result_file_path = Str8Lit("../data/test/fft_test_result.float");
  String8 model_result_file = os_read_entire_file(scratch.arena, model_result_file_path);

  // NOTE: prepare signal and result buffers
  FloatBuffer test_signal = {.count = test_signal_file.count/sizeof(R32),
			     .mem = (R32*)test_signal_file.string};
  ComplexBuffer model_result = {.count = test_signal.count,
			       .re = arena_push_array(scratch.arena, R32, test_signal.count),
			       .im = arena_push_array(scratch.arena, R32, test_signal.count)};
  {
    // NOTE: [(re, im) ...] -> ([re ...], [im ...])
    R32 *src = (R32*)model_result_file.string;
    for(U32 i = 0; i < model_result.count; ++i) {

      model_result.re[i] = src[i];
      model_result.im[i] = src[i+1];
    }
  }

  // NOTE: compute fft
  ComplexBuffer test_result = fft_re(scratch.arena, test_signal);

  // NOTE: compare against model
  B32 result = 1;
  R32 eps = 1e-5;
  Assert(test_result.count == model_result.count);  
  for(U32 i = 0; i < test_result.count; ++i) {

    result = ((fabsf(test_result.re[i] - model_result.re[i]) < eps) &&
	      (fabsf(test_result.im[i] - model_result.im[i]) < eps));
    if(!result) {

      fprintf(stderr,
	      "fft result sample %u outside of target range\n  computed: (%.6f, %.6f)\n  target: (%.6f, %.6f)\n",
	      i,
	      test_result.re[i], test_result.im[i],
	      model_result.re[i], model_result.im[i]);
    }
  }
  
  arena_release_scratch(scratch);
  return(result ? 0 : 1);
}
