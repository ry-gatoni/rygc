proc U64 cpu_get_cycle_count_fixed(void);
proc U64 cpu_get_cycle_counter_freq(void);
proc U64 cpu_get_cycle_count(void);

#if OS_LINUX || OS_MAC
#  define AtomicCompareAndSwap(dest, old_val, new_val) ({	\
    U32 temp = old_val;							\
    __atomic_compare_exchange_n((volatile U32*)(dest), &temp, new_val, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST); \
    })
#elif OS_WINDOWS
#  define AtomicCompareAndSwap(dest, old_val, new_val) \
  InterlockedCompareExchange((volatile LONG*)dest, new_val, old_val)
#else
#  error ERROR: unsupported os
#endif

#if COMPILER_CLANG || COMPILER_GCC
#  define MSB(n) (8*sizeof(void*)-1 - __builtin_clz((n)|1))
#  define LSB(n) (__builtin_ctz((n)|1) + 1)
#elif COMPILER_MSVC
#  include <intrin.h>
static inline U32
msb_helper(U64 num)
{
  U32 index = 0;
  _BitScanReverse64(&index, num | 1);
  return(index);
}
static inline U32
lsb_helper(U64 num)
{
  U32 index = 0;
  _BitScanForward64(&index, num | 1);
  return(index);
}
#  define MSB(n) msb_helper(n)
#  define LSB(n) lsb_helper(n)
#else
#  error compiler not implemented
#endif
