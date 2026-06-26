#define __USE_GNU
#include "base.os.posix.h"

#define POSIX_OS_READ_MAX_BYTES  (0x7FFFF000ULL)
#define POSIX_OS_WRITE_MAX_BYTES POSIX_OS_READ_MAX_BYTES

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
