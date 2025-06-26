#include "wayland_generated.c"

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
wayland_init(void)
{
  B32 result = wayland_display_connect();
  if(result){
    wayland_state.arena = arena_alloc();
  }

  return(result);
}

proc U32
wayland_new_id(WaylandWindow *window) {
  Assert(window->next_id <= WAYLAND_MAX_CLIENT_OBJECT_ID);
  U32 result = window->next_id++;
  return(result);
}

proc B32
wayland_display_get_registry(WaylandWindow *window)
{
  U32 registry_id = wayland_new_id(window);
  B32 result = wl_display_get_registry(window->wl_display_id, registry_id);
  if(result) {
    window->wl_registry_id = registry_id;
  }
  
  return(result);
}

proc B32
wayland_create_surface(WaylandWindow *window, String8 title)
{
  B32 result = 1;

  U32 wl_surface_id = wayland_new_id(window);
  if(wl_compositor_create_surface(window->wl_compositor_id, wl_surface_id)) {
    window->wl_surface_id = wl_surface_id;

    U32 xdg_surface_id = wayland_new_id(window);
    if(xdg_wm_base_get_xdg_surface(window->xdg_wm_base_id, xdg_surface_id, wl_surface_id)) {
      window->xdg_surface_id = xdg_surface_id;

      U32 toplevel_id = wayland_new_id(window);
      if(xdg_surface_get_toplevel(window->xdg_surface_id, toplevel_id)) {
	window->xdg_toplevel_id = toplevel_id;	

	if(wl_surface_commit(window->wl_surface_id)) {
	  // TODO: better logging
	  fprintf(stderr, "surface committed\n");
	} else {
	  // NOTE: surface commit failed
	  result = 0;
	}

	/* if(xdg_toplevel_set_title(window->xdg_toplevel_id, title)) {	   */
	/* } else { */
	/*   // NOTE: set title failed */
	/*   result = 0; */
	/* } */
      } else {
	// NOTE: get toplevel failed
	result = 0;
      }
    } else {
      // NOTE: get xdg surface failed
      result = 0;
    }
  } else {
    // NOTE: create surface failed
    result = 0;
  }

  return(result);
}

proc B32
wayland_allocate_shared_memory(WaylandWindow *window, U64 size)
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
	  window->shared_memory_handle = shared_memory_handle;
	  window->shared_memory = shared_memory;

	  U32 wl_shm_pool_id = wayland_new_id(window);
	  if(wl_shm_create_pool(window->wl_shm_id, wl_shm_pool_id, window->shared_memory_handle, size)) {
	    window->wl_shm_pool_id = wl_shm_pool_id;
	  } else {
	    // NOTE: create pool failed
	    result = 0;
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

proc B32
wayland_create_buffers(WaylandWindow *window, S32 width, S32 height)
{
  B32 result = 1;

  S32 stride = width*sizeof(U32);
  U32 format = 0x34324241; // NOTE: AGBR 32-bit little-endian // TODO: generate enums
  U32 wl_buffer_id = wayland_new_id(window);
  if(wl_shm_pool_create_buffer(window->wl_shm_pool_id, wl_buffer_id, 0, width, height, stride, format)) {
    window->wl_buffer_id = wl_buffer_id;

    if(wl_surface_attach(window->wl_surface_id, window->wl_buffer_id, 0, 0)) {
      if(wl_surface_commit(window->wl_surface_id)) {
	window->width = width;
	window->height = height;
      } else {
	// NOTE: commit failed
	result = 0;
      }
    } else {
      // NOTE: attach failed
    }
  } else {
    // NOTE: buffer creation failed
    result = 0;
  }

  return(result);
}

proc void
wayland_log_error_(char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  str8_list_push_fv(wayland_state.arena, &wayland_state.error_list, fmt, args);
  va_end(args);
}

proc Buffer
wayland_poll_events(WaylandWindow *window) {
  Buffer result = {0};
  
  S64 recv_size = recv(wayland_state.display_socket_handle,
		       window->message_buffer.mem,
		       window->message_buffer.size,
		       0);
  if(recv_size != -1) {
    result.size = recv_size;
    result.mem = window->message_buffer.mem;    
  } else {
    wayland_log_error("recv failed: %s", strerror(errno));
  }

  return(result);
}

proc WaylandWindow*
wayland_open_window(String8 title, S32 width, S32 height)
{
  U64 wayland_arena_pre_init_pos = arena_pos(wayland_state.arena);
  
  WaylandWindow *window = arena_push_struct(wayland_state.arena, WaylandWindow);
  window->wl_display_id = 1;
  window->next_id = 2;
    
  if(wayland_display_get_registry(window)) {
    window->message_buffer.size = KB(4);
    window->message_buffer.mem = arena_push_array(wayland_state.arena, U8, window->message_buffer.size);
    
    Buffer events = wayland_poll_events(window);
    while(events.size) {
      WaylandMessageHeader *event_header = (WaylandMessageHeader *)events.mem;
      U32 object_id = event_header->object_id;
      U32 opcode = event_header->opcode;
      U32 message_size = event_header->message_size;
      U32 *event_body = (U32 *)(event_header + 1);

      // NOTE: bind globals
      // TODO: generate message reading functions
      if(object_id == window->wl_registry_id &&
	 opcode == wl_registry_global_opcode) {
	U32 name = event_body[0];
	U32 interface_string_count = event_body[1];
	U8 *interface_string = (U8 *)(event_body + 2);
	U32 version = *(U32 *)(interface_string + AlignPow2(interface_string_count, 4));

	String8 message_interface = {.count = (U64)interface_string_count - 1, .string = interface_string};
	String8 wl_shm_str = Str8Lit("wl_shm");
	String8 xdg_wm_base_str = Str8Lit("xdg_wm_base");
	String8 wl_compositor_str = Str8Lit("wl_compositor");
	if(str8s_are_equal(message_interface, wl_shm_str))
	  {
	    U32 shm_id = wayland_new_id(window);
	    if(wl_registry_bind(window->wl_registry_id, name, wl_shm_str, version, shm_id)) {
	      window->wl_shm_id = shm_id;
	    }
	  }
	else if(str8s_are_equal(message_interface, xdg_wm_base_str))
	  {
	    U32 wm_base_id = wayland_new_id(window);
	    if(wl_registry_bind(window->wl_registry_id, name, xdg_wm_base_str, version, wm_base_id)) {
	      window->xdg_wm_base_id = wm_base_id;
	    }
	  }
	else if(str8s_are_equal(message_interface, wl_compositor_str))
	  {
	    U32 compositor_id = wayland_new_id(window);
	    if(wl_registry_bind(window->wl_registry_id, name, wl_compositor_str, version, compositor_id)) {
	      window->wl_compositor_id = compositor_id;
	    }
	  }
      }
      else {
	// TODO: better logging
	fprintf(stderr, "unhandled message: object=%u, opcode=%u, length=%u\n", object_id, opcode, message_size);
      }

      events.mem += message_size;
      events.size -= message_size;
    }

    if(wayland_create_surface(window, title)) {
      B32 acked_configure = 0;
      while(!acked_configure) {
	events = wayland_poll_events(window);
	while(events.size) {
	  WaylandMessageHeader *event_header = (WaylandMessageHeader *)events.mem;
	  U32 object_id = event_header->object_id;
	  U32 opcode = event_header->opcode;
	  U32 message_size = event_header->message_size;
	  U32 *event_body = (U32 *)(event_header + 1);

	  if(object_id == window->xdg_wm_base_id &&
	     opcode == xdg_wm_base_ping_opcode) {
	    U32 serial = event_body[0];
	    xdg_wm_base_pong(window->xdg_wm_base_id, serial);
	  }
	  else if(object_id == window->xdg_surface_id &&
		  opcode == xdg_surface_configure_opcode) {
	    U32 serial = event_body[0];
	    if(xdg_surface_ack_configure(window->xdg_surface_id, serial)) {
	      acked_configure = 1;
	    }
	  }
	  else if(object_id == window->wl_display_id &&
		  opcode == wl_display_error_opcode) {
	    U32 error_object_id = event_body[0];
	    U32 error_code = event_body[1];
	    U32 error_string_count = event_body[2];
	    U8 *error_string = (U8 *)(event_body + 3);
	    // TODO: better logging
	    fprintf(stderr, "ERROR: object %u, code %u: %.*s\n",
		    error_object_id, error_code, (int)error_string_count, error_string);
	  }
	  else {
	    // TODO: better logging
	    fprintf(stderr, "unhandled message: object=%u, opcode=%u, length=%u\n", object_id, opcode, message_size);
	  }

	  events.mem += message_size;
	  events.size -= message_size;
	}
      }
	
      U64 shared_memory_size = width*height*sizeof(U32)*2;
      if(wayland_allocate_shared_memory(window, shared_memory_size)) {
	if(wayland_create_buffers(window, width, height)) {
	  // NOTE: initialization successful
	  SLLQueuePush(wayland_state.first_window, wayland_state.last_window, window);
	} else {
	  // NOTE: buffer creation failure
	  arena_pop_to(wayland_state.arena, wayland_arena_pre_init_pos);
	  window = 0;
	}
      } else {
	// NOTE: shared memory allocation failure
	arena_pop_to(wayland_state.arena, wayland_arena_pre_init_pos);
	window = 0;
      }
    } else {
      // NOTE: surface creation failure
      arena_pop_to(wayland_state.arena, wayland_arena_pre_init_pos);
      window = 0;
    }
  } else {
    // NOTE: registry creation failure
    arena_pop_to(wayland_state.arena, wayland_arena_pre_init_pos);
    window = 0;
  }
  
  return(window);
}

proc B32
wayland_get_event(WaylandWindow *window, WaylandEvent *event)
{
  B32 result = 1;
  if(!window->events_polled_this_frame) {
    window->frame_event_buffer = wayland_poll_events(window);
    window->events_polled_this_frame = 1;
  }

  if(window->frame_event_buffer.size) {
    WaylandMessageHeader *event_header = (WaylandMessageHeader *)window->frame_event_buffer.mem;
    event->object_id = event_header->object_id;
    event->opcode = event_header->opcode;
    event->body = (Buffer){.size = event_header->message_size - sizeof(WaylandMessageHeader),
			   .mem = (U8 *)(event_header + 1)};

    window->frame_event_buffer.mem += event_header->message_size;
    window->frame_event_buffer.size -= event_header->message_size;
  } else {
    result = 0;
  }

  return(result);
}

proc B32
wayland_swap_buffers(WaylandWindow *window)
{
  B32 result = 1;
  if(wl_surface_attach(window->wl_surface_id, window->wl_buffer_id, 0, 0)) {
    if(wl_surface_damage(window->wl_surface_id, 0, 0, window->width, window->height)) {
      if(wl_surface_commit(window->wl_surface_id)) {
	++window->frame_index;
	window->events_polled_this_frame = 0;
      } else {
	result = 0;
      }
    } else {
      result = 0;
    }
  } else {
    result = 0;
  }

  return(result);
}
