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

typedef struct WaylandState
{
  int display_socket_handle;
  int shared_memory_handle;

  U32 wl_registry_id;
  U32 wl_shm_id;
  U32 xdg_wm_base_id;
  U32 wl_compositor_id;
  /* Arena *id_table_arena; */
  /* WaylandIdBucket **id_table; */
  /* U64 id_table_count; */

  void *shared_memory;
} WaylandState;

typedef struct WaylandHashKey
{
  U64 hash;
  String8 string;
} WaylandHashKey;

global U32 wl_display_id = 1;

/* global U32 wl_display_op_get_registry = 1; */
/* global U32 wl_registry_op_bind = 0; */

/* global U32 wl_display_event_error = 0; */
/* global U32 wl_display_event_delete_id = 1; */
/* global U32 wl_registry_event_global = 0; */
/* global U32 wl_registry_event_global_remove = 1; */

global WaylandState wayland_state = {0};
global U32 wayland_current_id = 1;

#define WAYLAND_MAX_CLIENT_OBJECT_ID 0xFEFFFFFF

// NOTE: functions
proc WaylandHashKey wayland_hash(String8 s);
//proc U32 *wayland_interface_object_id_from_name(String8 name);

//proc void wayland_state_initialize(void);

proc B32 wayland_display_connect(void);
proc B32 wayland_display_get_registry(void);
//proc void wayland_registry_bind(U32 name, String8 interface_name, U32 new_id);
proc B32 wayland_allocate_shared_memory(U64 size);
proc void wayland_handle_message(U8 **messages, S64 *len);

/* proc void wayland_registry_bind_wl_shm(U32 name, U32 new_id); */
/* proc void wayland_registry_bind_xdg_wm_base(U32 name, U32 new_id); */
/* proc void wayland_registry_bind_wl_compositor(U32 name, U32 new_id); */
proc B32 wayland_registry_bind(U32 name, String8 interface, U32 version, U32 new_id);
