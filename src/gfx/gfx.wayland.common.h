#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>

#include <linux/input-event-codes.h>

#include <xkbcommon/xkbcommon.h>

//#include "gfx.wayland.ogl.h"

/* #include <EGL/egl.h> */
/* #include <EGL/eglext.h> */

/* #define GL_GLEXT_PROTOTYPES */
/* #include <GL/gl.h> */
//#include "OpenGL/ogl.h"

#include <errno.h>

typedef struct Wayland_Window Wayland_Window;

#pragma pack(push, 1)
typedef struct Wayland_MessageHeader
{
  U32 object_id;
  U16 opcode;
  U16 message_size;
} Wayland_MessageHeader;
#pragma pack(pop)

typedef struct Wayland_Id Wayland_Id;
struct Wayland_Id
{
  Wayland_Id *next;
  Wayland_Id *prev;

  Wayland_Window *window;
  U32 id;
};

/* typedef struct WaylandTempId WaylandTempId; */
/* struct WaylandTempId */
/* { */
/*   WaylandTempId *next; */
/*   U32 id; */
/* }; */

/* typedef struct GlFramebuffer */
/* { */
/*   U32 color_texture; */
/*   U32 depth_texture; */
/*   U32 fbo; */

/*   S32 width; */
/*   S32 height; */

/*   EGLImage egl_image; */
/*   int num_planes; */
/*   int fourcc; */
/*   int fd; */
/*   EGLint stride; */
/*   EGLint offset; */
/*   U32 modifier_hi; */
/*   U32 modifier_lo; */
/* } GlFramebuffer; */

typedef enum RenderTarget
{
  RenderTarget_software,
  RenderTarget_hardware,
} RenderTarget;

typedef enum Wayland_Backend
{
  Wayland_Backend_software,
  Wayland_Backend_opengl,
  Wayland_Backend_vulkan,
  Wayland_Backend_Count,
} Wayland_Backend;

typedef enum Wayland_BackendFlag
{
  Wayland_BackendFlag_software	= (1 << Wayland_Backend_software),
  Wayland_BackendFlag_opengl	= (1 << Wayland_Backend_opengl),
  Wayland_BackendFlag_vulkan	= (1 << Wayland_Backend_vulkan),
} Wayland_BackendFlag;

typedef B32 (Wayland_CreateBufferProc)(Wayland_Window *window);
/* typedef B32 (Wayland_WindowOpenProc)(Wayland_Window *window); */
/* typedef B32 (Wayland_WindowCloseProc)(Wayland_Window *window); */
typedef B32 (Wayland_WindowBeginFrameProc)(Wayland_Window *window);
typedef B32 (Wayland_WindowEndFrameProc)(Wayland_Window *window);

typedef struct Wayland_BackendApi
{
  Wayland_CreateBufferProc *create_buffer;
  /* Wayland_WindowOpenProc *open_window; */
  /* Wayland_WindowCloseProc *close_window; */
  Wayland_WindowBeginFrameProc *begin_frame;
  Wayland_WindowEndFrameProc *end_frame;
} Wayland_BackendApi;

struct Wayland_Window
{
  Wayland_Window *next;
  Wayland_Window *prev;

  Wayland_Id *wl_surface;
  Wayland_Id *xdg_surface;
  Wayland_Id *xdg_toplevel;
  /* Wayland_Id *wl_shm_pool; */

  //Wayland_Id *zwp_linux_buffer_params_v1;

  //Wayland_Id *buffers[2];
  //U32 backbuffer_index;
  //Wayland_Id *gl_buffer;

  Wayland_Id *frame_callback;
  U32 last_frame_timestamp;
  U32 last_frame_ms_elapsed;

  /* void *shared_memory; */
  /* U64 shared_memory_size; */

  //GlFramebuffer gl_framebuffer[2];

  //RenderTarget render_target;
  Wayland_Backend enabled_backend;
  void *backends[Wayland_Backend_Count]; // NOTE: backend data
  /* void *software_backend; */
  /* void *ogl_backend; */
  /* void *vulkan_backend; */

  U32 backbuffer_index;
  Wayland_Id *buffers[2];

  U64 frame_index;

  S32 width;
  S32 height;
};

typedef struct WaylandState
{
  Arena *arena;

  Wayland_Window *first_window;
  Wayland_Window *last_window;
  U64 window_count;

  Wayland_Window *window_freelist;

  Wayland_Window *focused_window;

  int display_socket_handle;

  // NOTE: global ids
  Wayland_Id *wl_display;
  Wayland_Id *wl_registry;

  Wayland_Id *wl_shm;
  Wayland_Id *zwp_linux_dmabuf_v1;
  Wayland_Id *wl_compositor;
  Wayland_Id *xdg_wm_base;
  Wayland_Id *wl_seat;
  Wayland_Id *wl_keyboard;
  Wayland_Id *wl_pointer;
  Wayland_Id *zwp_linux_dmabuf_feedback_v1;

  Wayland_Id *sync;

  // NOTE: window id lists
  Wayland_Id *first_wl_surface;
  Wayland_Id *last_wl_surface;
  U64 wl_surface_count;

  Wayland_Id *first_xdg_surface;
  Wayland_Id *last_xdg_surface;
  U64 xdg_surface_count;

  Wayland_Id *first_xdg_toplevel;
  Wayland_Id *last_xdg_toplevel;
  U64 xdg_toplevel_count;

  Wayland_Id *first_wl_shm_pool;
  Wayland_Id *last_wl_shm_pool;
  U64 wl_shm_pool_count;

  Wayland_Id *first_zwp_linux_buffer_params_v1;
  Wayland_Id *last_zwp_linux_buffer_params_v1;
  U64 zwp_linux_buffer_params_v1_count;

  Wayland_Id *first_wl_buffer;
  Wayland_Id *last_wl_buffer;
  U64 wl_buffer_count;

  /* Wayland_Id *first_gl_buffer; */
  /* Wayland_Id *last_gl_buffer; */
  /* U64 gl_buffer_count; */

  Wayland_Id *first_frame_callback;
  Wayland_Id *last_frame_callback;
  U64 frame_callback_count;

  U32 next_id;
  Wayland_Id *id_freelist;

  void *dmabuf_format_table;
  U64 dmabuf_format_table_size;

  /* EGLDisplay egl_display; */
  /* EGLContext egl_context; */

  U32 supported_backends; // @Wayland_BackendFlag
  Wayland_BackendApi backends[Wayland_Backend_Count];

  String8List error_list;

  struct xkb_context *xkb_context;
  struct xkb_keymap *xkb_keymap;
  struct xkb_state *xkb_state;
} WaylandState;

global WaylandState *wayland_state;

#define WAYLAND_MAX_CLIENT_OBJECT_ID 0xFEFFFFFF

// NOTE: import from egl
/* global PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR; */
/* global PFNEGLEXPORTDMABUFIMAGEQUERYMESAPROC eglExportDMABUFImageQueryMESA; */
/* global PFNEGLEXPORTDMABUFIMAGEMESAPROC eglExportDMABUFImageMESA; */

// -----------------------------------------------------------------------------
// id management

proc Wayland_Id* wayland__new_id(Wayland_Window *window);
proc void wayland__release_id(Wayland_Id *id);
proc Wayland_Id* wayland__id_from_list(Wayland_Id *ids, U32 id);

// -----------------------------------------------------------------------------
// initialization helpers
// TODO: mark these inline, and don't have them here

proc B32 wayland__display_connect(void);
proc B32 wayland__display_get_registry(void);
proc B32 wayland__registry_bind_globals(void);
proc B32 wayland__get_capabilities(void);
proc B32 wayland__initialize_input(void);
//proc B32 wayland_gl_init(void);

// TODO: abstract software vs hardware surface/buffer
proc B32 wayland__create_surface(Wayland_Window *window, String8 name);
proc B32 wayland_allocate_shared_memory(Wayland_Window *window, U64 size);
//proc B32 wayland_allocate_textures(Wayland_Window *window);
proc B32 wayland_create_buffer(Wayland_Window *window);
proc B32 wayland_create_shm_buffer(Wayland_Window *window);
//proc B32 wayland_create_gl_buffer(Wayland_Window *window);

// -----------------------------------------------------------------------------
// event helpers

proc Buffer wayland__poll_events(Arena *arena);

proc Os_Key wayland_os_key_from_linux_event_code(U32 button);
proc Os_Key wayland_os_key_from_xkb_keysym(xkb_keysym_t keysym);

// NOTE: these functions don't depend on the wayland protocol or its
//       implementation, and so should be moved to an implementation-independent
//       place when the move to a full OS graphics abstraction layer is done

proc void wayland_log_error_(char *fmt, ...);
#define wayland_log_error(message, ...) wayland_log_error_("ERROR(%s, %u): "message, __FUNCTION__, __LINE__, __VA_ARGS__)

// -----------------------------------------------------------------------------
// window helpers

proc Os_Handle wayland__os_handle_from_window(Wayland_Window *window);
proc Wayland_Window* wayland__window_from_os_handle(Os_Handle handle);

// -----------------------------------------------------------------------------
// user-facing api / functions that get get wrapped by user-facing api

proc B32 wayland_init(void);
proc void wayland_uninit(void);

proc Wayland_Window* wayland_window_open(String8 name, S32 width, S32 height);
proc void wayland_window_close(Wayland_Window *window);

proc B32 wayland_backend_is_supported(Wayland_Backend backend);
proc void wayland_window_set_backend(Wayland_Window *window, Wayland_Backend backend);

proc Os_EventList wayland_events(Arena *arena);

proc V2S32 wayland_window_get_dim(Wayland_Window *window);

// TODO: how to expose these uniformly for each backend?
/* proc U32 *wayland_get_render_pixels(Wayland_Window *window); */
/* proc GlFramebuffer wayland_get_framebuffer(Wayland_Window *window); */

proc B32 wayland_window_begin_frame(Wayland_Window *window);
proc B32 wayland_window_end_frame(Wayland_Window *window);
