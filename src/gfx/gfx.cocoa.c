// -----------------------------------------------------------------------------
// init

proc B32
cocoa_init(Arena *arena)
{
  NSApplication *app = 0;

  if((app = NSApplication_sharedApplication()) == 0)
  { goto cocoa_init_failure; }
  if(!NSApplication_setActivationPolicy(app, NSApplicationActivationPolicyRegular))
  { goto cocoa_init_failure; }

  NSApplication_activate(app);
  NSApplication_finishLaunching(app);

  cocoa_state = arena_push_struct(arena, Cocoa_State);
  cocoa_state->arena = arena;
  cocoa_state->app = app;
  return(1);

cocoa_init_failure:
  if(app) NSApplication_terminate(app, 0);
  return(0);
}

proc void
cocoa_uninit(void)
{
  NSApplication_terminate(cocoa_state->app, 0);
  cocoa_state = 0;
}

// -----------------------------------------------------------------------------
// window

global const char *window_association_key = "rygc_cocoa_window";

// NOTE: private function
proc B32
cocoa__on_window_close(id self, SEL cmd, NSWindow *sender)
{
  Unused(self);
  Unused(cmd);
  Cocoa_Window *window = (Cocoa_Window*)objc_getAssociatedObject(sender, window_association_key);
  Gfx_Event *event = gfx__event_new();
  Assert(event);
  event->kind = Gfx_EventKind_close;
  event->window = cocoa__gfx_handle_from_window(window);
  gfx__event_push(event);

  // TODO: close the window
  return(1);
}

proc Cocoa_Window*
cocoa_window_open(V2S32 dim, String8 title)
{
  NSWindow *ns_window = 0;
  NSString *ns_title = 0;
  NSView *view = 0;
  CALayer *layer = 0;
  Cocoa_Window *window = 0;

  // TODO: make windows open at different positions
  NSRect ns_window_rect = CGRectMake(200, 200, dim.width, dim.height);
  NSWindowStyleMask ns_window_style_mask = (NSWindowStyleMaskClosable|
					    NSWindowStyleMaskMiniaturizable|
					    NSWindowStyleMaskTitled|
					    NSWindowStyleMaskResizable);
  NSBackingStoreType ns_window_backing = NSBackingStoreBuffered;
  if(!(ns_window = NSWindow_initWithContentRect(ns_window_rect, ns_window_style_mask, ns_window_backing, false)))
  { goto cocoa_window_open_failure; }

  if(!objc_add_method(NSObject, windowShouldClose, cocoa__on_window_close, "c@:@"))
  { goto cocoa_window_open_failure; }

  NSWindow_makeKeyAndOrderFront(ns_window, 0);
  NSWindow_setIsVisible(ns_window, true);

  // TODO: ensure title.string points to a null-terminated string
  ns_title = NSString_initWithBytesLength((const char*)title.string, title.count, NSUTF8StringEncoding);
  NSWindow_setTitle(ns_window, ns_title);

  view = NSWindow_contentView(ns_window);
  NSView_setWantsLayer(view, true);

  layer = NSView_layer(view);
  CALayer_setContentsGravity(layer, kCAGravityBottomLeft);

  // TODO: alloc buffers

  window = cocoa__window_alloc();
  objc_setAssociatedObject(ns_window, window_association_key, (id)window, OBJC_ASSOCIATION_ASSIGN);
  window->window = ns_window;
  window->view = view;
  window->layer = layer;
  return(window);

cocoa_window_open_failure:
  if(ns_window) NSWindow_close(ns_window);
  return(0);
}

proc void
cocoa_window_close(Cocoa_Window *win)
{
  NSWindow_close(win->window);
  cocoa__window_release(win);
}

proc Gfx_Handle
gfx_window_open(S32 width, S32 height, String8 name)
{
  Cocoa_Window *win = cocoa_window_open(v2s32(width, height), name);
  Gfx_Handle result = cocoa__gfx_handle_from_window(win);
  return result;
}

proc void
gfx_window_close(Gfx_Handle window)
{
  Cocoa_Window *win = cocoa__window_from_gfx_handle(window);
  cocoa_window_close(win);
}

// -----------------------------------------------------------------------------
// events

global Gfx_EventKind gfx_event_kind_from_cocoa_event_type[] = {
  [NSEventTypeMouseMoved] = Gfx_EventKind_move,
  [NSEventTypeLeftMouseDragged] = Gfx_EventKind_move,
  [NSEventTypeRightMouseDragged] = Gfx_EventKind_move,
  [NSEventTypeOtherMouseDragged] = Gfx_EventKind_move,

  [NSEventTypeLeftMouseDown] = Gfx_EventKind_press,
  [NSEventTypeLeftMouseUp] = Gfx_EventKind_release,
  [NSEventTypeRightMouseDown] = Gfx_EventKind_press,
  [NSEventTypeRightMouseUp] = Gfx_EventKind_release,
  [NSEventTypeOtherMouseDown] = Gfx_EventKind_press,
  [NSEventTypeOtherMouseUp] = Gfx_EventKind_release,
  [NSEventTypeKeyDown] = Gfx_EventKind_press,
  [NSEventTypeKeyUp] = Gfx_EventKind_release,

  [NSEventTypeScrollWheel] = Gfx_EventKind_scroll,
};

proc void
cocoa_events(void)
{
  gfx__events_flush();

  NSEvent *e = 0;
  NSEventMask mask = NSEventMaskAny;
  NSRunLoopMode mode = NSDefaultRunLoopMode;
  while((e = NSApplication_nextEventMatchingMask(cocoa_state->app, mask, 0, mode, true)) != 0)
  {
    NSEventType type = NSEvent_type(e);
    switch(type)
    {
      // TODO: handle events that we don't have os kinds for
      default:
      {
	printf("cocoa event with type %lu\n", type);
	Gfx_Event *event = gfx__event_new();
	Assert(event);
	event->kind = gfx_event_kind_from_cocoa_event_type[type];
	gfx__event_push(event);
	//gfx__event_list_push_new(arena, &result, gfx_event_kind_from_cocoa_event_type[type]);
      }break;
    }

    NSApplication_sendEvent(cocoa_state->app, e);
  }

  NSApplication_updateWindows(cocoa_state->app);
}

// -----------------------------------------------------------------------------
// render

// TODO:

// -----------------------------------------------------------------------------
// helpers

proc inline Cocoa_Window*
cocoa__window_alloc(void)
{
  Cocoa_Window *window = cocoa_state->window_freelist;
  if(window)
  {
    SLLStackPop(cocoa_state->window_freelist);
  }
  else
  {
    window = arena_push_struct(cocoa_state->arena, Cocoa_Window);
  }
  Assert(window);

  SLLQueuePush(cocoa_state->first_window, cocoa_state->last_window, window);
  ++cocoa_state->window_count;

  return(window);
}

proc inline void
cocoa__window_release(Cocoa_Window *win)
{
  DLLRemove(cocoa_state->first_window, cocoa_state->last_window, win);
  --cocoa_state->window_count;

  ZeroStruct(win);
  SLLStackPush(cocoa_state->window_freelist, win);
}

proc inline Gfx_Handle
cocoa__gfx_handle_from_window(Cocoa_Window *win)
{
  Gfx_Handle result = {0};
  result.handle = win;
  return result;
}

proc inline Cocoa_Window*
cocoa__window_from_gfx_handle(Gfx_Handle win)
{
  Cocoa_Window *result = win.handle;
  return result;
}
