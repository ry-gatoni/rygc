#pragma pack(push, 1)
typedef struct WaylandMessageHeader
{
  U32 object_id;
  U16 opcode;
  U16 message_size;
} WaylandMessageHeader;
#pragma pack(pop)

typedef struct WaylandIdBucket
{
  String8 interface_name;
  U32 id;
} WaylandIdBucket;

typedef struct WaylandHashKey
{
  U64 hash;
  String8 string;
} WaylandHashKey;

typedef struct WaylandWindow WaylandWindow;
struct WaylandWindow
{
  WaylandWindow *next;

  U32 wl_display_id;
  U32 wl_registry_id;
  
  U32 wl_shm_id;
  U32 wl_compositor_id;
  U32 xdg_wm_base_id;
  U32 wl_seat_id;

  U32 wl_keyboard_id;
  U32 wl_pointer_id;

  U32 wl_surface_id;
  U32 xdg_surface_id;
  U32 xdg_toplevel_id;
  U32 wl_shm_pool_id;
  U32 wl_buffer_id;

  U32 next_id;

  int shared_memory_handle;
  void *shared_memory;

  Buffer message_buffer;     // NOTE: underlying storage for messages (ie dest of recv())
  Buffer frame_event_buffer; // NOTE: view into message_buffer

  B32 events_polled_this_frame;

  U64 frame_index;

  S32 width;
  S32 height;
};

// TODO: make an easier to use abstraction for the kind of events a user wants to respond to
typedef struct WaylandEvent
{
  U32 object_id;
  U32 opcode;
  Buffer body;
} WaylandEvent;

typedef struct WaylandState
{
  int display_socket_handle;

  Arena *arena;
  
  WaylandWindow *first_window;
  WaylandWindow *last_window;
  
  String8List error_list;
} WaylandState;

global WaylandState wayland_state;

#define WAYLAND_MAX_CLIENT_OBJECT_ID 0xFEFFFFFF

// NOTE: functions
proc WaylandHashKey wayland_hash(String8 s);

proc U32 wayland_new_id(WaylandWindow *window);

proc B32 wayland_display_connect(void);
proc B32 wayland_display_get_registry(WaylandWindow *window);
proc B32 wayland_create_surface(WaylandWindow *window, String8 name);
proc B32 wayland_allocate_shared_memory(WaylandWindow *window, U64 size);
proc B32 wayland_create_buffers(WaylandWindow *window, S32 width, S32 height);

proc Buffer wayland_poll_events(WaylandWindow *window);

proc void wayland_log_error_(char *fmt, ...);
#define wayland_log_error(message, ...) wayland_log_error_("ERROR(%s, %u): "message, __FUNCTION__, __LINE__, __VA_ARGS__)

// NOTE: user-facing api
proc B32 wayland_init(void);
proc WaylandWindow* wayland_open_window(String8 name, S32 width, S32 height);
proc B32 wayland_get_event(WaylandWindow *window, WaylandEvent *event);
proc B32 wayland_swap_buffers(WaylandWindow *window);
