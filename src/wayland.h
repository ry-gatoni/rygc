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
  U32 wl_compositor_id;
  U32 xdg_wm_base_id;
  U32 wl_surface_id;
  U32 xdg_surface_id;
  U32 xdg_toplevel_id;
  U32 wl_shm_id;
  U32 wl_shm_pool_id;
  U32 wl_buffer_id;

  U32 next_id;

  int shared_memory_handle;
  void *shared_memory;

  Buffer message_buffer;     // NOTE: underlying storage for messages (ie dest of recv())
  Buffer frame_event_buffer; // NOTE: view into message_buffer
  U64 frame_index;
  U64 event_frame_index;     // NOTE: index of last frame events were polled

  S32 width;
  S32 height;

  String8List error_list;
};

typedef struct WaylandEvent
{
  U32 object_id;
  U32 opcode;
  Buffer body;
} WaylandEvent;

typedef enum WaylandInitialization
{
  WaylandInit_none,
  WaylandInit_acked,
  WaylandInit_attached,
} WaylandInitialization;

typedef struct WaylandState
{
  int display_socket_handle;
#if 1
  Arena *arena;
  WaylandWindow *first_window;
  WaylandWindow *last_window;
#else
  int shared_memory_handle;

  void *shared_memory;

  WaylandInitialization init_state;

  U32 wl_display_id;
  U32 wl_registry_id;
  U32 wl_shm_id;  
  U32 wl_compositor_id;
  U32 xdg_wm_base_id;
  U32 wl_surface_id;
  U32 xdg_surface_id;
  U32 xdg_toplevel_id;
  U32 wl_shm_pool_id;
  U32 wl_buffer_id;

  // NOTE: required by generated functions but unused    
  U32 wl_data_offer_id;
  U32 wl_data_source_id;
  U32 wl_data_device_id;
  U32 wl_data_device_manager_id;
  U32 wl_shell_id;
  U32 wl_shell_surface_id;  
  U32 wl_seat_id;
  U32 wl_pointer_id;
  U32 wl_keyboard_id;
  U32 wl_touch_id;
  U32 wl_output_id;
  U32 wl_region_id;
  U32 wl_subcompositor_id;
  U32 wl_subsurface_id;

  U32 xdg_positioner_id;  
  U32 xdg_popup_id;
  /* Arena *id_table_arena; */
  /* WaylandIdBucket **id_table; */
  /* U64 id_table_count; */
#endif
} WaylandState;

global WaylandState wayland_state;
//global U32 wayland_current_id = 1;

#define WAYLAND_MAX_CLIENT_OBJECT_ID 0xFEFFFFFF

// NOTE: functions
proc WaylandHashKey wayland_hash(String8 s);
//proc U32 *wayland_interface_object_id_from_name(String8 name);

proc B32 wayland_display_connect(void);

proc U32 wayland_new_id(WaylandWindow *window);
proc B32 wayland_display_get_registry(WaylandWindow *window);
proc B32 wayland_create_surface(WaylandWindow *window, String8 name);
proc B32 wayland_allocate_shared_memory(WaylandWindow *window, U64 size);
proc B32 wayland_create_buffers(WaylandWindow *window, S32 width, S32 height);

proc void wayland_handle_messages(WaylandWindow *window, Buffer *buffer);
proc B32 wayland_registry_bind(U32 name, String8 interface, U32 version, U32 new_id);
proc B32 wayland_events_polled_this_frame(WaylandWindow *window);

// NOTE: user-facing api
proc B32 wayland_init(void);
proc WaylandWindow* wayland_open_window(String8 name, S32 width, S32 height);
proc B32 wayland_get_event(WaylandWindow *window, WaylandEvent *event);
proc void wayland_swap_buffers(WaylandWindow *window);
