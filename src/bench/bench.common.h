typedef void (Bench_Proc)(void *args);

typedef enum Bench_Counter
{
  Bench_Counter_cpu_cycles,
  Bench_Counter_cache_references,
  Bench_Counter_cache_misses,
  Bench_Counter_branch_instructions,
  Bench_Counter_branch_misses,
} Bench_Counter;

typedef struct Bench_CounterResult
{
  U64 min;
  U64 max;
  U64 avg;
} Bench_CounterResult;

typedef struct Bench_Result
{
  Bench_CounterResult *counters;
  U64 counters_count;
  U64 run_count;
} Bench_Result;

typedef struct Bench_State
{
  Arena *arena;

  U64 max_counter_count;
  U64 counter_count;
  Bench_Counter *counters;

  U64 repetition_time_ms;

  void *platform_state;
} Bench_State;

global Bench_State *bench_state = 0;

// -----------------------------------------------------------------------------
// functions
// -----------------------------------------------------------------------------
// init
proc B32 bench_init(void);

// -----------------------------------------------------------------------------
// config
proc B32 bench_use_counters(Bench_Counter *counters, U64 counter_count);
proc void bench_repetition_time_ms(U64 ms);

// -----------------------------------------------------------------------------
// run
proc Bench_Result bench_run_proc(Bench_Proc *p, void *args);
