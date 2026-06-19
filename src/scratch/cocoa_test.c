#include <base/base.h>

#include <base/base.c>

// -----------------------------------------------------------------------------
// CoreGraphics types

// NOTE: taken from CoreGraphics header CGBase.h
#if defined(__LP64__) && __LP64__
typedef double CGFloat;
#else
typedef float CGFloat;
#endif

typedef struct CGPoint
{
  CGFloat x;
  CGFloat y;
} CGPoint;

typedef struct CGSize
{
  CGFloat width;
  CGFloat height;
} CGSize;

typedef struct CGRect
{
  CGPoint origin;
  CGSize size;
} CGRect;

// -----------------------------------------------------------------------------
// AppKit types

typedef void NSApplication;
typedef void NSWindow;
typedef void NSView;
typedef void NSEvent;
typedef void NSString;
typedef void NSDate;

typedef NSString *NSRunLoopMode;
typedef double   NSTimeInterval;

typedef CGPoint NSPoint;
typedef CGSize  NSSize;
typedef CGRect  NSRect;

#define NS_Enum(name, type)\
  typedef type name;\
  enum

NS_Enum(NSApplicationActivationPolicy, NSInteger)
{
  NSApplicationActivationPolicyRegular,
  NSApplicationActivationPolicyAccessory,
  NSApplicationActivationPolicyProhibited,
};

// TODO: API_AVAILABLE macros
NS_Enum(NSWindowStyleMask, NSUInteger)
{
  NSWindowStyleMaskBorderless             = 0,
  NSWindowStyleMaskTitled                 = 1 <<  0,
  NSWindowStyleMaskClosable               = 1 <<  1,
  NSWindowStyleMaskMiniaturizable         = 1 <<  2,
  NSWindowStyleMaskResizable              = 1 <<  3,
  NSWindowStyleMaskTexturedBackground     = 1 <<  8, /* NOTE: deprecated */
  NSWindowStyleMaskUnifiedTitleAndToolbar = 1 << 12,
  NSWindowStyleMaskFullScreen             = 1 << 14,
  NSWindowStyleMaskFullSizeContentView    = 1 << 15,
  NSWindowStyleMaskUtilityWindow          = 1 <<  4,
  NSWindowStyleMaskDocModalWindow         = 1 <<  6,
  NSWindowStyleMaskNonactivatingPanel     = 1 <<  7,
  NSWindowStyleMaskHUDWindow              = 1 << 13,
};

NS_Enum(NSBackingStoreType, NSUInteger)
{
  NSBackingStoreRetained, 	 /* NOTE: deprecated */
  NSBackingStoreNonretained,     /* NOTE: deprecated */
  NSBackingStoreBuffered,
};

// TODO: generate strings for each event type
NS_Enum(NSEventType, NSUInteger)
{
  NSEventTypeNone                       =  0,

  NSEventTypeLeftMouseDown              =  1,
  NSEventTypeLeftMouseUp                =  2,

  NSEventTypeRightMouseDown             =  3,
  NSEventTypeRightMouseUp               =  4,

  NSEventTypeMouseMoved                 =  5,

  NSEventTypeLeftMouseDragged           =  6,
  NSEventTypeRightMouseDragged          =  7,

  NSEventTypeMouseEntered               =  8,
  NSEventTypeMouseExited                =  9,

  NSEventTypeKeyDown                    = 10,
  NSEventTypeKeyUp                      = 11,

  NSEventTypeFlagsChanged               = 12,

  NSEventTypeAppKitDefined              = 13,
  NSEventTypeSystemDefined              = 14,
  NSEventTypeApplicationDefined         = 15,

  NSEventTypePeriodic                   = 16,

  NSEventTypeCursorUpdate               = 17,

  NSEventTypeScrollWheel                = 22,

  NSEventTypeTabletPoint                = 23,
  NSEventTypeTabletProximity            = 24,

  NSEventTypeOtherMouseDown             = 25,
  NSEventTypeOtherMouseUp               = 26,
  NSEventTypeOtherMouseDragged          = 27,
};

NS_Enum(NSEventMask, U64)
{
  NSEventMaskAny                        = U64_MAX,
  NSEventMaskLeftMouseDown              = 1 << NSEventTypeLeftMouseDown,
  NSEventMaskLeftMouseDragged           = 1 << NSEventTypeLeftMouseDragged,
  NSEventMaskLeftMouseUp                = 1 << NSEventTypeLeftMouseUp,

  NSEventMaskRightMouseDown             = 1 << NSEventTypeRightMouseDown,
  NSEventMaskRightMouseDragged          = 1 << NSEventTypeRightMouseDragged,
  NSEventMaskRightMouseUp               = 1 << NSEventTypeRightMouseUp,

  NSEventMaskOtherMouseDown             = 1 << NSEventTypeOtherMouseDown,
  NSEventMaskOtherMouseDragged          = 1 << NSEventTypeOtherMouseDragged,
  NSEventMaskOtherMouseUp               = 1 << NSEventTypeOtherMouseUp,

  NSEventMaskMouseEntered               = 1 << NSEventTypeMouseEntered,
  NSEventMaskMouseMoved                 = 1 << NSEventTypeMouseMoved,
  NSEventMaskMouseExited                = 1 << NSEventTypeMouseExited,

  NSEventMaskKeyDown                    = 1 << NSEventTypeKeyDown,
  NSEventMaskKeyUp                      = 1 << NSEventTypeKeyUp,

  NSEventMaskScrollWheel                = 1 << NSEventTypeScrollWheel,

  NSEventMaskFlagsChanged               = 1 << NSEventTypeFlagsChanged,

  NSEventMaskAppKitDefined              = 1 << NSEventTypeAppKitDefined,
  NSEventMaskSystemDefined              = 1 << NSEventTypeSystemDefined,
  NSEventMaskApplicationDefined         = 1 << NSEventTypeApplicationDefined,

  NSEventMaskPeriodic                   = 1 << NSEventTypePeriodic,

  NSEventMaskCursorUpdate               = 1 << NSEventTypeCursorUpdate,

  NSEventMaskTabletPoint                = 1 << NSEventTypeTabletPoint,
  NSEventMaskTabletProximity            = 1 << NSEventTypeTabletProximity,
};

NS_Enum(NSEventSubtype, short)
{
  // NSEventTypeAppKitDefined
  NSEventSubtypeWindowExposed = 0,
  NSEventSubtypeApplicationActivated = 1,
  NSEventSubtypeApplicationDeactivated = 2,
  NSEventSubtypeWindowMoved = 4,
  NSEventSubtypeScreenChanged = 8,

  // NSEventTypeSystemDefined
  NSEventSubtypePowerOff = 1,

  // mouse events
  NSEventSubtypeMouseEvent = 0,
  NSEventSubtypeTabletPoint = 1,
  NSEventSubtypeTabletProximity = 2,
  NSEventSubtypeTouch = 3,
};

NS_Enum(NSEventModifierFlags, NSUInteger)
{
  NSEventModifierFlagCapsLock                   = 1 << 16,
  NSEventModifierFlagShift                      = 1 << 17,
  NSEventModifierFlagControl                    = 1 << 18,
  NSEventModifierFlagOption                     = 1 << 19,
  NSEventModifierFlagCommand                    = 1 << 20,
  NSEventModifierFlagNumericPad                 = 1 << 21,
  NSEventModifierFlagHelp                       = 1 << 22,
  NSEventModifierFlagFunction                   = 1 << 23,
  NSEventModifierFlagDeviceIndependentFlagsMask = 0xFFFF0000UL,
};

NS_Enum(NSEventPhase, NSUInteger)
{
  NSEventPhaseNone              = 0,
  NSEventPhaseBegan             = 1 << 0,
  NSEventPhaseStationary        = 1 << 1,
  NSEventPhaseChanged           = 1 << 2,
  NSEventPhaseEnded             = 1 << 3,
  NSEventPhaseCancelled         = 1 << 4,
  NSEventPhaseMayBegin          = 1 << 5,
};

// TODO: how to generate all this?
// TODO: make sure floating point returns are handled correctly for each abi
// -----------------------------------------------------------------------------
// functions

#define objc_add_method(class, method, impl, sigstr)\
  class_addMethod(mac_state->classes[Glue(MacClass_, class)], mac_state->sels[Glue(MacSelector_, method)], (IMP)impl, sigstr)

proc inline id
NSAlloc(Class nsclass)
{
  id nsid = (id)nsclass;
  SEL nssel = mac_state->sels[MacSelector_alloc];
  return ((id (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

// -----------------------------------------------------------------------------
// NSApplication

proc inline NSApplication*
NSApplication_sharedApplication(void)
{
  id nsid = (id)mac_state->classes[MacClass_NSApplication];
  SEL nssel = mac_state->sels[MacSelector_sharedApplication];
  return ((id (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline BOOL
NSApplication_setActivationPolicy(NSApplication *app, NSApplicationActivationPolicy activation_policy)
{
  id nsid = app;
  SEL nssel = mac_state->sels[MacSelector_setActivationPolicy];
  return ((BOOL (*)(id, SEL, NSInteger))objc_msgSend)(nsid, nssel, activation_policy);
}

proc inline void
NSApplication_activate(NSApplication *app)
{
  id nsid = app;
  SEL nssel = mac_state->sels[MacSelector_activate];
  return ((void (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline void
NSApplication_finishLaunching(NSApplication *app)
{
  id nsid = app;
  SEL nssel = mac_state->sels[MacSelector_finishLaunching];
  return ((void (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline NSEvent*
NSApplication_nextEventMatchingMask(NSApplication *app, NSEventMask mask, NSDate *expiration, NSRunLoopMode mode, BOOL dequeue)
{
  id nsid = app;
  SEL nssel = mac_state->sels[MacSelector_nextEventMatchingMask];
  return ((id (*)(id, SEL, U64, id, id, BOOL))objc_msgSend)(nsid, nssel, mask, expiration, mode, dequeue);
}

proc inline void
NSApplication_sendEvent(NSApplication *app, NSEvent *event)
{
  id nsid = app;
  SEL nssel = mac_state->sels[MacSelector_sendEvent];
  return ((void (*)(id, SEL, id))objc_msgSend)(nsid, nssel, event);
}

proc inline void
NSApplication_updateWindows(NSApplication *app)
{
  id nsid = app;
  SEL nssel = mac_state->sels[MacSelector_updateWindows];
  return ((void (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

// -----------------------------------------------------------------------------
// NSWindow

proc inline NSWindow*
NSWindow_initWithContentRect(NSRect content_rect, NSWindowStyleMask style_mask, NSBackingStoreType backing, BOOL defer)
{
  Class nsclass = mac_state->classes[MacClass_NSWindow];
  id nsid = NSAlloc(nsclass);
  SEL nssel = mac_state->sels[MacSelector_initWithContentRect];
  return ((id (*)(id, SEL, NSRect, NSUInteger, NSUInteger, BOOL))objc_msgSend)(nsid, nssel, content_rect, style_mask, backing, defer);
}

proc inline NSView*
NSWindow_contentView(NSWindow *window)
{
  id nsid = window;
  SEL nssel = mac_state->sels[MacSelector_contentView];
  return ((id (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline void
NSWindow_makeKeyAndOrderFront(NSWindow *window, id sender)
{
  id nsid = window;
  SEL nssel = mac_state->sels[MacSelector_makeKeyAndOrderFront];
  return ((void (*)(id, SEL, id))objc_msgSend)(nsid, nssel, sender);
}

proc inline void
NSWindow_setIsVisible(NSWindow *window, BOOL flag)
{
  id nsid = window;
  SEL nssel = mac_state->sels[MacSelector_setIsVisible];
  return ((void (*)(id, SEL, BOOL))objc_msgSend)(nsid, nssel, flag);
}

// -----------------------------------------------------------------------------
// NSEvent

proc inline NSEventType
NSEvent_type(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_type];
  return ((NSUInteger (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline NSEventSubtype
NSEvent_subtype(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_subtype];
  return ((short (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline NSPoint
NSEvent_locationInWindow(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_locationInWindow];
  return ((NSPoint (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline NSTimeInterval
NSEvent_timestamp(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_timestamp];
  return ((double (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline NSWindow*
NSEvent_window(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_window];
  return ((id (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline NSInteger
NSEvent_windowNumber(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_windowNumber];
  return ((NSInteger (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

// TODO: event ref

proc inline NSEventModifierFlags
NSEvent_modifierFlags(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_modifierFlags];
  return ((NSUInteger (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

// TODO: global modifier flags?

// TODO: characters, or characters igoring modifiers?

proc inline U16
NSEvent_keyCode(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_keyCode];
  return ((U16 (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline NSTimeInterval
NSEvent_keyRepeatDelay(void)
{
  Class nsclass = mac_state->classes[MacClass_NSEvent];
  id nsid = (id)nsclass;
  SEL nssel = mac_state->sels[MacSelector_keyRepeatDelay];
  return ((double (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline NSTimeInterval
NSEvent_keyRepeatInterval(void)
{
  Class nsclass = mac_state->classes[MacClass_NSEvent];
  id nsid = (id)nsclass;
  SEL nssel = mac_state->sels[MacSelector_keyRepeatInterval];
  return ((double (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline NSUInteger
NSEvent_pressedMouseButtons(void)
{
  Class nsclass = mac_state->classes[MacClass_NSEvent];
  id nsid = (id)nsclass;
  SEL nssel = mac_state->sels[MacSelector_pressedMouseButtons];
  return ((NSUInteger (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline NSTimeInterval
NSEvent_doubleClickInterval(void)
{
  Class nsclass = mac_state->classes[MacClass_NSEvent];
  id nsid = (id)nsclass;
  SEL nssel = mac_state->sels[MacSelector_doubleClickInterval];
  return ((double (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline NSPoint
NSEvent_mouseLocation(void)
{
  Class nsclass = mac_state->classes[MacClass_NSEvent];
  id nsid = (id)nsclass;
  SEL nssel = mac_state->sels[MacSelector_mouseLocation];
  return ((NSPoint (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline NSInteger
NSEvent_buttonNumber(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_buttonNumber];
  return ((NSInteger (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline NSInteger
NSEvent_clickCount(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_clickCount];
  return ((NSInteger (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline CGFloat
NSEvent_deltaX(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_deltaX];
  return ((CGFloat (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline CGFloat
NSEvent_deltaY(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_deltaY];
  return ((CGFloat (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline CGFloat
NSEvent_deltaZ(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_deltaZ];
  return ((CGFloat (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline BOOL
NSEvent_hasPreciseScrollingDeltas(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_hasPreciseScrollingDeltas];
  return ((BOOL (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline CGFloat
NSEvent_scrollingDeltaX(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_scrollingDeltaX];
  return ((CGFloat (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline CGFloat
NSEvent_scrollingDeltaY(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_scrollingDeltaY];
  return ((CGFloat (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline NSEventPhase
NSEvent_momentumPhase(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_momentumPhase];
  return ((NSUInteger (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline BOOL
NSEvent_isDirectionInvertedFromDevice(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_isDirectionInvertedFromDevice];
  return ((BOOL (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

// -----------------------------------------------------------------------------
// NSString

proc inline NSString*
NSString_stringWithUTF8String(const char *cstr)
{
  Class nsclass = mac_state->classes[MacClass_NSString];
  id nsid = (id)nsclass;
  SEL nssel = mac_state->sels[MacSelector_stringWithUTF8String];
  return ((id (*)(id, SEL, const char*))objc_msgSend)(nsid, nssel, cstr);
}

// -----------------------------------------------------------------------------
// main

global B32 running = 1;

proc B32
on_window_close(void *self)
{
  Unused(self);
  running = 0;
  return(1);
}

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  int result = 0;

  if(!os_init())
  { result = 1; goto end; }

  NSApplication *app = 0;
  if((app = NSApplication_sharedApplication()) == 0)
  { result = 1; goto end; }
  if(!NSApplication_setActivationPolicy(app, NSApplicationActivationPolicyRegular))
  { result = 1; goto end; }

  if(!objc_add_method(NSObject, windowShouldClose, on_window_close, 0))
  { result = 1; goto end; }

  NSWindow *window = 0;
  NSRect window_rect = {{200, 200}, {200, 200}};
  NSWindowStyleMask window_style_mask = (NSWindowStyleMaskClosable|
					 NSWindowStyleMaskMiniaturizable|
					 NSWindowStyleMaskTitled|
					 NSWindowStyleMaskResizable);
  NSBackingStoreType window_backing = NSBackingStoreBuffered;
  if((window = NSWindow_initWithContentRect(window_rect, window_style_mask, window_backing, false)) == 0)
  { result = 1; goto end; }

  NSApplication_activate(app);
  NSWindow_makeKeyAndOrderFront(window, 0);
  NSWindow_setIsVisible(window, true);

  NSApplication_finishLaunching(app);

  // TODO: more event handling
  while(running)
  {
    NSEventMask mask = NSEventMaskAny;
    NSRunLoopMode mode = NSString_stringWithUTF8String("kCFRunLoopDefaultMode");
    NSEvent *e = NSApplication_nextEventMatchingMask(app, mask, 0, mode, true);
    NSEventType event_type = NSEvent_type(e);
    //NSEventSubtype event_subtype = NSEvent_subtype(e);
    NSPoint event_pos = NSEvent_locationInWindow(e);
    NSEventModifierFlags event_mods = NSEvent_modifierFlags(e);
    if(event_type)
    {
      printf("event\n"
	     "type: %lu\n"
	     //"subtype: %hd\n"
	     "pos: (%.2f, %.2f)\n"
	     "mods: %lu\n",
	     event_type,
	     //event_subtype,
	     event_pos.x, event_pos.y,
	     event_mods);
    }

    NSApplication_sendEvent(app, e);
    NSApplication_updateWindows(app);
  }

end:
  // TODO: proper cleanup
  return(result);
}
