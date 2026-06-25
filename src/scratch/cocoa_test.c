#include "base/base.h"
#include "gfx/gfx.h"
//#include "gfx/gfx.cocoa.h"

#include "base/base.c"
#include "gfx/gfx.c"
//#include "gfx/gfx.cocoa.c"

// -----------------------------------------------------------------------------
// main

global Cocoa_PixelBuffer *backbuffer  = 0;
global Cocoa_PixelBuffer *frontbuffer = 0;

proc void
render(NSView *view, NSRect frame_bounds)
{
  CALayer *layer = NSView_layer(view);
  CALayer_setContents(layer, (id)backbuffer->buf);
  CGFloat norm_w = ClampToRange(frame_bounds.size.width / (CGFloat)backbuffer->pixels_width, 0.0, 1.0);
  CGFloat norm_h = ClampToRange(frame_bounds.size.height / (CGFloat)backbuffer->pixels_height, 0.0, 1.0);
  CGFloat norm_y = ClampToRange(1.0 - norm_h, 0.0, 1.0);

  CGRect contents_rect = CGRectMake(0, norm_y, norm_w, norm_h);
  CALayer_setContentsRect(layer, contents_rect);
  /* CALayer_setGeometryFlipped(layer, true); */
  //CALayer_setNeedsDisplay(layer);

  // NOTE: swap buffers
  Cocoa_PixelBuffer *temp = frontbuffer;
  frontbuffer = backbuffer;
  backbuffer = temp;
}

global B32 running = 1;

proc B32
on_window_close(id self, SEL cmd, NSWindow *sender)
{
  Unused(self);
  Unused(cmd);
  Unused(sender);

  running = 0;
  return(1);
}

// TODO: how to deal with initial shift at start of resize?
proc NSSize
on_window_resize(id self, SEL cmd, NSWindow *sender, NSSize frame_size)
{
  Unused(self);
  Unused(cmd);

  CGFloat norm_w = ClampToRange(frame_size.width / (CGFloat)backbuffer->pixels_width, 0.0, 1.0);
  CGFloat norm_h = ClampToRange(frame_size.height / (CGFloat)backbuffer->pixels_height, 0.0, 1.0);
  CGFloat norm_y = ClampToRange(1.0 - norm_h, 0.0, 1.0);
  CGRect contents_rect = CGRectMake(0, norm_y, norm_w, norm_h);

  NSView *view = NSWindow_contentView(sender);
  CALayer *layer = NSView_layer(view);
  CALayer_setContentsRect(layer, contents_rect);
  /* CALayer_setGeometryFlipped(layer, true); */

  return(frame_size);
}

typedef struct BoxState
{
  V2 p;
  V2 v;
  V2 dim;
} BoxState;

proc void
update_and_draw(BoxState *box, U32 *pixels, U32 pixel_stride, V2S32 frame_dim)
{
  // clear background
  {
    S32 frame_w = frame_dim.width;
    S32 frame_h = frame_dim.height;
    U8 *dest = (U8*)pixels;
    for(S32 row_idx = 0; row_idx < frame_h; ++row_idx)
    {
      U32 *row = (U32*)(dest + row_idx*pixel_stride);
      for(S32 col_idx = 0; col_idx < frame_w; ++col_idx)
      {
	*row++ = 0xFF080C1C;
      }
    }
  }

  // update box
  if(0 > box->p.x + box->v.x || box->p.x + box->dim.x + box->v.x > frame_dim.width)
  {
    box->v.x = -box->v.x;
  }
  if(0 > box->p.y + box->v.y || box->p.y + box->dim.y + box->v.y > frame_dim.height)
  {
    box->v.y = -box->v.y;
  }

  box->p.x += box->v.x;
  box->p.y += box->v.y;

  // draw box
  {
    S32 box_x = (S32)box->p.x;
    S32 box_y = (S32)box->p.y;
    S32 box_w = (S32)box->dim.x;
    S32 box_h = (S32)box->dim.y;
    U8 *dest = (U8*)pixels + pixel_stride*box_y + box_x*sizeof(*pixels);
    for(S32 row_idx = 0; row_idx < box_h; ++row_idx)
    {
      U32 *row = (U32*)(dest + pixel_stride*row_idx);
      for(S32 col_idx = 0; col_idx < box_w; ++col_idx)
      {
	*row++ = 0xFFFF0000;
      }
    }
  }
}

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  int result = 0;

  if(!os_init())
  { result = 1; goto end; }

  if(!gfx_init())
  { result = 1; goto end; }

  Gfx_Handle window = gfx_window_open(640, 480, Str8Lit("cocoa test"));

#if 1
  B32 running = 1;
  while(running)
  {
    // handle events
    Gfx_EventSpan events = gfx_events();
    for(Gfx_Event *e = events.first; e != events.last; ++e)
    {
      if(!gfx_windows_are_equal(window, e->window))
      { continue; }

      switch(e->kind)
      {
	case Gfx_EventKind_close:
	{
	  running = 0;
	}break;

	default: {}break;
      }
    }

    // TODO: draw
  }

  gfx_window_close(window);

  gfx_uninit();
#else
  Arena *frame_arena = arena_alloc();
  B32 running = 1;
  while(running)
  {
    Gfx_EventList events = cocoa_events(frame_arena);
    for(Gfx_Event *e = events.first; e; e = e->next)
    {
      switch(e->kind)
      {
	case Gfx_EventKind_close:
	{
	  running = 0;
	}break;

	default: {}break;
      }
    }

    arena_clear(frame_arena);
  }
#endif

#if 0
  NSDefaultRunLoopMode = NSString_stringWithUTF8String("kCFRunLoopDefaultMode");
  NSRunLoopCommonModes = NSString_stringWithUTF8String("kCFRunLoopCommonModes");

  // init app
  NSApplication *app = 0;
  if((app = NSApplication_sharedApplication()) == 0)
  { result = 1; goto end; }
  if(!NSApplication_setActivationPolicy(app, NSApplicationActivationPolicyRegular))
  { result = 1; goto end; }

  // allocate backbuffer
  Arena *arena = arena_alloc();
  // TODO: get actual screen dim
  S32 screen_width = 1920;
  S32 screen_height = 1080;
  CFAllocatorRef pbuf_allocator = kCFAllocatorDefault;
  OSType pbuf_fmt = kCVPixelFormatType_32BGRA;
  CFStringRef pbuf_attr_iosurface_key = kCVPixelBufferIOSurfacePropertiesKey;
  CFDictionaryRef pbuf_attr_iosurface_val = CFDictionaryCreate(pbuf_allocator, 0, 0, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
  const void *pbuf_attr_keys[] = { pbuf_attr_iosurface_key, };
  const void *pbuf_attr_vals[] = { pbuf_attr_iosurface_val, };
  CFDictionaryRef pbuf_attr = CFDictionaryCreate(pbuf_allocator, pbuf_attr_keys, pbuf_attr_vals, 1, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
  backbuffer = arena_push_struct(arena, Cocoa_PixelBuffer);
  CVPixelBufferCreate(pbuf_allocator, screen_width, screen_height, pbuf_fmt, pbuf_attr, &backbuffer->buf);
  backbuffer->pixels_width = screen_width;
  backbuffer->pixels_height = screen_height;
  //backbuffer->pixels = CVPixelBufferGetBaseAddress(backbuffer->buf);

  frontbuffer = arena_push_struct(arena, Cocoa_PixelBuffer);
  CVPixelBufferCreate(pbuf_allocator, screen_width, screen_height, pbuf_fmt, pbuf_attr, &frontbuffer->buf);
  frontbuffer->pixels_width = screen_width;
  frontbuffer->pixels_height = screen_height;
  //frontbuffer->pixels = CVPixelBufferGetBaseAddress(frontbuffer->buf);

  CFRelease(pbuf_attr_iosurface_val);
  CFRelease(pbuf_attr);
  // NOTE: clear color
  /* { */
  /*   U32 color = 0x080C1CFF; */
  /*   U32 *dest = backbuffer_pixels; */
  /*   for(S32 idx = 0; idx < 2*screen_width*screen_height; ++idx) */
  /*   { */
  /*     *dest++ = color; */
  /*   } */
  /* } */

  // open window
  if(!objc_add_method(NSObject, windowShouldClose, on_window_close, "c@:@"))
  { result = 1; goto end; }

  if(!objc_add_method(NSObject, windowWillResize, on_window_resize, "{NSSize=dd}@:@{NSSize=dd}"))
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

  NSView *view = NSWindow_contentView(window);
  NSView_setWantsLayer(view, true);
  //NSView_setLayerContentsRedrawPolicy(view, NSViewLayerContentsRedrawDuringViewResize);
  //NSView_setLayerContentsRedrawPolicy(view, NSViewLayerContentsRedrawNever);

  CALayer *layer = NSView_layer(view);
  CALayer_setContentsGravity(layer, kCAGravityBottomLeft);
  /* CALayer_setContentsGravity(layer, kCAGravityTopLeft); */
  //CALayer_setContentsGravity(layer, kCAGravityResize);
  //CALayer_setGeometryFlipped(layer, true);

  NSApplication_activate(app);
  NSWindow_makeKeyAndOrderFront(window, 0);
  NSWindow_setIsVisible(window, true);

  NSApplication_finishLaunching(app);

  BoxState box = {0};
  box.p = v2(20, 40);
  box.v = v2(1.f, 3.f);
  box.dim = v2(20, 20);
  // TODO: more event handling
  // TODO: smooth resize
  R64 loop_counter_fixed_period = 1.0 / (R64)cpu_counter_fixed_freq();
  U64 loop_start = 0;
  U64 last_loop_end = cpu_counter_fixed();
  while(running)
  {
    loop_start = cpu_counter_fixed();
    U64 time_since_last_loop = loop_start - last_loop_end;
    R64 time_since_last_loop_ms = 1000.0 * (R64)time_since_last_loop * loop_counter_fixed_period;
    printf("time since last loop: %.6f\n", time_since_last_loop_ms);

    // NOTE: get events
    NSEvent *e = 0;
    NSEventMask mask = NSEventMaskAny;
    //NSRunLoopMode mode = NSString_stringWithUTF8String("kCFRunLoopDefaultMode");
    NSRunLoopMode mode = NSDefaultRunLoopMode;
    //NSRunLoopMode mode = NSModalPanelRunLoopMode;
    //NSRunLoopMode mode = NSEventTrackingRunLoopMode;
    while((e = NSApplication_nextEventMatchingMask(app, mask, 0, mode, true)) != 0)
    {
      NSEventType event_type = NSEvent_type(e);
      //NSEventSubtype event_subtype = NSEvent_subtype(e);
      NSPoint event_pos = NSEvent_locationInWindow(e);
      NSEventModifierFlags event_mods = NSEvent_modifierFlags(e);
      if(event_type)
      {
	printf("default event:\n"
	       "  type: %lu\n"
	       //"  subtype: %hd\n"
	       "  pos: (%.2f, %.2f)\n"
	       "  mods: %lu\n",
	       event_type,
	       //event_subtype,
	       event_pos.x, event_pos.y,
	       event_mods);
      }

      NSApplication_sendEvent(app, e);
    }

    /* mode = NSRunLoopCommonModes; */
    /* while((e = NSApplication_nextEventMatchingMask(app, mask, 0, mode, true)) != 0) */
    /* { */
    /*   NSEventType event_type = NSEvent_type(e); */
    /*   //NSEventSubtype event_subtype = NSEvent_subtype(e); */
    /*   NSPoint event_pos = NSEvent_locationInWindow(e); */
    /*   NSEventModifierFlags event_mods = NSEvent_modifierFlags(e); */
    /*   if(event_type) */
    /*   { */
    /* 	printf("common event:\n" */
    /* 	       "  type: %lu\n" */
    /* 	       //"  subtype: %hd\n" */
    /* 	       "  pos: (%.2f, %.2f)\n" */
    /* 	       "  mods: %lu\n", */
    /* 	       event_type, */
    /* 	       //event_subtype, */
    /* 	       event_pos.x, event_pos.y, */
    /* 	       event_mods); */
    /*   } */

    /*   NSApplication_sendEvent(app, e); */
    /* } */

    NSApplication_updateWindows(app);

    CGRect frame_bounds = NSView_bounds(view);
    V2S32 frame_dim = v2s32(frame_bounds.size.width, frame_bounds.size.height);

    CVPixelBufferLockBaseAddress(backbuffer->buf, 0);
    U32 *backbuffer_pixels = CVPixelBufferGetBaseAddress(backbuffer->buf);
    U32 pixel_stride = CVPixelBufferGetBytesPerRow(backbuffer->buf);
    update_and_draw(&box, backbuffer_pixels, pixel_stride, frame_dim);
    CVPixelBufferUnlockBaseAddress(backbuffer->buf, 0);

    render(view, frame_bounds);
    U64 loop_end = cpu_counter_fixed();
    U64 loop_elapsed = loop_end - loop_start;
    R64 loop_elapsed_ms = 1000.0 * (R64)loop_elapsed / (R64)cpu_counter_fixed_freq();
    printf("loop elapsed: %.6f ms\n", loop_elapsed_ms);
    last_loop_end = loop_end;

    usleep(16667);
  }
#endif

end:
  // TODO: proper cleanup
  return(result);
}
