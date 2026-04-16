#include <cpuid.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>

typedef struct Bench_LinuxConfig
{
  U32 type;
  U32 config;
} Bench_LinuxConfig;

global Bench_LinuxConfig const bench_linux_configs[] = {
  [Bench_Counter_cpu_cycles]          = { .type = PERF_TYPE_HARDWARE,
                                          .config = PERF_COUNT_HW_CPU_CYCLES},
  [Bench_Counter_cache_references]    = { .type = PERF_TYPE_HARDWARE,
                                          .config = PERF_COUNT_HW_CACHE_REFERENCES},
  [Bench_Counter_cache_misses]        = { .type = PERF_TYPE_HARDWARE,
                                          .config = PERF_COUNT_HW_CACHE_MISSES},
  [Bench_Counter_branch_instructions] = { .type = PERF_TYPE_HARDWARE,
                                          .config = PERF_COUNT_HW_BRANCH_INSTRUCTIONS},
  [Bench_Counter_branch_misses]        = { .type = PERF_TYPE_HARDWARE,
                                          .config = PERF_COUNT_HW_BRANCH_MISSES},
};

typedef struct Bench_LinuxState
{
  int *fds;
  U64 *counter_buf;
  Bench_CounterResult *result_buf;
} Bench_LinuxState;

global Bench_LinuxState *bench_linux_state = 0;


proc B32 bench_init__linux(void);
proc B32 bench_use_counters__linux(void);
proc Bench_Result bench_run_proc__linux(Bench_Proc *p, void *args);
