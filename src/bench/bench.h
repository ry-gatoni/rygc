/** TODO:
 * - features:
 *   - run procedure many times
 *   - min/max/avg counts across many runs
 *   - more counters
 * - bugs:
 *   - investigate amd leaf 0x80000022 being unavailable (fedora 43)
 */

#include "bench.common.h"

#if OS_LINUX
#  include "bench.linux.h"
global B32 (*bench_init__platform)(void) = bench_init__linux;
global B32 (*bench_use_counters__platform)(void) = bench_use_counters__linux;
global Bench_Result (*bench_run_proc__platform)(Bench_Proc *p, void *args) = bench_run_proc__linux;
#elif OS_MAC
#  error not implemented
#elif OS_WINDOWS
#  error not implemented
#else
#  error unsupported os
#endif
