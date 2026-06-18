#define __USE_GNU
#include "base.os.posix.h"

#if CPU_X86 || CPU_X64
#  include <x86intrin.h>
#endif

#define POSIX_OS_READ_MAX_BYTES  (0x7FFFF000ULL)
#define POSIX_OS_WRITE_MAX_BYTES LINUX_MAX_BYTES_TO_READ

typedef struct Linux_State
{
  Arena *arena;

  U64 cpu_counter_freq;

  U64 page_size;
} Linux_State;

global Linux_State *linux_state = 0;

// -----------------------------------------------------------------------------
// initialization

proc B32 linux_init(void);
