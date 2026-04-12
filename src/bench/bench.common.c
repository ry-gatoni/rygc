proc B32
bench_init(void)
{
  Arena *arena = arena_alloc();
  bench_state = arena_push_struct(arena, Bench_State);
  bench_state->arena = arena;

  B32 result = bench_init__platform();
  return(result);
}

proc B32
bench_use_counters(Bench_Counter *counters, U64 counter_count)
{
  B32 result = counter_count <= bench_state->max_counter_count;
  if(result)
  {
    bench_state->counter_count = counter_count;
    for(U64 counter_idx = 0; counter_idx < counter_count; ++counter_idx)
    {
      bench_state->counters[counter_idx] = counters[counter_idx];
    }

    result = bench_use_counters__platform();
  }

  return(result);
}

proc void
bench_repetition_time_ms(U64 ms)
{
  bench_state->repetition_time_ms = ms;
}

proc Bench_Result
bench_run_proc(Bench_Proc *p, void *args)
{
  Bench_Result result = bench_run_proc__platform(p, args);
  return(result);
}
