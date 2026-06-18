#include <x86intrin.h>

proc U64
cpu_counter_fixed(void)
{
  U64 result = __rdtsc();
  return(result);
}

// TODO: is there a better way of doing this?
proc U64
cpu_counter_fixed_freq(void)
{
  U64 wait_ms = 100; // TODO: make configurable
  U64 os_freq = os_counter_freq();
  U64 os_wait = os_freq * wait_ms / 1000;

  U64 os_elapsed = 0;
  U64 cpu_start = cpu_counter_fixed();
  U64 os_start = os_counter();
  while(os_elapsed < os_wait)
  {
    os_elapsed = os_counter() - os_start;
  }
  U64 cpu_elapsed = cpu_counter_fixed() - cpu_start;

  U64 result = os_freq * cpu_elapsed / os_elapsed;
  return(result);
}
