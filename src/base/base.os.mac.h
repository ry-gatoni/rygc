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
  MacClass_NSWindowDelegate,
  MacClass_NSView,
  MacClass_NSEvent,
  MacClass_NSDate,
  MacClass_NSString,
  MacClass_CALayer,
  MacClass_Count,
} MacClass;

typedef enum MacSelector
{
  // universal
  MacSelector_alloc,
  MacSelector_release,
  MacSelector_init,

  // NSApplication
  MacSelector_sharedApplication,
  MacSelector_setActivationPolicy,
  MacSelector_activate,
  MacSelector_finishLaunching,
  MacSelector_nextEventMatchingMask,
  MacSelector_sendEvent,
  MacSelector_updateWindows,

  // NSWindow
  MacSelector_initWithContentRect,
  MacSelector_contentView,
  MacSelector_setContentView,
  MacSelector_makeKeyAndOrderFront,
  MacSelector_setIsVisible,

  // NSWindowDelegate
  MacSelector_windowShouldClose,

  // NSView
  MacSelector_initWithFrame,
  MacSelector_bounds,
  MacSelector_wantsLayer,
  MacSelector_setWantsLayer,
  MacSelector_layer,
  MacSelector_setLayer,

  // NSEvent
  MacSelector_type,
  MacSelector_subtype,
  MacSelector_locationInWindow,
  MacSelector_timestamp,
  MacSelector_window,
  MacSelector_windowNumber,
  MacSelector_modifierFlags,
  MacSelector_keyCode,
  MacSelector_keyRepeatDelay,
  MacSelector_keyRepeatInterval,
  MacSelector_pressedMouseButtons,
  MacSelector_doubleClickInterval,
  MacSelector_mouseLocation,
  MacSelector_buttonNumber,
  MacSelector_clickCount,
  MacSelector_deltaX,
  MacSelector_deltaY,
  MacSelector_deltaZ,
  MacSelector_hasPreciseScrollingDeltas,
  MacSelector_scrollingDeltaX,
  MacSelector_scrollingDeltaY,
  MacSelector_momentumPhase,
  MacSelector_isDirectionInvertedFromDevice,

  // NSString
  MacSelector_stringWithUTF8String,

  // CALayer
  MacSelector_contents,
  MacSelector_setContents,

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
