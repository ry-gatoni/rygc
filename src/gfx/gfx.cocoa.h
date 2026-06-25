#include "gfx.cocoa.api_defs.h"

typedef struct Cocoa_PixelBuffer
{
  S32 pixels_width;
  S32 pixels_height;

  CVPixelBufferRef buf;
} Cocoa_PixelBuffer;

typedef struct Cocoa_Window Cocoa_Window;
struct Cocoa_Window
{
  Cocoa_Window *next;
  Cocoa_Window *prev;

  NSWindow *window;
  NSView *view;
  CALayer *layer;

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

  Cocoa_Window *first_window;
  Cocoa_Window *last_window;
  U64 window_count;
  Cocoa_Window *window_freelist;
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

// -----------------------------------------------------------------------------
// helpers

proc inline Cocoa_Window* cocoa__window_alloc(void);
proc inline void cocoa__window_release(Cocoa_Window *win);

proc inline Gfx_Handle cocoa__gfx_handle_from_window(Cocoa_Window *win);
proc inline Cocoa_Window* cocoa__window_from_gfx_handle(Gfx_Handle win);

proc inline Cocoa_Window* cocoa__window_from_ns_window(NSWindow *ns_win);
proc inline void cocoa__set_window_for_ns_window(NSWindow *ns_win, Cocoa_Window *window);
