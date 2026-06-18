#include "base.os.posix.h"

#include <objc/runtime.h>
#include <objc/message.h>

#define POSIX_OS_READ_MAX_BYTES  (S32_MAX)
#define POSIX_OS_WRITE_MAX_BYTES POSIX_OS_READ_MAX_BYTES

typedef unsigned long NSUInteger;
typedef long NSInteger;

// TODO: how to generate all this?
typedef enum MacClass
{
  MacClass_NSObject,
  MacClass_NSApplication,
  MacClass_NSWindow,
  MacClass_NSEvent,
  MacClass_NSDate,
  MacClass_NSString,
  MacClass_Count,
} MacClass;

typedef enum MacSelector
{
  MacSelector_alloc,
  MacSelector_release,
  MacSelector_init,

  MacSelector_NSApplication_sharedApplication,
  MacSelector_NSApplication_setActivationPolicy,
  MacSelector_NSApplication_activate,
  MacSelector_NSApplication_finishLaunching,
  MacSelector_NSApplication_nextEventMatchingMask,
  MacSelector_NSApplication_sendEvent,
  MacSelector_NSApplication_updateWindows,

  MacSelector_NSWindow_initWithContentRect,
  MacSelector_NSWindow_makeKeyAndOrderFront,
  MacSelector_NSWindow_setIsVisible,

  MacSelector_NSString_stringWithUTF8String,

  MacSelector_Count,
} MacSelector;

typedef struct Mac_State
{
  Arena *arena;

  Class *classes;
  U64 class_count;

  SEL *sels;
  U64 sel_count;

  U64 cpu_counter_freq;

  U64 page_size;
} Mac_State;

global Mac_State *mac_state = 0;

// -----------------------------------------------------------------------------
// initialization

proc B32 mac_init(void);
