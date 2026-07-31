proc B32
bench_init__mac(void)
{
  void *kperf = 0;
  void *kperfdata = 0;

  int status = 0;
  kpep_db *kpepdb = 0;
  kpep_config *kpepcfg = 0;

  // NOTE: load functions
  kperf = dlopen("/System/Library/PrivateFrameworks/kperf.framework/kperf", RTLD_LAZY);
  if(kperf == 0) goto bench_init__mac_failure;

#define X(ret, name, args)\
  name = dlsym(kperf, Stringify(name)); if(name == 0) goto bench_init__mac_failure;
  KPERF_PROC_XLIST;
#undef X

  kperfdata = dlopen("/System/Library/PrivateFrameworks/kperfdata.framework/kperfdata", RTLD_LAZY);
  if(kperfdata == 0) goto bench_init__mac_failure;

#define X(ret, name, args)\
  name = dlsym(kperfdata, Stringify(name)); if(name == 0) goto bench_init__mac_failure;
  KPERFDATA_PROC_XLIST;
#undef X

  if((status = kpep_db_create(0, &kpepdb)) != 0) goto bench_init__mac_failure;
  if((status = kpep_config_create(kpepdb, &kpepcfg))) goto bench_init__mac_failure;
  if((status = kpep_config_force_counters(kpepcfg))) goto bench_init__mac_failure;

  U32 max_counter_count = kpc_get_counter_count(KPC_CLASS_CONFIGURABLE_MASK);

  Arena *arena = bench_state->arena;
  bench_state->counters = arena_push_array(arena, U32, max_counter_count);
  bench_state->max_counter_count = max_counter_count;

  bench_mac_state = arena_push_struct(bench_state->arena, Bench_MacState);
  bench_mac_state->kpepdb = kpepdb;
  bench_mac_state->kpepcfg = kpepcfg;

  dlclose(kperf);
  dlclose(kperfdata);

  return 1;

bench_init__mac_failure:
  if(kperf != 0) dlclose(kperf);
  if(kperfdata != 0) dlclose(kperfdata);
  bench_mac_state = 0;
  return 0;
}

proc B32
bench_use_counters__mac(void)
{
  kpep_db *kpepdb = bench_mac_state->kpepdb;
  kpep_config *kpepcfg = bench_mac_state->kpepcfg;
  int status = 0;

  for(U32 c = 0; c < bench_state->counter_count; ++c)
  {
    Bench_Counter e = bench_state->counters[c];
    B32 event_supported = 0;
    for(U32 n = 0; n < ArrayCount(mac_bench_event_names[e]); ++n)
    {
      kpep_event *event;
      const char *name = mac_bench_event_names[e][n];
      if(name == 0) continue;

      if(kpep_db_event(kpepdb, mac_bench_event_names[e][n], &event) == 0)
      {
	// NOTE: 1 means user space only
	if((status = kpep_config_add_event(kpepcfg, &event, 1, 0)) != 0) goto bench_use_counters__mac_failure;
	event_supported = 1;
	break;
      }
    }

    if(!event_supported) goto bench_use_counters__mac_failure;
  }

  U32 *counter_classes = &bench_mac_state->counter_classes;
  size_t *counter_reg_count = &bench_mac_state->counter_reg_count;
  size_t *counter_map = &bench_mac_state->counter_map[0];
  kpc_config_t *counter_regs = &bench_mac_state->counter_regs[0];
  if((status = kpep_config_kpc_classes(kpepcfg, counter_classes)) != 0) goto bench_use_counters__mac_failure;
  if((status = kpep_config_kpc_count(kpepcfg, counter_reg_count)) != 0) goto bench_use_counters__mac_failure;
  if((status = kpep_config_kpc_map(kpepcfg, counter_map, sizeof(bench_mac_state->counter_map))) != 0) goto bench_use_counters__mac_failure;
  if((status = kpep_config_kpc(kpepcfg, counter_regs, sizeof(bench_mac_state->counter_regs))) != 0) goto bench_use_counters__mac_failure;
  bench_mac_state->result_buf = arena_push_array(bench_state->arena, Bench_CounterResult, bench_state->counter_count);

  return 1;

bench_use_counters__mac_failure:
  return 0;
}

proc Bench_Result
bench_run_proc__mac(Bench_Proc *p, void *args)
{
  int status = 0;

  qos_class_t thread_class;
  int thread_priority;
  pthread_get_qos_class_np(pthread_self(), &thread_class, &thread_priority);

  pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, thread_priority);

  int counters_enabled = kpc_force_all_ctrs_set(1);
  if(counters_enabled == 0)
  {
    if((bench_mac_state->counter_classes & KPC_CLASS_CONFIGURABLE_MASK) && bench_mac_state->counter_reg_count)
    {
      if((status = kpc_set_config(bench_mac_state->counter_classes, bench_mac_state->counter_regs)) != 0) Assert(0); // TODO: better error handling
    }

    if((status = kpc_set_counting(bench_mac_state->counter_classes)) != 0) Assert(0); // TODO: better error handling
    if((status = kpc_set_thread_counting(bench_mac_state->counter_classes)) != 0) Assert(0); // TODO: better error handling
  }

  /* struct rusage usage_start; */
  /* getrusage(RUSAGE_SELF, &usage_start); */

  // NOTE: init counts
  U64 counter_count = bench_state->counter_count;
  Bench_CounterResult *result_buf = bench_mac_state->result_buf;
  ZeroArray(result_buf, Bench_CounterResult, counter_count);
  for(U64 counter_idx = 0; counter_idx < counter_count; ++counter_idx)
  {
    result_buf[counter_idx].min = U64_MAX;
  }

  U64 run_count = 0;
  U64 time_elapsed = 0;
  U64 time_to_run = bench_state->repetition_time_ms * os_counter_freq() / 1000;
  U64 time_start = os_counter();
  while(time_elapsed < time_to_run)
  {
    U64 counters_start[KPC_MAX_COUNTERS] = {0};
    if(counters_enabled == 0)
    {
      if((status = kpc_get_thread_counters(0, KPC_MAX_COUNTERS, counters_start)) != 0) Assert(0); // TODO: better error handling
    }

    p(args);

    U64 counters_end[KPC_MAX_COUNTERS] = {0};
    if(counters_enabled == 0)
    {
      if((status = kpc_get_thread_counters(0, KPC_MAX_COUNTERS, counters_end)) != 0) Assert(0); // TODO: better error handling
    }

    time_elapsed = os_counter() - time_start;

    for(U64 i = 0; i < counter_count; ++i)
    {
      size_t idx = bench_mac_state->counter_map[i];
      U64 count = counters_end[idx] - counters_start[idx];

      U64 min = result_buf[i].min;
      result_buf[i].min = Min(min, count);

      U64 max = result_buf[i].max;
      result_buf[i].max = Max(max, count);

      result_buf[i].avg += count;
    }

    ++run_count;
  }

  if(counters_enabled == 0)
  {
    kpc_set_thread_counting(0);
    kpc_set_counting(0);
    kpc_force_all_ctrs_set(0);
  }

  pthread_set_qos_class_self_np(thread_class, thread_priority);

  // NOTE: compute averages
  for(U64 counter_idx = 0; counter_idx < counter_count; ++counter_idx)
  {
    U64 sum = result_buf[counter_idx].avg;
    result_buf[counter_idx].avg = (U64)((R64)sum / (R64)run_count);
  }

  Bench_Result result = {0};
  result.counters = result_buf;
  result.counters_count = counter_count;
  result.run_count = run_count;
  return result;
}
