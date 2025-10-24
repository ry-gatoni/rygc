#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>

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
  /* GLuint color_texture; */
  /* GLuint depth_texture; */
  /* GLuint fbo; */
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
  
  /* U32 wl_keyboard_id; */
  /* U32 wl_pointer_id; */

  /* U32 wl_surface_id; */
  /* U32 xdg_surface_id; */
  /* U32 xdg_toplevel_id; */
  /* U32 wl_shm_pool_id; */

  /* U32 zwp_linux_buffer_params_v1_id;   */
  WaylandId *wl_surface;
  WaylandId *xdg_surface;
  WaylandId *xdg_toplevel;
  WaylandId *wl_shm_pool;

  WaylandId *zwp_linux_buffer_params_v1;  

  /* WaylandTempId *buffer_id[2]; */
  WaylandId *buffers[2];
  U32 backbuffer_index;
  /* WaylandTempId *gl_buffer_id;   */
  WaylandId *gl_buffer;

  /* WaylandTempId *frame_callback_id; // NOTE: id to check for frame callback */
  WaylandId *frame_callback;
  U32 last_frame_timestamp;
  U32 last_frame_ms_elapsed;

  void *shared_memory;
  U64 shared_memory_size;

  GlFramebuffer gl_framebuffer[2];

  //Arena *event_arena; // NOTE: cleared each frame

  RenderTarget render_target;

  U64 frame_index;

  S32 width;
  S32 height;
};

// NOTE: from `linux/input-event-codes.h`
#define WAYLAND_BUTTON_MOUSE_MASK	(0x110)
#define WAYLAND_BUTTON_JOYSTICK_MASK	(0x120)
#define WAYLAND_BUTTON_GAMEPAD_MASK	(0x130)
// NOTE: from `xkbcommon/xkbcommon-keysyms.h`
#define WAYLAND_BUTTON_KEY_MASK		(0xFF00)

typedef enum Wayland_Key
{  
  Wayland_Key_mouse_left,
  Wayland_Key_mouse_right,
  Wayland_Key_mouse_middle,
  Wayland_Key_mouse_side,
  Wayland_Key_mouse_extra,
  Wayland_Key_mouse_forward,
  Wayland_Key_mouse_back,
  Wayland_Key_mouse_task,

  Wayland_Key_joystick_trigger,
  Wayland_Key_joystick_thumb,
  Wayland_Key_joystick_thumb2,
  Wayland_Key_joystick_top,
  Wayland_Key_joystick_top2,
  Wayland_Key_joystick_pinkie,
  Wayland_Key_joystick_base,
  Wayland_Key_joystick_base2,
  Wayland_Key_joystick_base3,
  Wayland_Key_joystick_base4,
  Wayland_Key_joystick_base5,
  Wayland_Key_joystick_base6,

  Wayland_Key_keyboard_backspace,

  Wayland_Key_Count,
} Wayland_Key;

global Os_Key os_key_from_wayland_key[Wayland_Key_Count] = {
  [Wayland_Key_mouse_left] = Os_Key_mouse_left,
  [Wayland_Key_mouse_right] = Os_Key_mouse_right,
  [Wayland_Key_mouse_middle] = Os_Key_mouse_middle,
  [Wayland_Key_mouse_side] = Os_Key_mouse_side,
  [Wayland_Key_mouse_extra] = Os_Key_mouse_extra,
  [Wayland_Key_mouse_forward] = Os_Key_mouse_forward,
  [Wayland_Key_mouse_back] = Os_Key_mouse_back,
  [Wayland_Key_mouse_task] = Os_Key_mouse_task,

  [Wayland_Key_joystick_trigger] = Os_Key_joystick_trigger,
  [Wayland_Key_joystick_thumb] = Os_Key_joystick_thumb,
  [Wayland_Key_joystick_thumb2] = Os_Key_joystick_thumb2,
  [Wayland_Key_joystick_top] = Os_Key_joystick_top,
  [Wayland_Key_joystick_top2] = Os_Key_joystick_top2,
  [Wayland_Key_joystick_pinkie] = Os_Key_joystick_pinkie,
  [Wayland_Key_joystick_base] = Os_Key_joystick_base,
  [Wayland_Key_joystick_base2] = Os_Key_joystick_base2,
  [Wayland_Key_joystick_base3] = Os_Key_joystick_base3,
  [Wayland_Key_joystick_base4] = Os_Key_joystick_base4,
  [Wayland_Key_joystick_base5] = Os_Key_joystick_base5,
  [Wayland_Key_joystick_base6] = Os_Key_joystick_base6,

  [Wayland_Key_keyboard_backspace] = Os_Key_keyboard_backspace,
};

typedef struct WaylandState
{
  Arena *arena;
  
  WaylandWindow *first_window;
  WaylandWindow *last_window;
  U64 window_count;

  WaylandWindow *focused_window;

  int display_socket_handle;

  // NOTE: global ids
  /* U32 wl_display_id; */
  /* U32 wl_registry_id; */
  WaylandId *wl_display;
  WaylandId *wl_registry;
  
  /* U32 wl_shm_id; */
  /* U32 zwp_linux_dmabuf_v1_id; */
  /* U32 wl_compositor_id; */
  /* U32 xdg_wm_base_id; */
  /* U32 wl_seat_id; */
  /* U32 zwp_linux_dmabuf_feedback_v1_id; */
  WaylandId *wl_shm;
  WaylandId *zwp_linux_dmabuf_v1;
  WaylandId *wl_compositor;
  WaylandId *xdg_wm_base;
  WaylandId *wl_seat;
  WaylandId *wl_keyboard;
  WaylandId *wl_pointer;
  WaylandId *zwp_linux_dmabuf_feedback_v1;

  /* WaylandTempId *sync_id; */
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
  /* WaylandTempId *id_freelist; */
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

// NOTE: functions

proc WaylandId* wayland_new_id(WaylandWindow *window);
//proc WaylandTempId* wayland_temp_id(void);
proc void wayland_release_id(WaylandId *id);
proc WaylandId* wayland_id_from_list(WaylandId *ids, U32 id);

// NOTE: initialization helpers
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

proc Buffer wayland_poll_events(Arena *arena);

// NOTE: these functions don't depend on the wayland protocol or its
//       implementation, and so should be moved to an implementation-independent
//       place when the move to a full OS graphics abstraction layer is done
/* proc void event_list_push_ex(EventList *list, Event event, EventNode *node); */
/* proc void event_list_push(Arena *arena, EventList *list, Event event); */

proc void wayland_log_error_(char *fmt, ...);
#define wayland_log_error(message, ...) wayland_log_error_("ERROR(%s, %u): "message, __FUNCTION__, __LINE__, __VA_ARGS__)

// TODO: find a better place for this
#define GlTextureDefaultParams() \
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); \
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); \
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); \
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); \


// NOTE: user-facing api
proc B32 wayland_init(void);
proc WaylandWindow* wayland_open_window(String8 name, S32 width, S32 height, RenderTarget render_target);

proc U32 *wayland_get_render_pixels(WaylandWindow *window);
proc GlFramebuffer wayland_get_framebuffer(WaylandWindow *window);

/* proc EventList wayland_get_events(WaylandWindow *window); */
/* proc B32 next_event(EventList *events, Event *event); */

proc B32 wayland_end_frame(WaylandWindow *window);
proc void wayland_close_window(WaylandWindow *window);
