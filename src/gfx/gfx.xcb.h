#include <xcb/xcb.h>
#include <xcb/shm.h>
#include <xcb/present.h>

#include <sys/shm.h>

// TODO:
// - events
// - opengl pixmap
// - integrate with general api
// - vulkan context
// - read keysyms/keycodes
// - gracefully handle window close
// - handle window resize

typedef enum Xcb_ExtensionFlags
{
  Xcb_Extension_shm	= 1 << 0,
  Xcb_Extension_present = 1 << 1,
} Xcb_ExtensionFlags;

typedef enum Xcb_Backend
{
  Xcb_Backend_base,
  Xcb_Backend_shm,
  Xcb_Backend_ogl,
  Xcb_Backend_Count,
} Xcb_Backend;

typedef enum Xcb_BackendFlag
{
  Xcb_BackendFlag_base = 1 << Xcb_Backend_base,
  Xcb_BackendFlag_shm  = 1 << Xcb_Backend_shm,
  Xcb_BackendFlag_ogl  = 1 << Xcb_Backend_ogl,
} Xcb_BackendFlag;

typedef struct Xcb_Window Xcb_Window;
struct Xcb_Window
{
  Xcb_Window *next;

  xcb_window_t id;

  V2S32 dim;

  void *backend_states[Xcb_Backend_Count];
};

typedef struct Xcb_WindowBase
{
  xcb_pixmap_t pixmap;

  U32 *pixels;
  U32 pixels_width;
  U32 pixels_height;
} Xcb_WindowBase;

typedef struct Xcb_WindowShm
{
  xcb_pixmap_t pixmap;

  U32 *pixels;
  U32 pixels_width;
  U32 pixels_height;

  int shm_id;
  xcb_shm_seg_t shm_segment;
} Xcb_WindowShm;

typedef struct Xcb_State
{
  Arena *arena;
  xcb_connection_t *conn;

  Xcb_Window *first_window;
  Xcb_Window *last_window;
  U64 window_count;
  Xcb_Window *window_freelist;

  xcb_screen_t *screen;

  xcb_gcontext_t g_ctxt;

  U32 extensions;

  U32 supported_backends;
  Xcb_Backend enabled_backend;
  void *backend_states[Xcb_Backend_Count];
} Xcb_State;

global Xcb_State *xcb_state = 0;
global xcb_void_cookie_t xcb_request_cookie = {0};
global xcb_generic_error_t *xcb_error = 0;

// -----------------------------------------------------------------------------
// state init/uninit

proc B32 xcb_init(void);
proc void xcb_uninit(void);

proc void xcb_init_backend(Xcb_Backend backend);
proc void xcb_uninit_backend(Xcb_Backend backend);

// -----------------------------------------------------------------------------
// window open/close

proc Xcb_Window* xcb_window_open(V2S32 dim);
proc void xcb_window_close(Xcb_Window *win);

proc void xcb_window_init_backend(Xcb_Window *win, Xcb_Backend backend);
proc void xcb_window_uninit_backend(Xcb_Window *win, Xcb_Backend backend);

// -----------------------------------------------------------------------------
// global helpers

proc Xcb_BackendFlag xcb_backend_flag(Xcb_Backend backend);
proc void xcb_log_error(xcb_generic_error_t *xcb_error, char *msg);

// -----------------------------------------------------------------------------
// window helpers

proc Xcb_Window* xcb_window_alloc(void);
proc void xcb_window_free(Xcb_Window *win);
