#include "wayland_generated.c"

#if 0
proc void
wayland_state_initialize(void)
{
  Arena *id_arena = arena_alloc();
  wayland_state.id_table_arena = id_arena;
  wayland_state.id_table_count = 512;
  wayland_state.id_table = arena_push_array(id_arena, WaylandIdBucket*, wayland_state.id_table_count);
}
#endif

proc WaylandHashKey
wayland_hash(String8 s)
{
  WaylandHashKey result = {0};

  // TODO better hash function
  U64 hash = 5381;
  U8 *at = s.string;
  for(U64 i = 0; i < s.count; ++i, ++at) {
    hash = ((hash << 5) + hash + *at);
  }

  result.hash = hash;
  result.string = s;

  return(result);
}

#if 0
proc U32 *
wayland_interface_object_id_from_name(String8 name)
{
  U32 *result = 0;

  WaylandHashKey hash_key = wayland_hash(name);
  U64 hash_index = hash_key.hash % wayland_state.id_table_count;
  WaylandIdBucket **buckets = wayland_state.id_table;
  WaylandIdBucket *bucket = buckets[hash_index];
  if(!bucket) {
    buckets[hash_index] = arena_push_struct(wayland_state.id_table_arena, WaylandIdBucket);
    bucket = buckets[hash_index];
    bucket->interface_name = arena_push_str8_copy(wayland_state.id_table_arena, name);
    result = &bucket->id;
  }
  else {
    for(U64 i = hash_index; i < wayland_state.id_table_count; ++i) {
      if(str8s_are_equal(name, bucket->interface_name)) {
	result = &bucket->id;
	break;
      }
    }
  }

  Assert(result);
  return(result);
}
#endif

proc B32
wayland_display_connect(void)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);  

  // NOTE: get path to wayland display
  char *xdg_runtime_dir = getenv("XDG_RUNTIME_DIR");
  if(xdg_runtime_dir) {
    char *wayland_display = getenv("WAYLAND_DISPLAY");
    if(!wayland_display) {
      wayland_display = "wayland-0";
    }
    if(wayland_display) {
      // NOTE: open unix socket to wayland_display
      String8 wayland_display_path = str8_concat(scratch.arena,
						 Str8Cstr(xdg_runtime_dir),
						 Str8Cstr(wayland_display),
						 Str8Lit("/"));
      int socket_handle = socket(AF_UNIX, SOCK_STREAM, 0);
      if(socket_handle != -1) {
	struct sockaddr_un socket_addr = {.sun_family = AF_UNIX};
	CopySize(socket_addr.sun_path, wayland_display_path.string, wayland_display_path.count);
	int connection_result = connect(socket_handle, (struct sockaddr *)&socket_addr, sizeof(socket_addr));
	if(connection_result != -1) {
	  wayland_state.display_socket_handle = socket_handle;
	}
	else {
	  result = 0;
	  // TODO: log error: connecting to socket failed
	}
      }
      else {
	result = 0;
	// TODO: log error: opening socket failed
      }
    }
    else {
      result = 0;
      // TODO: log error: wayland_display not found
    }      
  }
  else {
    result = 0;
    // TODO: log error: xdg_runtime_dir not found
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wayland_display_get_registry(void)
{
  B32 result = wl_display_get_registry(++wayland_current_id);
  if(result) {
    wayland_state.wl_registry_id = wayland_current_id;
  }
  
  return(result);
}

proc B32
wayland_allocate_shared_memory(U64 size)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  // TODO: use memfd_create() on linux instead of creating a file by hand?
  char *filename = "fhgjkaspytqwczvxbteqiomzxcvalwxpm";
  int shared_memory_handle = shm_open(filename, O_RDWR | O_EXCL | O_CREAT, S_IRUSR | S_IWUSR);
  if(shared_memory_handle != -1) {    
    int shm_unlink_result = shm_unlink(filename);
    if(shm_unlink_result != -1) {
      int ftruncate_result = ftruncate(shared_memory_handle, size);
      if(ftruncate_result != -1) {
	void *shared_memory = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_handle, 0);
	if(shared_memory) {
	  wayland_state.shared_memory_handle = shared_memory_handle;
	  wayland_state.shared_memory = shared_memory;
	}
	else {
	  result = 0;
	  // TODO: log error
	}
      }
      else {
	result = 0;
	// TODO: log error
      }
    }
    else {
      result = 0;
      // TODO: log error
    }
  }
  else {
    result = 0;
    fprintf(stderr, "ERROR: shm_open failed: %s\n", strerror(errno));
    // TODO: log error: wayland_create_shared_memory_file: shm_open failure
  }

  arena_release_scratch(scratch);
  return(result);
}

proc void
wayland_handle_message(U8 **messages, S64 *len)
{
  Assert(*len >= 8);
  WaylandMessageHeader *header = (WaylandMessageHeader *)*messages;
  U32 object_id = header->object_id;
  U16 opcode = header->opcode;
  U16 message_size = header->message_size;

  printf("object_id: %u\n", object_id);
  printf("opcode: %u\n", opcode);
  printf("message size: %u\n", message_size);

  U32 *message = (U32 *)(header + 1);

  if(object_id == wayland_state.wl_display_id &&
     opcode == wl_display_error_opcode) {
    U32 error_object_id = *message;
    U32 error_code = *(message + 1);
    U32 error_string_count = *(message + 2);
    U8 *error_string = (U8 *)(message + 3);
    fprintf(stderr, "ERROR: object %u, code %u: %.*s\n",
	    error_object_id, error_code, (int)error_string_count, error_string);
  }
  else if(object_id == wayland_state.wl_registry_id &&
	  opcode == wl_registry_global_opcode) {
    U32 name = *message;
    U32 interface_string_count = *(message + 1);
    U8 *interface_string = (U8 *)(message + 2);
    U32 version = *(U32 *)(interface_string + AlignPow2(interface_string_count, 4));
    printf("registry global: name: %u, interface: %.*s, version: %u\n",
	   name, (int)interface_string_count, interface_string, version);

    String8 message_interface = {.count = (U64)interface_string_count - 1, .string = interface_string};
    String8 wl_shm = Str8Lit("wl_shm");
    String8 xdg_wm_base = Str8Lit("xdg_wm_base");
    String8 wl_compositor = Str8Lit("wl_compositor");
    if(str8s_are_equal(message_interface, wl_shm))
      {
	Assert(wayland_current_id < WAYLAND_MAX_CLIENT_OBJECT_ID);
	if(wl_registry_bind(name, wl_shm, version, ++wayland_current_id)) {
	  wayland_state.wl_shm_id = wayland_current_id;
	}
      }
    else if(str8s_are_equal(message_interface, xdg_wm_base))
      {
	Assert(wayland_current_id < WAYLAND_MAX_CLIENT_OBJECT_ID);
	if(wl_registry_bind(name, xdg_wm_base, version, ++wayland_current_id)) {
	  wayland_state.xdg_wm_base_id = wayland_current_id;
	}
      }
    else if(str8s_are_equal(message_interface, wl_compositor))
      {
	Assert(wayland_current_id < WAYLAND_MAX_CLIENT_OBJECT_ID);
	if(wl_registry_bind(name, wl_compositor, version, ++wayland_current_id)) {
	  wayland_state.wl_compositor_id = wayland_current_id;
	}
      }
  }

  *messages = (U8 *)header + message_size;
  *len -= message_size;
}
