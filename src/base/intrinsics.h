proc U64 cpu_get_cycle_count_fixed(void);
proc U64 cpu_get_cycle_counter_freq(void);
proc U64 cpu_get_cycle_count(void);

#if OS_LINUX || OS_MAC
#  define AtomicCompareAndSwap(dest, old_val, new_val) ({	\
    U32 temp = old_val;							\
    __atomic_compare_exchange_n((volatile U32*)(dest), &temp, new_val, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST); \
    })
#elif OS_WINDOWS
#  error TODO: implement
#else
#  error ERROR: unsupported os
#endif
