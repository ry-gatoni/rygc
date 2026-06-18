proc U64
cpu_counter_fixed(void)
{
  U64 result;
#if COMPILER_GCC || COMPILER_CLANG
  __asm__ __volatile__("mrs %0, cntvct_el0" : "=r"(result));
#elif COMPILER_MSVC
  result = _ReadStatusReg(ARM64_CNTVCT_EL0);
#else
#  error cpu_counter_fixed() not implemented for this compiler
#endif
  return(result);
}

proc U64
cpu_counter_fixed_freq(void)
{
  U64 result;
#if COMPILER_GCC || COMPILER_CLANG
  __asm__ __volatile__("mrs %0, cntfrq_el0" : "=r"(result));
#elif COMPILER_MSVC
  result = _ReadStatusReg(ARM64_CNTFRQ_EL0);
#else
#  error cpu_counter_fixed_freq() not implemented for this compiler
#endif
  return(result);
}
