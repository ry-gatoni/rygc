proc B32
bench_init__linux(void)
{
  bench_linux_state = arena_push_struct(bench_state->arena, Bench_LinuxState);
  bench_state->platform_state = bench_linux_state;

  B32 result = 1;
#if CPU_X64
  // NOTE: get pmc count
  {
    unsigned eax, ebx, ecx, edx;
    __cpuid(0, eax, ebx, ecx, edx);
    if(ebx == FOURCC("Genu") && edx == FOURCC("ineI") && ecx == FOURCC("ntel"))
    {
      // NOTE: intel cpu
      if(eax >= 0xA)
      {
	// NOTE: "architectural performance monitoring leaf"
	__cpuid(0xA, eax, ebx, ecx, edx);
	bench_state->max_counter_count = ebx & 0xF;
      }
    }
    else if(ebx == FOURCC("Auth") && edx == FOURCC("enti") && ecx == FOURCC("cAMD"))
    {
      // NOTE: amd cpu
      __cpuid(0x80000000, eax, ebx, ecx, edx);
      if(eax >= 0x80000022)
      {
	// NOTE: "exteded performance monitoring and debug" leaf supported
	__cpuid(0x80000022, eax, ebx, ecx, edx);
	if(eax & 1)
	{
	  bench_state->max_counter_count = ebx & 0xF;
	}
      }

      if(bench_state->max_counter_count == 0)
      {
	// NOTE: legacy path when pmc count is not directly available
	__cpuid(0x80000001, eax, ebx, ecx, edx);
	U32 base_family = (eax >> 8) & 0xF;
	U32 extended_family = (eax >> 20) & 0xFF;
	U32 family = (base_family < 0xF) ? base_family : base_family + extended_family;
	if(family >= 0x15)
	{
	  // NOTE: "bulldozer" or higher
	  bench_state->max_counter_count = 6;
	}
	else
	{
	  // TODO: we could be more granular here to add support for older amd cpus
	  bench_state->max_counter_count = 0;
	  result = 0;
	}
      }
    }
    else
    {
      // NOTE: unsupported vendor
      result = 0;
    }
  }
  bench_state->counters = arena_push_array(bench_state->arena, U32, bench_state->max_counter_count);
  bench_linux_state->fds = arena_push_array(bench_state->arena, int, bench_state->max_counter_count);

#else
#  error not implemented
#endif

  return(result);
}

proc B32
bench_use_counters__linux(void)
{
  B32 result = 1;

  // NOTE: open file descriptors for counters
  {
    bench_linux_state->fds[0] = -1;
    for(U32 counter_idx = 0; counter_idx < bench_state->counter_count; ++counter_idx)
    {
      Bench_Counter counter = bench_state->counters[counter_idx];
      Bench_LinuxConfig config = bench_linux_configs[counter];
      struct perf_event_attr perf_attr = {0};
      perf_attr.type = config.type;
      perf_attr.size = sizeof(perf_attr);
      perf_attr.config = config.config;
      perf_attr.disabled = 1;
      perf_attr.exclude_kernel = 1;
      perf_attr.read_format = PERF_FORMAT_GROUP;

      int fd = syscall(SYS_perf_event_open, &perf_attr, 0, -1, bench_linux_state->fds[0], 0);
      if(fd == -1)
      {
	result = 0;
	break;
      }
      else
      {
	bench_linux_state->fds[counter_idx] = fd;
      }
    }
  }

  if(result)
  {
    // NOTE: enable counters
    ioctl(bench_linux_state->fds[0], PERF_EVENT_IOC_ENABLE, PERF_IOC_FLAG_GROUP);

    // NOTE:
    bench_linux_state->counter_buf = arena_push_array(bench_state->arena, U64, bench_state->counter_count + 1);
  }

  return(result);
}

proc Bench_Result
bench_run_proc__linux(Bench_Proc *p, void *args)
{
  int group_fd = bench_linux_state->fds[0];
  ioctl(group_fd, PERF_EVENT_IOC_RESET, PERF_IOC_FLAG_GROUP);
  ioctl(group_fd, PERF_EVENT_IOC_ENABLE, PERF_IOC_FLAG_GROUP);
  p(args);
  ioctl(group_fd, PERF_EVENT_IOC_DISABLE, PERF_IOC_FLAG_GROUP);

  Bench_Result result = {0};
  U64 counter_count = bench_state->counter_count;
  U64 *counter_buf = bench_linux_state->counter_buf;
  int read_result = read(group_fd, counter_buf, (counter_count+1)*sizeof(*counter_buf));
  if(read_result != -1)
  {
    if(counter_buf[0] == counter_count)
    {
      result.counters = counter_buf + 1;
      result.counters_count = counter_count;
    }
  }

  return(result);
}
