#include <xcb/xcb.h>
#include <xcb/shm.h>
#include <xcb/present.h>

#include <EGL/egl.h>
//#define EGL_EGLEXT_PROTOTYPES
#include <EGL/eglext.h>

#include <GL/gl.h>

#include <sys/shm.h>

// TODO:
// - events:
//   - gracefully handle window close
//   - read keysyms/keycodes
//   - sync to refresh rate
//   - smooth resize
//   - smooth scroll
// - fully integrate with gfx api
// - integrate with render api
// - vulkan context
// - integrate with logging system
// - dynamically load libs
// - split backend implemenations/states into their own files
//   - maybe some dynamic functionality
// - accelerate window lookup by id
//   - potentially other lookups (eg by area)
// - configuration:
//   - override x server
//   - preferred screen

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

  Xcb_Backend selected_backend;
  void *backend_states[Xcb_Backend_Count];
};

typedef struct Xcb_WindowBase Xcb_WindowBase;
struct Xcb_WindowBase
{
  Xcb_WindowBase *next_free;

  xcb_pixmap_t pixmap;

  U32 *pixels;
  U32 pixels_width;
  U32 pixels_height;
};

typedef struct Xcb_WindowShm Xcb_WindowShm;
struct Xcb_WindowShm
{
  Xcb_WindowShm *next_free;

  xcb_pixmap_t pixmap;

  U32 *pixels;
  U32 pixels_width;
  U32 pixels_height;

  int shm_id;
  xcb_shm_seg_t shm_segment;
};

typedef struct Xcb_WindowOgl Xcb_WindowOgl;
struct Xcb_WindowOgl
{
  Xcb_WindowOgl *next_free;

  xcb_pixmap_t pixmap;
  EGLSurface egl_surface;
};

typedef struct Xcb_BackendOgl
{
  EGLDisplay egl_display;
  EGLConfig egl_config;
  EGLContext egl_context;
} Xcb_BackendOgl;

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
  void *backend_states[Xcb_Backend_Count];
  void *window_backend_freelist[Xcb_Backend_Count];
} Xcb_State;


global Xcb_State *xcb_state = 0;
global xcb_void_cookie_t xcb_request_cookie = {0};
global xcb_generic_error_t *xcb_error = 0;

// NOTE: documentation appears to be wrong here
global Os_Key xcb_button_map[] = {
  [XCB_BUTTON_INDEX_1] = Os_Key_mouse_left,
  [XCB_BUTTON_INDEX_2] = Os_Key_mouse_middle,
  [XCB_BUTTON_INDEX_3] = Os_Key_mouse_right,
};

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

proc B32 xcb_select_backend(Xcb_Window *win, Xcb_Backend backend);

// -----------------------------------------------------------------------------
// events

proc Os_EventList xcb_events(Arena *arena);

// -----------------------------------------------------------------------------
// global helpers

proc Xcb_BackendFlag xcb_backend_flag(Xcb_Backend backend);
proc B32 xcb_backend_is_supported(Xcb_Backend backend);
proc void xcb_log_error(xcb_generic_error_t *xcb_error, char *msg);

// -----------------------------------------------------------------------------
// window helpers

proc Xcb_Window* xcb_window_alloc(void);
proc void xcb_window_free(Xcb_Window *win);

proc Xcb_Window* xcb_window_from_id(xcb_window_t id);

proc Os_Handle gfx__handle_from_xcb_window(Xcb_Window *win);
proc Xcb_Window* xcb__window_from_gfx_handle(Os_Handle handle);
