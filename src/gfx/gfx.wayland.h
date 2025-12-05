#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>

#include <linux/input-event-codes.h>

#include <xkbcommon/xkbcommon.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>

/* #define GL_GLEXT_PROTOTYPES */
/* #include <GL/gl.h> */
//#include "OpenGL/ogl.h"

#include <errno.h>

#pragma pack(push, 1)
typedef struct WaylandMessageHeader
{
  U32 object_id;
  U16 opcode;
  U16 message_size;
} WaylandMessageHeader;
#pragma pack(pop)

typedef struct WaylandId WaylandId;
struct WaylandId
{
  WaylandId *next;
  WaylandId *prev;

  struct WaylandWindow *window;
  U32 id;
};

typedef struct WaylandTempId WaylandTempId;
struct WaylandTempId
{
  WaylandTempId *next;
  U32 id;
};

typedef struct GlFramebuffer
{
  U32 color_texture;
  U32 depth_texture;
  U32 fbo;

  S32 width;
  S32 height;

  EGLImage egl_image;
  int num_planes;
  int fourcc;
  int fd;
  EGLint stride;
  EGLint offset;
  U32 modifier_hi;
  U32 modifier_lo;
} GlFramebuffer;

typedef enum RenderTarget
{
  RenderTarget_software,
  RenderTarget_hardware,
} RenderTarget;

typedef struct WaylandWindow WaylandWindow;
struct WaylandWindow
{
  WaylandWindow *next;
  WaylandWindow *prev;

  WaylandId *wl_surface;
  WaylandId *xdg_surface;
  WaylandId *xdg_toplevel;
  WaylandId *wl_shm_pool;

  WaylandId *zwp_linux_buffer_params_v1;

  WaylandId *buffers[2];
  U32 backbuffer_index;
  WaylandId *gl_buffer;

  WaylandId *frame_callback;
  U32 last_frame_timestamp;
  U32 last_frame_ms_elapsed;

  void *shared_memory;
  U64 shared_memory_size;

  GlFramebuffer gl_framebuffer[2];

  RenderTarget render_target;

  U64 frame_index;

  S32 width;
  S32 height;
};

typedef struct WaylandState
{
  Arena *arena;

  WaylandWindow *first_window;
  WaylandWindow *last_window;
  U64 window_count;

  WaylandWindow *window_freelist;

  WaylandWindow *focused_window;

  int display_socket_handle;

  // NOTE: global ids
  WaylandId *wl_display;
  WaylandId *wl_registry;

  WaylandId *wl_shm;
  WaylandId *zwp_linux_dmabuf_v1;
  WaylandId *wl_compositor;
  WaylandId *xdg_wm_base;
  WaylandId *wl_seat;
  WaylandId *wl_keyboard;
  WaylandId *wl_pointer;
  WaylandId *zwp_linux_dmabuf_feedback_v1;

  WaylandId *sync;

  // NOTE: window id lists
  WaylandId *first_wl_surface;
  WaylandId *last_wl_surface;
  U64 wl_surface_count;

  WaylandId *first_xdg_surface;
  WaylandId *last_xdg_surface;
  U64 xdg_surface_count;

  WaylandId *first_xdg_toplevel;
  WaylandId *last_xdg_toplevel;
  U64 xdg_toplevel_count;

  WaylandId *first_wl_shm_pool;
  WaylandId *last_wl_shm_pool;
  U64 wl_shm_pool_count;

  WaylandId *first_zwp_linux_buffer_params_v1;
  WaylandId *last_zwp_linux_buffer_params_v1;
  U64 zwp_linux_buffer_params_v1_count;

  WaylandId *first_wl_buffer;
  WaylandId *last_wl_buffer;
  U64 wl_buffer_count;

  WaylandId *first_gl_buffer;
  WaylandId *last_gl_buffer;
  U64 gl_buffer_count;

  WaylandId *first_frame_callback;
  WaylandId *last_frame_callback;
  U64 frame_callback_count;

  U32 next_id;
  WaylandId *id_freelist;

  void *dmabuf_format_table;
  U64 dmabuf_format_table_size;

  EGLDisplay egl_display;
  EGLContext egl_context;

  String8List error_list;

  struct xkb_context *xkb_context;
  struct xkb_keymap *xkb_keymap;
  struct xkb_state *xkb_state;
} WaylandState;

global WaylandState *wayland_state;

#define WAYLAND_MAX_CLIENT_OBJECT_ID 0xFEFFFFFF

// NOTE: import from egl
global PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR;
global PFNEGLEXPORTDMABUFIMAGEQUERYMESAPROC eglExportDMABUFImageQueryMESA;
global PFNEGLEXPORTDMABUFIMAGEMESAPROC eglExportDMABUFImageMESA;

// -----------------------------------------------------------------------------
// id management

proc WaylandId* wayland_new_id(WaylandWindow *window);
proc void wayland_release_id(WaylandId *id);
proc WaylandId* wayland_id_from_list(WaylandId *ids, U32 id);

// -----------------------------------------------------------------------------
// initialization helpers
// TODO: mark these inline, and don't have them here

proc B32 wayland_display_connect(void);
proc B32 wayland_display_get_registry(void);
proc B32 wayland_registry_bind_globals(void);
proc B32 wayland_get_capabilities(void);
proc B32 wayland_initialize_input(void);
proc B32 wayland_gl_init(void);

proc B32 wayland_create_surface(WaylandWindow *window, String8 name);
proc B32 wayland_allocate_shared_memory(WaylandWindow *window, U64 size);
proc B32 wayland_allocate_textures(WaylandWindow *window);
proc B32 wayland_create_buffer(WaylandWindow *window);
proc B32 wayland_create_shm_buffer(WaylandWindow *window);
proc B32 wayland_create_gl_buffer(WaylandWindow *window);

// -----------------------------------------------------------------------------
// event helpers

proc Buffer wayland_poll_events(Arena *arena);

proc Os_Key wayland_os_key_from_linux_event_code(U32 button);
proc Os_Key wayland_os_key_from_xkb_keysym(xkb_keysym_t keysym);

// NOTE: these functions don't depend on the wayland protocol or its
//       implementation, and so should be moved to an implementation-independent
//       place when the move to a full OS graphics abstraction layer is done

proc void wayland_log_error_(char *fmt, ...);
#define wayland_log_error(message, ...) wayland_log_error_("ERROR(%s, %u): "message, __FUNCTION__, __LINE__, __VA_ARGS__)

// TODO: find a better place for this
#define GlTextureDefaultParams() \
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); \
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); \
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); \
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); \

// -----------------------------------------------------------------------------
// window helpers

proc Os_Handle wayland_os_handle_from_window(WaylandWindow *window);
proc WaylandWindow* wayland_window_from_os_handle(Os_Handle handle);

// -----------------------------------------------------------------------------
// user-facing api / functions that get get wrapped by user-facing api

proc B32 wayland_init(void);
proc WaylandWindow* wayland_open_window(String8 name, S32 width, S32 height, RenderTarget render_target);

proc U32 *wayland_get_render_pixels(WaylandWindow *window);
proc GlFramebuffer wayland_get_framebuffer(WaylandWindow *window);

proc B32 wayland_end_frame(WaylandWindow *window);
proc void wayland_close_window(WaylandWindow *window);
