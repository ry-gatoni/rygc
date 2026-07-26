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

  // NOTE: get screen dim
  NSScreen *screen;
  S32 screen_width, screen_height;
  {
    screen = NSScreen_mainScreen();
    NSRect screen_rect = NSScreen_frame(screen);
    screen_width = (S32)screen_rect.size.width;
    screen_height = (S32)screen_rect.size.height;
  }

  // NOTE: pixel buffer allocator
  CFDictionaryRef pbuf_attr;
  {
    CFAllocatorRef pbuf_allocator = kCFAllocatorDefault;
    CFStringRef iosurface_key = kCVPixelBufferIOSurfacePropertiesKey;
    CFDictionaryRef iosurface_val = CFDictionaryCreate(pbuf_allocator, 0, 0, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    const void *pbuf_attr_keys[] = { iosurface_key, };
    const void *pbuf_attr_vals[] = { iosurface_val, };
    pbuf_attr = CFDictionaryCreate(pbuf_allocator, pbuf_attr_keys, pbuf_attr_vals, 1, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    CFRelease(iosurface_val);
  }

  NSApplication_activate(app);
  NSApplication_finishLaunching(app);

  cocoa_state = arena_push_struct(arena, Cocoa_State);
  cocoa_state->arena = arena;
  cocoa_state->app = app;
  cocoa_state->screen = screen;
  cocoa_state->screen_width = screen_width;
  cocoa_state->screen_height = screen_height;
  cocoa_state->pbuf_attr = pbuf_attr;
  return(1);

cocoa_init_failure:
  if(app) NSApplication_terminate(app, 0);
  return(0);
}

proc void
cocoa_uninit(void)
{
  CFRelease(cocoa_state->pbuf_attr);
  NSApplication_terminate(cocoa_state->app, 0);
  cocoa_state = 0;
}

// -----------------------------------------------------------------------------
// window

// NOTE: private function
proc B32
cocoa__on_window_close(id self, SEL cmd, NSWindow *sender)
{
  Unused(self);
  Unused(cmd);

  Cocoa_Window *window = cocoa__window_from_ns_window(sender);
  Gfx_Event *event = gfx__event_new();
  Assert(event);
  event->kind = Gfx_EventKind_close;
  event->window = cocoa__gfx_handle_from_window(window);
  gfx__event_push(event);

  return(1);
}

// NOTE: private function
proc void
cocoa__on_key_down(id self, SEL cmd, NSEvent *event)
{
  Unused(self);
  Unused(cmd);
  Unused(event);
  return;
}

global const char *display_link_handler_selector_name = "rygc_display_link_handle:";
// NOTE: private function
proc void
cocoa__display_link_handle(id self, SEL cmd, CADisplayLink *sender)
{
  Unused(self);
  Unused(cmd);
  Cocoa_Window *window = cocoa__window_from_display_link(sender);
  if(window->pending_frame)
  {
    window->pending_frame = 0;
    --cocoa_state->pending_frame_count;
  }
}

proc Cocoa_Window*
cocoa_window_open(V2S32 dim, String8 title)
{
  NSWindow *ns_window = 0;
  NSString *ns_title = 0;
  NSView *ns_view = 0;
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

  if(!objc_replace_method(NSResponder, keyDown, cocoa__on_key_down, "c@:@"))
  { goto cocoa_window_open_failure; }

  NSWindow_makeKeyAndOrderFront(ns_window, 0);
  NSWindow_setIsVisible(ns_window, true);

  // TODO: ensure title.string points to a null-terminated string
  ns_title = NSString_initWithBytesLength((const char*)title.string, title.count, NSUTF8StringEncoding);
  NSWindow_setTitle(ns_window, ns_title);

  ns_view = NSWindow_contentView(ns_window);
  NSView_setWantsLayer(ns_view, true);

  // NOTE: opengl initialization
  {
    if(mac_ogl_ctxt.ns_ctxt == 0)
    {
      if(!ogl_init())
      { goto cocoa_window_open_failure; }
    }

    NSOpenGLContext *ogl_ctxt = mac_ogl_ctxt.ns_ctxt;
    NSOpenGLContext_setView(ogl_ctxt, ns_view);
    NSOpenGLContext_update(ogl_ctxt);
  }

  layer = NSView_layer(ns_view);
  CALayer_setContentsGravity(layer, kCAGravityBottomLeft);

  // NOTE: set up frame-rate sync
  CADisplayLink *display_link;
  {
    Class metaclass = objc_getMetaClass("NSObject");
    id receiver = (id)objc_class_NSObject;
    SEL display_link_selector = sel_registerName(display_link_handler_selector_name);
    Assert(class_addMethod(metaclass, display_link_selector, (IMP)cocoa__display_link_handle, "v@:@"));
    NSRunLoop *run_loop = NSRunLoop_currentRunLoop();
    display_link = NSScreen_displayLinkWithTarget(cocoa_state->screen, receiver, display_link_selector);
    CADisplayLink_addToRunLoop(display_link, run_loop, NSRunLoopCommonModes);
  }

  Cocoa_PixelBuffer *backbuffer = cocoa__buffer_alloc();
  Cocoa_PixelBuffer *frontbuffer = cocoa__buffer_alloc();

  window = cocoa__window_alloc();
  cocoa__set_window_for_ns_window(ns_window, window);
  window->window = ns_window;
  window->ns_view = ns_view;
  window->layer = layer;
  cocoa__set_window_for_display_link(display_link, window);
  window->display_link = display_link;
  window->backbuffer = backbuffer;
  window->frontbuffer = frontbuffer;
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

proc V2S32
cocoa_window_dim(Cocoa_Window *win)
{
  NSRect bounds = NSView_bounds(win->ns_view);
  CGFloat width = bounds.size.width;
  CGFloat height = bounds.size.height;
  V2S32 result = v2s32((S32)width, (S32)height);
  return result;
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

proc V2S32
gfx_window_dim(Gfx_Handle window)
{
  Cocoa_Window *win = cocoa__window_from_gfx_handle(window);
  V2S32 result = cocoa_window_dim(win);
  return result;
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

global Gfx_Key gfx_key_from_cocoa_key[CocoaKey_Count] = {
#define X(name, value) [Glue(CocoaKey_, name)] = Glue(Gfx_Key_, name),
  COCOA_KEY_XLIST
#undef X
};

global Gfx_Key gfx_key_from_cocoa_mouse[] = {
  [NSEventTypeLeftMouseDown] = Gfx_Key_mouse_left,
  [NSEventTypeLeftMouseUp] = Gfx_Key_mouse_left,
  [NSEventTypeRightMouseDown] = Gfx_Key_mouse_right,
  [NSEventTypeRightMouseUp] = Gfx_Key_mouse_right,
  [NSEventTypeOtherMouseDown] = Gfx_Key_mouse_middle,
  [NSEventTypeOtherMouseUp] = Gfx_Key_mouse_middle,
};

proc void
cocoa_events(void)
{
  // TODO: make frame-rate wait configurable
  // TODO: this makes it so we don't poll for events until after we submit the first frame, which makes startup latency higher than it needs to be
  U64 pending_frame_loop_counter = 0;
  while(cocoa_state->pending_frame_count)
  {
    NSEvent *e = 0;
    NSEventMask mask = NSEventMaskAny;
    NSRunLoopMode mode = NSDefaultRunLoopMode;
    while((e = NSApplication_nextEventMatchingMask(cocoa_state->app, mask, 0, mode, true)) != 0)
    {
      NSEventType type = NSEvent_type(e);
      NSWindow *ns_window = NSEvent_window(e);
      Cocoa_Window *window = cocoa__window_from_ns_window(ns_window);
      switch(type)
      {
	case NSEventTypeLeftMouseDown:
	case NSEventTypeRightMouseDown:
	case NSEventTypeOtherMouseDown:
	case NSEventTypeLeftMouseUp:
	case NSEventTypeRightMouseUp:
	case NSEventTypeOtherMouseUp:
	{
	  Gfx_Event *event = gfx__event_new();
	  event->kind = gfx_event_kind_from_cocoa_event_type[type];
	  event->window = cocoa__gfx_handle_from_window(window);
	  event->key = gfx_key_from_cocoa_mouse[type];
	  gfx__event_push(event);
	}break;

	case NSEventTypeKeyDown:
	case NSEventTypeKeyUp:
	{
	  U16 cocoa_key = NSEvent_keyCode(e);
	  Gfx_Key gfx_key = gfx_key_from_cocoa_key[cocoa_key];

	  Gfx_Event *event = gfx__event_new();
	  event->kind = gfx_event_kind_from_cocoa_event_type[type];
	  event->window = cocoa__gfx_handle_from_window(window);
	  event->key = gfx_key;
	  gfx__event_push(event);
	}break;

	// TODO: handle events that we don't have os kinds for
	default:
	{
	  Gfx_Event *event = gfx__event_new();
	  Assert(event);
	  event->kind = gfx_event_kind_from_cocoa_event_type[type];
	  event->window = cocoa__gfx_handle_from_window(window);
	  gfx__event_push(event);
	}break;
      }

      NSApplication_sendEvent(cocoa_state->app, e);
    }

    NSApplication_updateWindows(cocoa_state->app);
    ++pending_frame_loop_counter;
    if(cocoa_state->pending_frame_count)
    { os_sleep_ms(1); }
  }

  printf("waited %llu times for pending frame\n", pending_frame_loop_counter);
}

// -----------------------------------------------------------------------------
// render

proc void
cocoa_set_render_target_kind(Cocoa_Window *window, Cocoa_Backend backend)
{
  window->backend = backend;

  NSWindow *ns_win = window->window;
  NSView *view = window->ns_view;
  NSWindow_setContentView(ns_win, view);
}

proc void
cocoa_pixel_render_target_from_window(Gfx_PixelRenderTarget *target, Cocoa_Window *window)
{
  Cocoa_PixelBuffer *backbuffer = window->backbuffer;
  CVPixelBufferLockBaseAddress(backbuffer->buf, 0);
  target->pixels = (U8*)CVPixelBufferGetBaseAddress(backbuffer->buf);
  target->stride = CVPixelBufferGetBytesPerRow(backbuffer->buf);
  target->row_count = backbuffer->pixels_height;
}

proc void
cocoa_ogl_render_target_from_window(Gfx_OglRenderTarget *target, Cocoa_Window *window)
{
  Unused(window);
  Gfx_Handle ogl_ctxt = cocoa__gfx_handle_from_ogl_context(&mac_ogl_ctxt);
  target->context = ogl_ctxt;
}

proc void
cocoa_submit_frame_pixels(Cocoa_Window *window)
{
  Cocoa_PixelBuffer *backbuffer = window->backbuffer;

  // NOTE: assign layer
  CVPixelBufferUnlockBaseAddress(backbuffer->buf, 0);
  CALayer_setContents(window->layer, (id)backbuffer->buf);

  // NOTE: resize layer
  NSRect bounds = NSView_bounds(window->ns_view);
  CGFloat norm_w = ClampToRange(bounds.size.width / (CGFloat)backbuffer->pixels_width, 0.0, 1.0);
  CGFloat norm_h = ClampToRange(bounds.size.height / (CGFloat)backbuffer->pixels_height, 0.0, 1.0);
  CGFloat norm_y = ClampToRange(1.0 - norm_h, 0.0, 1.0);
  CGRect contents_rect = CGRectMake(0, norm_y, norm_w, norm_h);
  CALayer_setContentsRect(window->layer, contents_rect);

  // NOTE: swap buffers
  Cocoa_PixelBuffer *temp = window->frontbuffer;
  window->frontbuffer = backbuffer;
  window->backbuffer = temp;

  window->pending_frame = 1;
  ++cocoa_state->pending_frame_count;
}

proc void
cocoa_submit_frame_ogl(Cocoa_Window *window)
{
  NSOpenGLContext *ogl_ctxt = mac_ogl_ctxt.ns_ctxt;
  NSOpenGLContext_flushBuffer(ogl_ctxt);
  glFlush();
  NSOpenGLContext_update(ogl_ctxt);

  window->pending_frame = 1;
  ++cocoa_state->pending_frame_count;
}

global Gfx_Backend gfx_backend_from_cocoa_backend[] = {
  [Cocoa_Backend_pixel_buffer] = Gfx_Backend_software,
  [Cocoa_Backend_opengl] = Gfx_Backend_opengl,
};

global Cocoa_Backend cocoa_backend_from_gfx_backend[] = {
  [Gfx_Backend_software] = Cocoa_Backend_pixel_buffer,
  [Gfx_Backend_opengl] = Cocoa_Backend_opengl,
};

proc Gfx_Backend
gfx_backend(Gfx_Handle window)
{
  Cocoa_Window *cocoa_window = cocoa__window_from_gfx_handle(window);
  Gfx_Backend result = gfx_backend_from_cocoa_backend[cocoa_window->backend];
  return result;
}

proc void
gfx_set_backend(Gfx_Handle window, Gfx_Backend backend)
{
  Cocoa_Window *cocoa_window = cocoa__window_from_gfx_handle(window);
  Cocoa_Backend cocoa_backend = cocoa_backend_from_gfx_backend[backend];
  cocoa_set_render_target_kind(cocoa_window, cocoa_backend);
}

proc void
gfx_pixel_render_target_from_window(Gfx_PixelRenderTarget *target, Gfx_Handle window)
{
  Cocoa_Window *cocoa_window = cocoa__window_from_gfx_handle(window);
  cocoa_pixel_render_target_from_window(target, cocoa_window);
}

proc void
gfx_ogl_render_target_from_window(Gfx_OglRenderTarget *target, Gfx_Handle window)
{
  Cocoa_Window *cocoa_window = cocoa__window_from_gfx_handle(window);
  cocoa_ogl_render_target_from_window(target, cocoa_window);
}

proc void
gfx_window_submit_frame_pixels(Gfx_Handle window)
{
  Cocoa_Window *cocoa_window = cocoa__window_from_gfx_handle(window);
  cocoa_submit_frame_pixels(cocoa_window);
}

proc void
gfx_window_submit_frame_ogl(Gfx_Handle window)
{
  Cocoa_Window *cocoa_window = cocoa__window_from_gfx_handle(window);
  cocoa_submit_frame_ogl(cocoa_window);
}

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

global const char *window_association_key = "rygc_cocoa_window";

proc inline Cocoa_Window*
cocoa__window_from_ns_window(NSWindow *ns_win)
{
  Cocoa_Window *result = (Cocoa_Window*)objc_getAssociatedObject(ns_win, window_association_key);
  return result;
}

proc inline void
cocoa__set_window_for_ns_window(NSWindow *ns_win, Cocoa_Window *window)
{
  objc_setAssociatedObject(ns_win, window_association_key, (id)window, OBJC_ASSOCIATION_ASSIGN);
}

proc inline Cocoa_Window*
cocoa__window_from_display_link(CADisplayLink *link)
{
  Cocoa_Window *result = (Cocoa_Window*)objc_getAssociatedObject(link, window_association_key);
  return result;
}

proc inline void
cocoa__set_window_for_display_link(CADisplayLink *link, Cocoa_Window *window)
{
  objc_setAssociatedObject(link, window_association_key, (id)window, OBJC_ASSOCIATION_ASSIGN);
}

proc inline Cocoa_PixelBuffer*
cocoa__buffer_alloc(void)
{
  Cocoa_PixelBuffer *result = cocoa_state->pbuf_freelist;
  if(result)
  {
    SLLStackPop(cocoa_state->pbuf_freelist);
  }
  else
  {
    CFAllocatorRef pbuf_allocator = kCFAllocatorDefault;
    OSType pbuf_fmt = kCVPixelFormatType_32BGRA;
    S32 screen_width = cocoa_state->screen_width;
    S32 screen_height = cocoa_state->screen_height;
    CFDictionaryRef pbuf_attr = cocoa_state->pbuf_attr;
    CVPixelBufferRef pbuf;
    CVPixelBufferCreate(pbuf_allocator, screen_width, screen_height, pbuf_fmt, pbuf_attr, &pbuf);

    result = arena_push_struct(cocoa_state->arena, Cocoa_PixelBuffer);
    result->buf = pbuf;
    result->pixels_width = screen_width;
    result->pixels_height = screen_height;
  }
  Assert(result);
  return result;
}

proc inline void
cocoa__buffer_release(Cocoa_PixelBuffer *buf)
{
  SLLStackPush(cocoa_state->pbuf_freelist, buf);
}

proc inline Gfx_Handle
cocoa__gfx_handle_from_ogl_context(Ogl_MacContext *ogl_ctxt)
{
  Gfx_Handle result = {0};
  result.handle = ogl_ctxt;
  return result;
}
