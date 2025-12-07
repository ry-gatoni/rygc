/* proc U64 cpu_counter_fixed(void); */
/* proc U64 cpu_counter_fixed_freq(void); */
proc U64 cpu_counter(void);
proc U64 cpu_counter_freq(void);

#if OS_LINUX || OS_MAC
#  define AtomicCompareAndSwap(dest, old_val, new_val) ({       \
    U32 temp = old_val;                                                 \
    __atomic_compare_exchange_n((volatile U32*)(dest), &temp, new_val, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST); \
    })
#elif OS_WINDOWS
#  define AtomicCompareAndSwap(dest, old_val, new_val) \
  InterlockedCompareExchange((volatile LONG*)dest, new_val, old_val)
#else
#  error ERROR: atomic operations not implemented for this OS
#endif

#if CPU_X64 || CPU_X86
#  define CpuSpinWait() _mm_pause()
#elif CPU_ARM64 || CPU_ARM
#  define CpuSpinWait() __yield()
#else
#  error ERROR: CpuSpinWait not implemented for this architecture
#endif

#define TakeLock_01(lock, zero, one) while(AtomicCompareAndSwap(lock, zero, one) != (zero)) { CpuSpinWait(); }
#define ReleaseLock_01(lock, zero, one) while(AtomicCompareAndSwap(lock, one, zero) != (one)) { CpuSpinWait(); }
#define TakeLock(lock) TakeLock_01(lock, 0, 1)
#define ReleaseLock(lock) ReleaseLock_01(lock, 0, 1)

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
#  error ERROR: bit operations not implemented for this compiler
#endif
