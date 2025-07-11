#include <xkbcommon/xkbcommon.h>

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

typedef struct WaylandTempId WaylandTempId;
struct WaylandTempId
{
  WaylandTempId *next;
  U32 id;
};

typedef struct WaylandWindow WaylandWindow;
struct WaylandWindow
{
  WaylandWindow *next;
  
  U32 wl_keyboard_id;
  U32 wl_pointer_id;

  U32 wl_surface_id;
  U32 xdg_surface_id;
  U32 xdg_toplevel_id;
  U32 wl_shm_pool_id;

  U32 zwp_linux_buffer_params_v1_id;  

  WaylandTempId *buffer_id;

  WaylandTempId *frame_callback_id; // NOTE: id to check for frame callback
  U32 last_frame_timestamp;
  U32 last_frame_ms_elapsed;

  void *shared_memory;
  U64 shared_memory_size;

  struct xkb_keymap *xkb_keymap;
  struct xkb_state *xkb_state;

  Arena *event_arena; // NOTE: cleared each frame

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

typedef enum EventKind
{
  EventKind_null,
  EventKind_Press,
  EventKind_Release,
  EventKind_Move,
  EventKind_Close,
  
  EventKind_Count,
} EventKind;

typedef enum EventButton
{
  EventButton_none = 1,
  EventButton_MouseLeft = 1,
  EventButton_MouseMiddle = 2,
  EventButton_MouseRight = 3,
  EventButton_KeySpace = XKB_KEY_space,
  
  EventButton_Count,
} EventButton;

typedef struct Event
{
  EventKind kind;

  EventButton button;
  V2 position;
} Event;

typedef struct EventNode EventNode;
struct EventNode
{
  EventNode *next;
  EventNode *prev;
  Event event;
};

typedef struct EventList
{
  EventNode *first;
  EventNode *last;
  U64 count;
} EventList;

typedef struct WaylandState
{
  int display_socket_handle;

  Arena *arena;
  
  WaylandWindow *first_window;
  WaylandWindow *last_window;

  U32 wl_display_id;
  U32 wl_registry_id;
  
  U32 wl_shm_id;
  U32 zwp_linux_dmabuf_v1_id;
  U32 wl_compositor_id;
  U32 xdg_wm_base_id;
  U32 wl_seat_id;
  U32 zwp_linux_dmabuf_feedback_v1_id;

  U32 next_id;
  WaylandTempId *id_freelist;

  String8List error_list;

  struct xkb_context *xkb_context;  
} WaylandState;

global WaylandState wayland_state;

#define WAYLAND_MAX_CLIENT_OBJECT_ID 0xFEFFFFFF

// NOTE: functions
proc WaylandHashKey wayland_hash(String8 s);

proc U32 wayland_new_id(void);
proc WaylandTempId* wayland_temp_id(void);
proc void wayland_release_id(WaylandTempId *id);

proc B32 wayland_display_connect(void);
proc B32 wayland_display_get_registry(void);
proc B32 wayland_registry_bind_globals(void);
proc B32 wayland_initialize_input(WaylandWindow *window);
proc B32 wayland_create_surface(WaylandWindow *window, String8 name);
proc B32 wayland_allocate_shared_memory(WaylandWindow *window, U64 size);
proc B32 wayland_create_buffer(WaylandWindow *window);
proc B32 wayland_dmabuf_create_buffer(WaylandWindow *window);

proc Buffer wayland_poll_events(Arena *arena);

// NOTE: these functions don't depend on the wayland protocol or its
//       implementation, and so should be moved to an implementation-independent
//       place when the move to a full OS graphics abstraction layer is done
proc void event_list_push_ex(EventList *list, Event event, EventNode *node);
proc void event_list_push(Arena *arena, EventList *list, Event event);

proc void wayland_log_error_(char *fmt, ...);
#define wayland_log_error(message, ...) wayland_log_error_("ERROR(%s, %u): "message, __FUNCTION__, __LINE__, __VA_ARGS__)

// NOTE: user-facing api
proc B32 wayland_init(void);
proc WaylandWindow* wayland_open_window(String8 name, S32 width, S32 height);

proc EventList wayland_get_events(WaylandWindow *window);
proc B32 next_event(EventList *events, Event *event);

proc B32 wayland_end_frame(WaylandWindow *window);
proc void wayland_close_window(WaylandWindow *window);
