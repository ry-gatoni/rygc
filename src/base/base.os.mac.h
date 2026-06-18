#include "base.os.posix.h"

#include <objc/runtime.h>
#include <objc/message.h>

#define POSIX_OS_READ_MAX_BYTES  (S32_MAX)
#define POSIX_OS_WRITE_MAX_BYTES POSIX_OS_READ_MAX_BYTES

typedef unsigned long NSUInteger;
typedef long NSInteger;

typedef struct Mac_State
{
  Arena *arena;

  id *class_ids;
  U64 class_id_count;

  U64 cpu_counter_freq;

  U64 page_size;
} Mac_State;

global Mac_State *mac_state = 0;

// -----------------------------------------------------------------------------
// initialization

proc B32 mac_init(void);
