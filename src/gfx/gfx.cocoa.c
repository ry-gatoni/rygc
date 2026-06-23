// -----------------------------------------------------------------------------
// init

proc B32
gfx_cocoa_init(void)
{
  NSApplication *app = 0;

  if((app = NSApplication_sharedApplication()) == 0)
  { goto gfx_cocoa_init_failure; }
  if(!NSApplication_setActivationPolicy(app, NSApplicationActivationPolicyRegular))
  { goto gfx_cocoa_init_failure; }

  NSApplication_activate(app);
  NSApplication_finishLaunching(app);

  Arena *arena = arena_alloc();
  cocoa_state = arena_push_struct(arena, Cocoa_State);
  cocoa_state->app = app;
  return(1);

gfx_cocoa_init_failure:
  if(app) NSApplication_terminate(app, 0);
  return(0);
}

proc void
gfx_cocoa_uninit(void)
{
  NSApplication_terminate(cocoa_state->app, 0);
  arena_release(cocoa_state->arena);
  cocoa_state = 0;
}

// -----------------------------------------------------------------------------
// window

proc Cocoa_Window*
gfx_cocoa_window_open(V2S32 dim, String8 title)
{
  NSWindow *ns_window = 0;
  NSView *view = 0;
  CALayer *layer = 0;

  // TODO: make windows open at different positions
  NSRect ns_window_rect = CGRectMake(200, 200, dim.width, dim.height);
  NSWindowStyleMask ns_window_style_mask = (NSWindowStyleMaskClosable|
					    NSWindowStyleMaskMiniaturizable|
					    NSWindowStyleMaskTitled|
					    NSWindowStyleMaskResizable);
  NSBackingStoreType ns_window_backing = NSBackingStoreBuffered;
  if(!(ns_window = NSWindow_initWithContentRect(ns_window_rect, ns_window_style_mask, ns_window_backing, false)))
  { goto gfx_cocoa_open_window_failure; }

  NSWindow_makeKeyAndOrderFront(ns_window, 0);
  NSWindow_setIsVisible(ns_window, true);

  NSString *ns_title = NSString_initWithBytesLength((const char*)title.string, title.count, NSUTF8StringEncoding);
  NSWindow_setTitle(ns_window, ns_title);

  view = NSWindow_contentView(ns_window);
  NSView_setWantsLayer(view, true);

  layer = NSView_layer(view);
  CALayer_setContentsGravity(layer, kCAGravityBottomLeft);

  // TODO: alloc buffers

  Cocoa_Window *window = gfx__cocoa_window_alloc();
  window->window = ns_window;
  window->view = view;
  window->layer = layer;
  return(window);

gfx_cocoa_open_window_failure:
  if(ns_window) NSWindow_close(ns_window);
  return(0);
}

proc void
gfx_cocoa_window_close(Cocoa_Window *win)
{
  NSWindow_close(win->window);
  gfx__cocoa_window_release(win);
}

// -----------------------------------------------------------------------------
// events

// TODO:

// -----------------------------------------------------------------------------
// render

// TODO:

// -----------------------------------------------------------------------------
// helpers

proc Cocoa_Window*
gfx__cocoa_window_alloc(void)
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

proc void
gfx__cocoa_window_release(Cocoa_Window *win)
{
  DLLRemove(cocoa_state->first_window, cocoa_state->last_window, win);
  --cocoa_state->window_count;

  ZeroStruct(win);
  SLLStackPush(cocoa_state->window_freelist, win);
}
