/** TODO:
 * support changing cursor icon
 * drag to resize
 * pull out common code in initialization event handling functions (or generate it)
 * (wip) improve event handling interface
 * log messages/errors somewhere besides the console
 * allow applications to use hardware rendering
 * explicit framebuffer and backbuffer
 */

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

proc void
event_list_push_ex(EventList *list, Event event, EventNode *node)
{
  node->event = event;
  SLLQueuePush(list->first, list->last, node);
  ++list->count;
}

proc void
event_list_push(Arena *arena, EventList *list, Event event)
{
  EventNode *node = arena_push_struct_z(arena, EventNode);
  event_list_push_ex(list, event, node);
}

proc U32
wayland_new_id(void) {
  Assert(wayland_state.next_id <= WAYLAND_MAX_CLIENT_OBJECT_ID);
  U32 result = wayland_state.next_id++;
  return(result);
}

proc WaylandTempId*
wayland_temp_id(void)
{
  WaylandTempId *result = 0;
  if(wayland_state.id_freelist) {
    result = wayland_state.id_freelist;
    SLLStackPop(wayland_state.id_freelist);
  } else {
    result = arena_push_struct(wayland_state.arena, WaylandTempId);
    result->id = wayland_new_id();
  }

  return(result);
}

proc void
wayland_release_id(WaylandTempId *id)
{
  SLLStackPush(wayland_state.id_freelist, id);
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
wayland_display_get_registry(void)
{
  U32 registry_id = wayland_new_id();
  B32 result = wl_display_get_registry(wayland_state.wl_display_id, registry_id);
  if(result) {
    wayland_state.wl_registry_id = registry_id;
  }
  
  return(result);
}

// TODO: all these initialization functions that poll for and handle events
//       should call a common routine. Or maybe generated routines?
proc B32
wayland_registry_bind_globals(void)
{
  ArenaTemp scratch = arena_get_scratch(0, 0);

  Buffer message_buffer = wayland_poll_events(scratch.arena);
  while(message_buffer.size) {
    WaylandMessageHeader *header = (WaylandMessageHeader *)message_buffer.mem;    
    U32 object_id = header->object_id;
    U32 opcode = header->opcode;
    U32 message_size = header->message_size;

    U32 *event_body = (U32*)(header + 1);

    if(object_id == wayland_state.wl_registry_id &&
       opcode == wl_registry_global_opcode) {
      U32 name = event_body[0];
      U32 interface_string_count = event_body[1];
      U8 *interface_string = (U8 *)(event_body + 2);
      U32 version = *(U32 *)(interface_string + AlignPow2(interface_string_count, 4));

      String8 message_interface = {.count = (U64)interface_string_count - 1, .string = interface_string};
      String8 wl_shm_str = Str8Lit("wl_shm");
      String8 xdg_wm_base_str = Str8Lit("xdg_wm_base");
      String8 wl_compositor_str = Str8Lit("wl_compositor");
      String8 wl_seat_str = Str8Lit("wl_seat");
      String8 zwp_linux_dmabuf_v1_str = Str8Lit("zwp_linux_dmabuf_v1");
      
      if(str8s_are_equal(message_interface, wl_shm_str)) {
	U32 shm_id = wayland_new_id();
	if(wl_registry_bind(wayland_state.wl_registry_id, name, wl_shm_str, version, shm_id)) {
	  wayland_state.wl_shm_id = shm_id;
	}
      }
      else if(str8s_are_equal(message_interface, xdg_wm_base_str)) {
	U32 wm_base_id = wayland_new_id();
	if(wl_registry_bind(wayland_state.wl_registry_id, name, xdg_wm_base_str, version, wm_base_id)) {
	  wayland_state.xdg_wm_base_id = wm_base_id;
	}
      }
      else if(str8s_are_equal(message_interface, wl_compositor_str)) {
	U32 compositor_id = wayland_new_id();
	if(wl_registry_bind(wayland_state.wl_registry_id, name, wl_compositor_str, version, compositor_id)) {
	  wayland_state.wl_compositor_id = compositor_id;
	}
      }
      else if(str8s_are_equal(message_interface, wl_seat_str)) {
	U32 seat_id = wayland_new_id();
	if(wl_registry_bind(wayland_state.wl_registry_id, name, wl_seat_str, version, seat_id)) {
	  wayland_state.wl_seat_id = seat_id;	      
	}
      }
      else if(str8s_are_equal(message_interface, zwp_linux_dmabuf_v1_str)) {
	U32 linux_dmabuf_id = wayland_new_id();
	if(wl_registry_bind(wayland_state.wl_registry_id, name, zwp_linux_dmabuf_v1_str, version, linux_dmabuf_id)) {
	  wayland_state.zwp_linux_dmabuf_v1_id = linux_dmabuf_id;
	}
      }
      else {
	fprintf(stderr, "unbound available global: name=%u, interface=%.*s, version=%u\n",
		name, (int)interface_string_count, interface_string, version);
      }
    }
    else {
      // TODO: better logging
      fprintf(stderr, "unhandled message(binding globals): object=%u, opcode=%u, length=%u\n",
	      object_id, opcode, message_size);
    }

    message_buffer.size -= message_size;
    message_buffer.mem += message_size;
  }

  B32 result = wayland_state.wl_shm_id && wayland_state.xdg_wm_base_id && wayland_state.wl_compositor_id && wayland_state.wl_seat_id && wayland_state.zwp_linux_dmabuf_v1_id;
  arena_release_scratch(scratch);
  return(result);
}

proc B32
wayland_get_capabilities(void)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  int dmabuf_format_table_fd = -1;
  U32 dmabuf_format_table_size = 0;

  Assert(wayland_state.sync_id);
  B32 sync_received = 0;
  while(!sync_received) {
    //Buffer message_buffer = wayland_poll_events(scratch.arena);
    U64 data_buffer_size = KB(4);
    U64 control_buffer_size = KB(1);
    U8 *data_buffer = arena_push_array_z(scratch.arena, U8, data_buffer_size);
    U8 *control_buffer = arena_push_array_z(scratch.arena, U8, control_buffer_size);

    struct iovec io = {.iov_base = data_buffer, .iov_len = data_buffer_size};
    struct msghdr socket_msg = {.msg_iov = &io, .msg_iovlen = 1,
				.msg_control = control_buffer, .msg_controllen = control_buffer_size};

    int bytes_received = recvmsg(wayland_state.display_socket_handle, &socket_msg, 0);
    if(bytes_received) {
      U32 bytes_remaining = bytes_received;
      while(bytes_remaining) {
	WaylandMessageHeader *header = (WaylandMessageHeader*)data_buffer;
	U32 object_id = header->object_id;
	U32 opcode = header->opcode;
	U32 message_size = header->message_size;

	U32 *body = (U32*)(header + 1);

	// TODO: get shm formats, seat capabilities
	if(object_id == wayland_state.wl_display_id &&
	   opcode == wl_display_error_opcode) {
	  U32 error_object_id = body[0];
	  U32 error_code = body[1];
	  U32 error_string_count = body[2];
	  U8 *error_string = (U8 *)(body + 3);
	  // TODO: better logging
	  fprintf(stderr, "ERROR: object %u, code %u: %.*s\n",
		  error_object_id, error_code, (int)error_string_count, error_string);
	}
	else if(object_id == wayland_state.sync_id->id &&
		opcode == wl_callback_done_opcode) {
	  sync_received = 1;
	}
	else if(object_id == wayland_state.zwp_linux_dmabuf_feedback_v1_id &&
		opcode == zwp_linux_dmabuf_feedback_v1_format_table_opcode) {
	  dmabuf_format_table_size = body[0];
	}	
	else {
	  fprintf(stderr, "unhandled message: object=%u, opcode=%u, length=%u\n", object_id, opcode, message_size);
	}

	bytes_remaining -= message_size;
	data_buffer += message_size;
      }

      for(struct cmsghdr *cmsg = CMSG_FIRSTHDR(&socket_msg); cmsg; cmsg = CMSG_NXTHDR(&socket_msg, cmsg)) {
	if(cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_RIGHTS) {
	  dmabuf_format_table_fd = *(int*)CMSG_DATA(cmsg);	  
	}
      }
    }
  }

  if(dmabuf_format_table_fd != -1) {
    if(dmabuf_format_table_size) {
      void *dmabuf_format_table = mmap(0, dmabuf_format_table_size,
				       PROT_READ, MAP_PRIVATE,
				       dmabuf_format_table_fd, 0);
      if(dmabuf_format_table != (void*)-1) {
	wayland_state.dmabuf_format_table = dmabuf_format_table;
	wayland_state.dmabuf_format_table_size = dmabuf_format_table_size;
	close(dmabuf_format_table_fd);
      }
    }
  }
  
  arena_release_scratch(scratch);
  return(result);
}

proc void
gl_debug_message_callback(GLenum src, GLenum type, GLuint id, GLenum severity, GLsizei len,
			  const GLchar *msg, const void *user_param)
{
  Unused(user_param);
  fprintf(stderr, "src=%u, type=%u, id=%u, severity=%u: %.*s\n", src, type, id, severity, len, msg);
}

proc B32
wayland_gl_init(void)
{
  B32 result = 1;

  EGLDisplay egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if(egl_display != EGL_NO_DISPLAY) {
    
    EGLint egl_major, egl_minor;
    if(eglInitialize(egl_display, &egl_major, &egl_minor)) {
      
      const char *egl_extensions_cstr = eglQueryString(egl_display, EGL_EXTENSIONS);
      fprintf(stderr, "egl extensions: %s\n", egl_extensions_cstr);

      String8 egl_extensions_str = Str8Cstr((char*)egl_extensions_cstr);
      if(str8_contains(egl_extensions_str, Str8Lit("EGL_MESA_image_dma_buf_export"))) {
	if(eglBindAPI(EGL_OPENGL_API)) {
	  
	  EGLint attrib_list[] = {EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT, EGL_NONE};
	  EGLConfig egl_config;
	  EGLint num_config;
	  if(eglChooseConfig(egl_display, attrib_list, &egl_config, 1, &num_config)) {
	  
	    EGLContext egl_context = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, 0);
	    if(egl_context != EGL_NO_CONTEXT) {
	      if(eglMakeCurrent(egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, egl_context)) {
		wayland_state.egl_display = egl_display;
		wayland_state.egl_context = egl_context;
	      } else {
		result = 0;
	      }
	    } else {
	      result = 0;
	    }
	  } else {
	    result = 0;
	  }
	} else {
	  result = 0;
	}
      } else {
	// NOTE: image dmabuf export extension not supported
	result = 0;
      }
    } else {
      // NOTE: egl not initialized
      result = 0;      
    }
  } else {
    // NOTE: no egl display
    result = 0;
  }
  
  return(result);
}

proc B32
wayland_init(void)
{
  ZeroStruct(&wayland_state, WaylandState);

  B32 result = wayland_display_connect();
  if(result) {
    wayland_state.arena = arena_alloc();
    wayland_state.wl_display_id = 1;
    wayland_state.next_id = 2;
    wayland_state.xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    
    if(wayland_display_get_registry()) {
      if(wayland_registry_bind_globals()) {
	
	U32 feedback_id = wayland_new_id();
	if(zwp_linux_dmabuf_v1_get_default_feedback(wayland_state.zwp_linux_dmabuf_v1_id, feedback_id)) {
	  wayland_state.zwp_linux_dmabuf_feedback_v1_id = feedback_id;

	  WaylandTempId *sync_id = wayland_temp_id();
	  if(wl_display_sync(wayland_state.wl_display_id, sync_id->id)) {
	    wayland_state.sync_id = sync_id;
	    
	    if(wayland_get_capabilities()) {
	      wayland_release_id(sync_id);
	      wayland_state.sync_id = 0;

	      if(wayland_gl_init()) {
		eglCreateImageKHR =
		  (PFNEGLCREATEIMAGEKHRPROC)eglGetProcAddress("eglCreateImageKHR");
		eglExportDMABUFImageQueryMESA =
		  (PFNEGLEXPORTDMABUFIMAGEQUERYMESAPROC)eglGetProcAddress("eglExportDMABUFImageQueryMESA");
		eglExportDMABUFImageMESA =
		  (PFNEGLEXPORTDMABUFIMAGEMESAPROC)eglGetProcAddress("eglExportDMABUFImageMESA");
	      } else {
		result = 0;
	      }
	    } else {
	      result = 0;
	    }
	  } else {
	    result = 0;
	  }
	} else {	  
	  result = 0;
	}
      } else {
	// NOTE: global binding failure
	result = 0;
      }    
    } else {
      // NOTE: registry creation failure
      result = 0;
    }
  } else {
    // NOTE: display
    result = 0;
  }

  if(result == 0 && wayland_state.arena != 0) {
    arena_release(wayland_state.arena);
  }

  return(result);
}


proc B32
wayland_initialize_input(WaylandWindow *window)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);   

  // TODO: It would be more proper to wait until we receive capabilities from
  //       the seat before we try getting keyboard and mouse. That might also
  //       prevent us from having to call recvmsg in a loop below
  U32 keyboard_id = wayland_new_id();
  if(wl_seat_get_keyboard(wayland_state.wl_seat_id, keyboard_id)) {
    window->wl_keyboard_id = keyboard_id;
  } else {
    result = 0;
  }

  U32 pointer_id = wayland_new_id();
  if(wl_seat_get_pointer(wayland_state.wl_seat_id, pointer_id)) {
    window->wl_pointer_id = pointer_id;
  } else {
    result = 0;
  }

  int keymap_fd = -1;
  U32 keymap_format = 0;
  U32 keymap_size = 0; 

  while(keymap_fd == -1) {
    fprintf(stderr, "DEBUG(wayland_initialize_input): attempting to receive keymap file descriptor...\n");

    U64 data_buffer_size = 256;
    U64 control_buffer_size = 256;
    U8 *data_buffer = arena_push_array_z(scratch.arena, U8, data_buffer_size);
    U8 *control_buffer = arena_push_array_z(scratch.arena, U8, control_buffer_size);

    struct iovec io = {.iov_base = data_buffer, .iov_len = data_buffer_size};
    struct msghdr socket_msg = {.msg_iov = &io, .msg_iovlen = 1,
				.msg_control = control_buffer, .msg_controllen = control_buffer_size};
  
    int bytes_received = recvmsg(wayland_state.display_socket_handle, &socket_msg, 0);
    if(bytes_received != -1) {    
      // NOTE: process regular data
      U32 bytes_remaining = bytes_received;
      while(bytes_remaining) {
	WaylandMessageHeader *event_header = (WaylandMessageHeader*)data_buffer;
	U32 object_id = event_header->object_id;
	U32 opcode = event_header->opcode;
	U32 message_size = event_header->message_size;
	Assert(message_size < data_buffer_size);
	U32 *event_body = (U32 *)(event_header + 1);

	// TODO: handle keyboard repeat info event
	if(object_id == window->wl_keyboard_id &&
	   opcode == wl_keyboard_keymap_opcode) {
	  keymap_format = event_body[0];
	  keymap_size = event_body[1];
	  fprintf(stderr, "%s (size=%u)\n", keymap_format ? "got xkb keymap" : "no keymap", keymap_size);
	} else {
	  fprintf(stderr, "unhandled message: object=%u, opcode=%u, length=%u\n", object_id, opcode, message_size);
	}

	bytes_remaining -= message_size;
	data_buffer += message_size;
      }
    
      // NOTE: process ancillary data    
      for(struct cmsghdr *cmsg = CMSG_FIRSTHDR(&socket_msg); cmsg; cmsg = CMSG_NXTHDR(&socket_msg, cmsg)) {
	fprintf(stderr, "DEBUG(wayland_initialize_input): recvmsg ancillary data loop iteration\n");
	if(cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_RIGHTS) {
	  fprintf(stderr, "DEBUG(wayland_initialize_input): reading file descriptor\n");
	  keymap_fd = *(int *)CMSG_DATA(cmsg);
	}
      }      
    }
    else {
      fprintf(stderr, "ERROR(wayland_initialize_input): recvmsg failed\n");
    }
  }

  if(keymap_fd != -1) {
    if(keymap_format) {
      char *keymap_shm = mmap(0, keymap_size, PROT_READ, MAP_SHARED, keymap_fd, 0);
      window->xkb_keymap = xkb_keymap_new_from_string(wayland_state.xkb_context,
						      keymap_shm,
						      XKB_KEYMAP_FORMAT_TEXT_V1,
						      XKB_KEYMAP_COMPILE_NO_FLAGS);
      window->xkb_state = xkb_state_new(window->xkb_keymap);
      munmap(keymap_shm, keymap_size);
      close(keymap_fd);
      fprintf(stderr, "got keymap\n");
    } else {
      fprintf(stderr, "ERROR: no xkb format\n");
    }
  } else {
    fprintf(stderr, "ERROR: no xkb keymap file descriptor\n");
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wayland_create_surface(WaylandWindow *window, String8 title)
{
  B32 result = 1;

  U32 wl_surface_id = wayland_new_id();
  if(wl_compositor_create_surface(wayland_state.wl_compositor_id, wl_surface_id)) {
    window->wl_surface_id = wl_surface_id;

    U32 xdg_surface_id = wayland_new_id();
    if(xdg_wm_base_get_xdg_surface(wayland_state.xdg_wm_base_id, xdg_surface_id, wl_surface_id)) {
      window->xdg_surface_id = xdg_surface_id;

      U32 toplevel_id = wayland_new_id();
      if(xdg_surface_get_toplevel(window->xdg_surface_id, toplevel_id)) {
	window->xdg_toplevel_id = toplevel_id;	
	
	if(!xdg_toplevel_set_title(window->xdg_toplevel_id, title)) {
	  fprintf(stderr, "ERROR: xdg_toplevel_set_title failed\n");
	}

	if(wl_surface_commit(window->wl_surface_id)) {
	  // TODO: better logging
	  fprintf(stderr, "(init) surface committed\n");
	} else {
	  // NOTE: surface commit failed
	  result = 0;
	}

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
	  window->shared_memory = shared_memory;
	  window->shared_memory_size = size;

	  U32 wl_shm_pool_id = wayland_new_id();
	  if(wl_shm_create_pool(wayland_state.wl_shm_id, wl_shm_pool_id, shared_memory_handle, size)) {
	    window->wl_shm_pool_id = wl_shm_pool_id;
	    close(shared_memory_handle);
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
wayland_create_buffer(WaylandWindow *window)
{
  B32 result = 1;

  S32 stride = window->width*sizeof(U32);
  U32 format = 0x34324241; // NOTE: ABGR 32-bit little-endian // TODO: generate enums
  
  Assert(!window->buffer_id);
  window->buffer_id = wayland_temp_id();
  if(wl_shm_pool_create_buffer(window->wl_shm_pool_id, window->buffer_id->id,
			       0, window->width, window->height, stride, format)) {
    // NOTE: success
  } else {
    // NOTE: buffer creation failed
    result = 0;
  }

  return(result);
}

proc B32
wayland_create_gl_buffer(WaylandWindow *window)
{
  B32 result = 1;

  ArenaTemp scratch = arena_get_scratch(0, 0);

  U32 params_id = wayland_new_id();
  if(zwp_linux_dmabuf_v1_create_params(wayland_state.zwp_linux_dmabuf_v1_id, params_id)) {
    window->zwp_linux_buffer_params_v1_id = params_id;

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(gl_debug_message_callback, 0);

    glEnable(GL_DEPTH_TEST);

    GLuint color_texture, depth_texture;
    glGenTextures(1, &color_texture);
    glGenTextures(1, &depth_texture);

    S32 buffer_width = 1920, buffer_height =  1080;
    glBindTexture(GL_TEXTURE_2D, color_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		 buffer_width, buffer_height,
		 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glBindTexture(GL_TEXTURE_2D, depth_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8,
		 buffer_width, buffer_height,
		 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);

    EGLImageKHR egl_image = eglCreateImageKHR(wayland_state.egl_display, wayland_state.egl_context,
					      EGL_GL_TEXTURE_2D, PtrFromInt(color_texture), 0);
    glFlush(); // NOTE: very important

    int num_planes, fourcc;
    EGLuint64KHR modifiers;
    if(eglExportDMABUFImageQueryMESA(wayland_state.egl_display, egl_image, &fourcc, &num_planes, &modifiers)) {
      Assert(num_planes == 1); // TODO: guarantee this somehow

      int fd;
      EGLint stride, offset;
      if(eglExportDMABUFImageMESA(wayland_state.egl_display, egl_image, &fd, &stride, &offset)) {
	// NOTE: success
	// TODO: add dmabuf (sending fd), create buffer
	U32 modifier_hi = modifiers >> 32;
	U32 modifier_lo = modifiers & U32_MAX;
	if(zwp_linux_buffer_params_v1_add(window->zwp_linux_buffer_params_v1_id,
					  fd, 0, offset, stride, modifier_hi, modifier_lo)) {

	  if(zwp_linux_buffer_params_v1_create(window->zwp_linux_buffer_params_v1_id,
					       buffer_width, buffer_height, fourcc, 0)) {
	    while(!window->gl_buffer_id) {
	      Buffer message_buffer = wayland_poll_events(scratch.arena);
	      while(message_buffer.size) {
		WaylandMessageHeader *header = (WaylandMessageHeader*)message_buffer.mem;
		U32 object_id = header->object_id;
		U32 opcode = header->opcode;
		U32 message_size = header->message_size;

		U32 *body = (U32*)(header + 1);
		if(object_id == window->zwp_linux_buffer_params_v1_id &&
		   opcode == zwp_linux_buffer_params_v1_created_opcode) {
		  window->gl_buffer_id = body[0];
		}
		else if(object_id == wayland_state.wl_display_id &&
			opcode == wl_display_error_opcode) {
		  U32 error_object_id = body[0];
		  U32 error_code = body[1];
		  U32 error_string_count = body[2];
		  U8 *error_string = (U8*)(body + 3);
		  fprintf(stderr, "ERROR: object %u, code %u: %.*s\n",
			  error_object_id, error_code, (int)error_string_count, error_string);
		}
		else {
		  fprintf(stderr, "unhandled message: object=%u, opcode=%u, length=%u\n",
			  object_id, opcode, message_size);
		}

		message_buffer.mem += message_size;
		message_buffer.size -= message_size;
	      }
	    }	      
	  }
	}
      } else {
	result = 0;
      }
    } else {
      result = 0;
    }
  } else {
    result = 0;
  }

  arena_release_scratch(scratch);
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
wayland_poll_events(Arena *arena)
{
  Buffer result = {0};
  result.size = KB(4);
  result.mem = arena_push_array_z(arena, U8, result.size);

  S64 recv_size = recv(wayland_state.display_socket_handle,
		       result.mem, result.size, 0);
  if(recv_size != -1) {
    result.size = recv_size;
  } else {
    arena_pop(arena, result.size);
    result.size = 0;
    result.mem = 0;
    wayland_log_error("recv failed: %s", strerror(errno));
  }

  return(result);
}

proc WaylandWindow*
wayland_open_window(String8 title, S32 width, S32 height)
{
  ArenaTemp scratch = arena_get_scratch(0, 0);
  
  U64 wayland_arena_pre_init_pos = arena_pos(wayland_state.arena);
  
  WaylandWindow *window = arena_push_struct(wayland_state.arena, WaylandWindow);
    
  if(wayland_initialize_input(window)) {
    if(wayland_create_surface(window, title)) {
      // TODO: pull out into another function?
      B32 acked_configure = 0;
      while(!acked_configure) {
	Buffer events = wayland_poll_events(scratch.arena);
	while(events.size) {
	  WaylandMessageHeader *event_header = (WaylandMessageHeader *)events.mem;
	  U32 object_id = event_header->object_id;
	  U32 opcode = event_header->opcode;
	  U32 message_size = event_header->message_size;
	  U32 *event_body = (U32 *)(event_header + 1);

	  if(object_id == wayland_state.xdg_wm_base_id &&
	     opcode == xdg_wm_base_ping_opcode) {
	    U32 serial = event_body[0];
	    xdg_wm_base_pong(wayland_state.xdg_wm_base_id, serial);
	  }
	  else if(object_id == window->xdg_surface_id &&
		  opcode == xdg_surface_configure_opcode) {
	    U32 serial = event_body[0];
	    if(xdg_surface_ack_configure(window->xdg_surface_id, serial)) {
	      acked_configure = 1;
	    }
	  }
	  else if(object_id == wayland_state.wl_display_id &&
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
	
      U64 shared_memory_size = 1960*1080*sizeof(U32)*2;
      if(wayland_allocate_shared_memory(window, shared_memory_size)) {
	window->event_arena = arena_alloc();
	    
	window->width = width;
	window->height = height;
	if(wayland_create_gl_buffer(window)) {
	  if(wayland_end_frame(window)) {
	    // NOTE: initialization successful
	    SLLQueuePush(wayland_state.first_window, wayland_state.last_window, window);
	  } else {
	    // NOTE: failure to create buffer or commit surface
	    arena_pop_to(wayland_state.arena, wayland_arena_pre_init_pos);
	    window = 0;
	  }
	} else {
	  // NOTE: failure to create dmabuf
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
    // NOTE: input initialization failure
    arena_pop_to(wayland_state.arena, wayland_arena_pre_init_pos);
    window = 0;
  }
   
  arena_release_scratch(scratch);
  return(window);
}

proc EventList
wayland_get_events(WaylandWindow *window)
{
  EventList result = {0};

  // NOTE: if we don't have a callback id, then we don't need to wait for it
  // TODO: we should probably still poll events, even if we don't have a callback

  ArenaTemp scratch = arena_get_scratch(0, 0);

  // TODO this infinite loop is a bit jank
  while(window->frame_callback_id) {
    Buffer message_buffer = wayland_poll_events(scratch.arena);
    while(message_buffer.size) {
      Event event = {0};
      
      WaylandMessageHeader *header = (WaylandMessageHeader*)message_buffer.mem;      
      U32 object_id = header->object_id;
      U32 opcode = header->opcode;
      U32 size = header->message_size;      
      U32 *body = (U32*)(header + 1);

      // NOTE: acknowledge ping
      if(object_id == wayland_state.xdg_wm_base_id &&
	 opcode == xdg_wm_base_ping_opcode) {
	U32 serial = body[0];
	if(xdg_wm_base_pong(wayland_state.xdg_wm_base_id, serial)) {
	  fprintf(stderr, "**acked ping**\n");
	}
      }
      else if(object_id == window->xdg_surface_id &&
	      opcode == xdg_surface_configure_opcode) {
	U32 serial = body[0];
	if(xdg_surface_ack_configure(window->xdg_surface_id, serial)) {
	  fprintf(stderr, "**acked configure**\n");
	}
      }
      else if(object_id == window->xdg_toplevel_id &&
	      opcode == xdg_toplevel_close_opcode) {
	event.kind = EventKind_Close;
	event_list_push(window->event_arena, &result, event);
      }
      else if(object_id == window->xdg_toplevel_id &&
	      opcode == xdg_toplevel_configure_opcode) {
	S32 width = *(S32*)(body + 0);
	S32 height = *(S32*)(body + 1);
	BufferU32 states = {.count = body[2]/sizeof(U32), .mem = body + 3};

	window->width = width;
	window->height = height;
	fprintf(stderr, "xdg_toplevel configured: width=%u, height=%u, %lu states\n",
		width, height, states.count);
      }
      else if(window->frame_callback_id &&
	      object_id == window->frame_callback_id->id &&
	      opcode == wl_callback_done_opcode) {
	U32 data = body[0];
	window->last_frame_ms_elapsed = data - window->last_frame_timestamp;
	window->last_frame_timestamp = data;

	//SLLStackPush(wayland_state.id_freelist, window->frame_callback_id);
	wayland_release_id(window->frame_callback_id);
	window->frame_callback_id = 0;
      }
      else if(window->buffer_id &&
	      object_id == window->buffer_id->id &&
	      opcode == wl_buffer_release_opcode) {
	wayland_release_id(window->buffer_id);
	wl_buffer_destroy(window->buffer_id->id);
	window->buffer_id = 0;
      }
      // NOTE: mouse events
      else if(object_id == window->wl_pointer_id &&
	      opcode == wl_pointer_motion_opcode) {
	U32 time = body[0];
	U32 surface_x__fixed = body[1];
	U32 surface_y__fixed = body[2];
	R32 surface_x = (R32)surface_x__fixed / 256.f;
	R32 surface_y = (R32)surface_y__fixed / 256.f;
	Unused(time);

	event.kind = EventKind_Move;
	event.position = (V2){.x = surface_x, .y = surface_y};
	event_list_push(window->event_arena, &result, event);
      }
      else if(object_id == window->wl_pointer_id &&
	      opcode == wl_pointer_button_opcode) {
	U32 serial = body[0];
	U32 time = body[1];
	U32 button = body[2];
	U32 state = body[3];
	Unused(serial);
	Unused(time);

	event.kind = state ? EventKind_Press : EventKind_Release;
	event.button = button; // TODO: this might be wrong
	event_list_push(window->event_arena, &result, event);
      }
      // NOTE: keyboard events
      else if(object_id == window->wl_keyboard_id &&
	      opcode == wl_keyboard_key_opcode) {
	U32 serial = body[0];
	U32 time = body[1];
	U32 key = body[2];
	U32 state = body[3];

	if(window->xkb_state != 0 && window->xkb_keymap != 0) {
	  U32 keycode = key + 8;
	  xkb_keysym_t keysym = xkb_state_key_get_one_sym(window->xkb_state, keycode);
	  /* U64 key_name_buffer_size = 128; */
	  /* U8 *key_name_buffer = arena_push_array_z(window->event_arena, U8, 128); */
	  /* int key_name_length = xkb_keysym_get_name(keysym, (char *)key_name_buffer, key_name_buffer_size); */
	  /* String8 key_name = {.string = key_name_buffer, .count = key_name_length}; */
	  Unused(serial);
	  Unused(time);

	  event.kind = state ? EventKind_Press : EventKind_Release;
	  event.button = keysym; // TODO: this might be wrong
	  event_list_push(window->event_arena, &result, event);

	  /* fprintf(stderr, "key event: time=%u, serial=%u, key=%u(%s), state=%s\n", */
	  /* 	time, serial, key, (char *)key_name_buffer, state ? "pressed" : "released"); */
	} else {
	  fprintf(stderr, "FUCK: where is my xkb???\n");
	}
      }    
      // NOTE: errors
      else if(object_id == wayland_state.wl_display_id &&
	      opcode == wl_display_error_opcode) {
	U32 error_object_id = body[0];
	U32 error_code = body[1];
	U32 error_string_count = body[2];
	U8 *error_string = (U8 *)(body + 3);
	// TODO: better logging
	fprintf(stderr, "ERROR: object %u, code %u: %.*s\n",
		error_object_id, error_code, (int)error_string_count, error_string);
      }
      else {
	// TODO: better logging
	fprintf(stderr, "unhandled message: object=%u, opcode=%u, length=%u\n",
		object_id, opcode, size);
      }

      message_buffer.size -= size;
      message_buffer.mem += size;
    }
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
next_event(EventList *events, Event *event)
{
  B32 result = 0;
  if(events->count) {
    *event = events->first->event;
    SLLQueuePop(events->first, events->last);
    --events->count;
      
    result = 1;
  } 

  return result;
}

proc B32
wayland_end_frame(WaylandWindow *window)
{
  B32 result = 1;

  Assert(!window->frame_callback_id);
  if(wayland_create_buffer(window)) {
    if(wl_surface_attach(window->wl_surface_id, window->buffer_id->id, 0, 0)) {
      if(wl_surface_damage(window->wl_surface_id, 0, 0, window->width, window->height)) {
	
	window->frame_callback_id = wayland_temp_id();
	if(wl_surface_frame(window->wl_surface_id, window->frame_callback_id->id)) {
	  if(wl_surface_commit(window->wl_surface_id)) {
	    ++window->frame_index;
	    arena_clear(window->event_arena);
	    fprintf(stderr, "(render) surface committed\n");
	  } else {
	    result = 0;
	  }
	} else {
	  result = 0;
	}
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

proc void
wayland_close_window(WaylandWindow *window)
{
  wl_shm_pool_destroy(window->wl_shm_pool_id);  
  xdg_toplevel_destroy(window->xdg_toplevel_id);  
  xdg_surface_destroy(window->xdg_surface_id);  
  wl_surface_destroy(window->wl_surface_id); 

  munmap(window->shared_memory, window->shared_memory_size);  

  xkb_keymap_unref(window->xkb_keymap);
  xkb_state_unref(window->xkb_state);
}
