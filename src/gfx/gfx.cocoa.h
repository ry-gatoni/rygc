#include "gfx.cocoa.api_defs.h"
#include "OpenGL/ogl.h"

#define COCOA_KEY_XLIST\
  X(a, 0)\
  X(s, 1)\
  X(d, 2)\
  X(f, 3)\
  X(h, 4)\
  X(g, 5)\
  X(z, 6)\
  X(x, 7)\
  X(c, 8)\
  X(v, 9)\
  X(b, 11)\
  X(q, 12)\
  X(w, 13)\
  X(e, 14)\
  X(r, 15)\
  X(y, 16)\
  X(t, 17)\
  X(1, 18)\
  X(2, 19)\
  X(3, 20)\
  X(4, 21)\
  X(6, 22)\
  X(5, 23)\
  X(equals, 24)\
  X(9, 25)\
  X(7, 26)\
  X(hyphen, 27)\
  X(8, 28)\
  X(0, 29)\
  X(r_bracket, 30)\
  X(o, 31)\
  X(u, 32)\
  X(l_bracket, 33)\
  X(i, 34)\
  X(p, 35)\
  X(enter, 36)\
  X(l, 37)\
  X(j, 38)\
  X(quote, 39)\
  X(semicolon, 41)\
  X(backslash, 42)\
  X(comma, 43)\
  X(slash, 44)\
  X(period, 47)\
  X(tab, 48)\
  X(space, 49)\
  X(backtick, 50)\
  X(backspace, 51)\
  X(esc, 53)\
  X(super, 55)\
  X(shift, 56)\
  X(capslock, 57)\
  X(alt, 58)\
  X(control, 59)\
  // TODO: finish

typedef enum CocoaKey
{
#define X(name, value) Glue(CocoaKey_, name) = value,
  COCOA_KEY_XLIST
#undef X
  CocoaKey_Count = 128,
} CocoaKey;

typedef enum Cocoa_Backend
{
  Cocoa_Backend_pixel_buffer,
  Cocoa_Backend_opengl,
} Cocoa_Backend;

typedef struct Cocoa_PixelBuffer Cocoa_PixelBuffer;
struct Cocoa_PixelBuffer
{
  Cocoa_PixelBuffer *next;

  U32 *pixels;
  S32 pixels_width;
  S32 pixels_height;

  CVPixelBufferRef buf;
};

typedef struct Cocoa_Window Cocoa_Window;
struct Cocoa_Window
{
  Cocoa_Window *next;
  Cocoa_Window *prev;

  NSWindow *window;

  NSView *ns_view;

  /* NSOpenGLView *ogl_view; */
  /* Ogl_MacContext ogl_ctxt; */

  CALayer *layer;
  CADisplayLink *display_link;

  B32 pending_frame;

  Cocoa_Backend backend;

  union{
    struct{
      Cocoa_PixelBuffer *backbuffer;
      Cocoa_PixelBuffer *frontbuffer;
    };
    Cocoa_PixelBuffer *buffers[2];
  };
};

typedef struct Cocoa_State
{
  Arena *arena;

  NSApplication *app;
  NSScreen *screen;

  S32 screen_width;
  S32 screen_height;

  Cocoa_Window *first_window;
  Cocoa_Window *last_window;
  U64 window_count;
  Cocoa_Window *window_freelist;

  U32 pending_frame_count;

  // NOTE: buffer allocator
  CFDictionaryRef pbuf_attr;
  Cocoa_PixelBuffer *pbuf_freelist;

} Cocoa_State;

global Cocoa_State *cocoa_state = 0;

// -----------------------------------------------------------------------------
// init

proc B32 cocoa_init(Arena *arena);
proc void cocoa_uninit(void);

// -----------------------------------------------------------------------------
// window

proc Cocoa_Window* cocoa_window_open(V2S32 dim, String8 title);
proc void cocoa_window_close(Cocoa_Window *win);

proc V2S32 cocoa_window_dim(Cocoa_Window *win);

// -----------------------------------------------------------------------------
// events

proc void cocoa_events();

// -----------------------------------------------------------------------------
// render

proc void cocoa_set_render_target_kind(Cocoa_Window *window, Cocoa_Backend backend);
proc void cocoa_pixel_render_target_from_window(Gfx_PixelRenderTarget *target, Cocoa_Window *window);
proc void cocoa_ogl_render_target_from_window(Gfx_OglRenderTarget *target, Cocoa_Window *window);

proc void cocoa_submit_frame_pixels(Cocoa_Window *window);
proc void cocoa_submit_frame_ogl(Cocoa_Window *window);

// -----------------------------------------------------------------------------
// helpers

proc inline Cocoa_Window* cocoa__window_alloc(void);
proc inline void cocoa__window_release(Cocoa_Window *win);

proc inline Gfx_Handle cocoa__gfx_handle_from_window(Cocoa_Window *win);
proc inline Cocoa_Window* cocoa__window_from_gfx_handle(Gfx_Handle win);

proc inline Cocoa_Window* cocoa__window_from_ns_window(NSWindow *ns_win);
proc inline void cocoa__set_window_for_ns_window(NSWindow *ns_win, Cocoa_Window *window);

proc inline Cocoa_Window* cocoa__window_from_display_link(CADisplayLink *link);
proc inline void cocoa__set_window_for_display_link(CADisplayLink *link, Cocoa_Window *window);

proc inline Cocoa_PixelBuffer* cocoa__buffer_alloc(void);
proc inline void cocoa__buffer_release(Cocoa_PixelBuffer *buf);

proc inline Gfx_Handle cocoa__gfx_handle_from_ogl_context(Ogl_MacContext *ogl_ctxt);
