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

typedef void NSApplication;
typedef void NSWindow;
typedef void NSEvent;
typedef void NSString;
typedef void NSDate;

typedef NSString *NSRunLoopMode;

typedef CGRect NSRect;

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
  NSWindowStyleMaskTitled                 = 1 << 0,
  NSWindowStyleMaskClosable               = 1 << 1,
  NSWindowStyleMaskMiniaturizable         = 1 << 2,
  NSWindowStyleMaskResizable              = 1 << 3,
  NSWindowStyleMaskTexturedBackground     = 1 << 8, /* NOTE: deprecated */
  NSWindowStyleMaskUnifiedTitleAndToolbar = 1 << 12,
  NSWindowStyleMaskFullScreen             = 1 << 14,
  NSWindowStyleMaskFullSizeContentView    = 1 << 15,
  NSWindowStyleMaskUtilityWindow          = 1 << 4,
  NSWindowStyleMaskDocModalWindow         = 1 << 6,
  NSWindowStyleMaskNonactivatingPanel     = 1 << 7,
  NSWindowStyleMaskHUDWindow              = 1 << 13,
};

NS_Enum(NSBackingStoreType, NSUInteger)
{
  NSBackingStoreRetained, 	 /* NOTE: deprecated */
  NSBackingStoreNonretained,     /* NOTE: deprecated */
  NSBackingStoreBuffered,
};

NS_Enum(NSEventMask, U64)
{
  NSEventMaskAny = U64_MAX,
};

// TODO: how to generate all this?
// -----------------------------------------------------------------------------
// functions

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
  SEL nssel = mac_state->sels[MacSelector_NSApplication_sharedApplication];
  return ((id (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline BOOL
NSApplication_setActivationPolicy(NSApplication *app, NSApplicationActivationPolicy activation_policy)
{
  id nsid = app;
  SEL nssel = mac_state->sels[MacSelector_NSApplication_setActivationPolicy];
  return ((BOOL (*)(id, SEL, NSInteger))objc_msgSend)(nsid, nssel, activation_policy);
}

proc inline void
NSApplication_activate(NSApplication *app)
{
  id nsid = app;
  SEL nssel = mac_state->sels[MacSelector_NSApplication_activate];
  return ((void (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline void
NSApplication_finishLaunching(NSApplication *app)
{
  id nsid = app;
  SEL nssel = mac_state->sels[MacSelector_NSApplication_finishLaunching];
  return ((void (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline NSEvent*
NSApplication_nextEventMatchingMask(NSApplication *app, NSEventMask mask, NSDate *expiration, NSRunLoopMode mode, BOOL dequeue)
{
  id nsid = app;
  SEL nssel = mac_state->sels[MacSelector_NSApplication_nextEventMatchingMask];
  return ((id (*)(id, SEL, U64, id, id, BOOL))objc_msgSend)(nsid, nssel, mask, expiration, mode, dequeue);
}

proc inline void
NSApplication_sendEvent(NSApplication *app, NSEvent *event)
{
  id nsid = app;
  SEL nssel = mac_state->sels[MacSelector_NSApplication_sendEvent];
  return ((void (*)(id, SEL, id))objc_msgSend)(nsid, nssel, event);
}

proc inline void
NSApplication_updateWindows(NSApplication *app)
{
  id nsid = app;
  SEL nssel = mac_state->sels[MacSelector_NSApplication_updateWindows];
  return ((void (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

// -----------------------------------------------------------------------------
// NSWindow

proc inline NSWindow*
NSWindow_initWithContentRect(NSRect content_rect, NSWindowStyleMask style_mask, NSBackingStoreType backing, BOOL defer)
{
  Class nsclass = mac_state->classes[MacClass_NSWindow];
  id nsid = NSAlloc(nsclass);
  SEL nssel = mac_state->sels[MacSelector_NSWindow_initWithContentRect];
  return ((id (*)(id, SEL, NSRect, NSUInteger, NSUInteger, BOOL))objc_msgSend)(nsid, nssel, content_rect, style_mask, backing, defer);
}

proc inline void
NSWindow_makeKeyAndOrderFront(NSWindow *window, id sender)
{
  id nsid = window;
  SEL nssel = mac_state->sels[MacSelector_NSWindow_makeKeyAndOrderFront];
  return ((void (*)(id, SEL, id))objc_msgSend)(nsid, nssel, sender);
}

proc inline void
NSWindow_setIsVisible(NSWindow *window, BOOL flag)
{
  id nsid = window;
  SEL nssel = mac_state->sels[MacSelector_NSWindow_setIsVisible];
  return ((void (*)(id, SEL, BOOL))objc_msgSend)(nsid, nssel, flag);
}

// -----------------------------------------------------------------------------
// NSString

proc inline NSString*
NSString_stringWithUTF8String(const char *cstr)
{
  Class nsclass = mac_state->classes[MacClass_NSString];
  id nsid = (id)nsclass;
  SEL nssel = mac_state->sels[MacSelector_NSString_stringWithUTF8String];
  return ((id (*)(id, SEL, const char*))objc_msgSend)(nsid, nssel, cstr);
}

// -----------------------------------------------------------------------------
// main

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

  // TODO: handle window close
  B32 running = 1;
  while(running)
  {
    NSEventMask mask = NSEventMaskAny;
    NSRunLoopMode mode = NSString_stringWithUTF8String("kCFRunLoopDefaultMode");
    NSEvent *e = NSApplication_nextEventMatchingMask(app, mask, 0, mode, true);

    NSApplication_sendEvent(app, 0);
    NSApplication_updateWindows(app);
  }

end:
  return(result);
}
