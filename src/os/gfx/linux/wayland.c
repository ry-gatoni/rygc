/** TODO:
 * support changing cursor icon
 * drag to resize
 * pull out common code in initialization event handling functions (or generate it)
 * (wip) improve event handling interface
 * log messages/errors somewhere besides the console
 */

//#include "OpenGL/ogl.c"
#include "wayland_generated.c"

proc WaylandId*
wayland_new_id(WaylandWindow *window) {
  Assert(wayland_state->next_id <= WAYLAND_MAX_CLIENT_OBJECT_ID);
  U32 id = wayland_state->next_id++;
  
  WaylandId *result = wayland_state->id_freelist;
  if(result) {
    SLLStackPop(wayland_state->id_freelist);
  }else {
    result = arena_push_struct(wayland_state->arena, WaylandId);
  }
  Assert(result);

  result->window = window;
  result->id = id;
  return(result);
}

#if 0
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
#endif

proc void
wayland_release_id(WaylandId *id)
{
  SLLStackPush(wayland_state->id_freelist, id);
}

proc WaylandId*
wayland_id_from_list(WaylandId *ids, U32 id)
{
  WaylandId *result = 0;
  for(WaylandId *node = ids; node; node = node->next) {
    if(node->id == id) {
      result = node;
      break;
    }
  }
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
	  wayland_state->display_socket_handle = socket_handle;
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
  WaylandId *registry = wayland_new_id(0);
  B32 result = wl_display_get_registry(wayland_state->wl_display->id, registry->id);
  if(result) {
    wayland_state->wl_registry = registry;
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

    if(object_id == wayland_state->wl_registry->id &&
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
	WaylandId *shm = wayland_new_id(0);
	if(wl_registry_bind(wayland_state->wl_registry->id, name, wl_shm_str, version, shm->id)) {
	  wayland_state->wl_shm = shm;
	}
      }
      else if(str8s_are_equal(message_interface, xdg_wm_base_str)) {
	WaylandId *wm_base = wayland_new_id(0);
	if(wl_registry_bind(wayland_state->wl_registry->id, name, xdg_wm_base_str, version, wm_base->id)) {
	  wayland_state->xdg_wm_base = wm_base;
	}
      }
      else if(str8s_are_equal(message_interface, wl_compositor_str)) {
	WaylandId *compositor = wayland_new_id(0);
	if(wl_registry_bind(wayland_state->wl_registry->id, name, wl_compositor_str, version, compositor->id)) {
	  wayland_state->wl_compositor = compositor;
	}
      }
      else if(str8s_are_equal(message_interface, wl_seat_str)) {
	WaylandId *seat = wayland_new_id(0);
	if(wl_registry_bind(wayland_state->wl_registry->id, name, wl_seat_str, version, seat->id)) {
	  wayland_state->wl_seat = seat;	      
	}
      }
      else if(str8s_are_equal(message_interface, zwp_linux_dmabuf_v1_str)) {
	WaylandId *linux_dmabuf = wayland_new_id(0);
	if(wl_registry_bind(wayland_state->wl_registry->id, name, zwp_linux_dmabuf_v1_str, version, linux_dmabuf->id)) {
	  wayland_state->zwp_linux_dmabuf_v1 = linux_dmabuf;
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

  B32 result = (wayland_state->wl_shm &&
		wayland_state->xdg_wm_base &&
		wayland_state->wl_compositor &&
		wayland_state->wl_seat &&
		wayland_state->zwp_linux_dmabuf_v1);
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

  Assert(wayland_state->sync);
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

    int bytes_received = recvmsg(wayland_state->display_socket_handle, &socket_msg, 0);
    if(bytes_received) {
      U32 bytes_remaining = bytes_received;
      while(bytes_remaining) {
	WaylandMessageHeader *header = (WaylandMessageHeader*)data_buffer;
	U32 object_id = header->object_id;
	U32 opcode = header->opcode;
	U32 message_size = header->message_size;

	U32 *body = (U32*)(header + 1);

	// TODO: get shm formats, seat capabilities
	if(object_id == wayland_state->wl_display->id &&
	   opcode == wl_display_error_opcode) {
	  U32 error_object_id = body[0];
	  U32 error_code = body[1];
	  U32 error_string_count = body[2];
	  U8 *error_string = (U8 *)(body + 3);
	  // TODO: better logging
	  fprintf(stderr, "ERROR: object %u, code %u: %.*s\n",
		  error_object_id, error_code, (int)error_string_count, error_string);
	}
	else if(object_id == wayland_state->sync->id &&
		opcode == wl_callback_done_opcode) {
	  sync_received = 1;
	}
	else if(object_id == wayland_state->zwp_linux_dmabuf_feedback_v1->id &&
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
	wayland_state->dmabuf_format_table = dmabuf_format_table;
	wayland_state->dmabuf_format_table_size = dmabuf_format_table_size;
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
		wayland_state->egl_display = egl_display;
		wayland_state->egl_context = egl_context;
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
  Arena *arena	= arena_alloc();
  wayland_state = arena_push_struct(arena, WaylandState);
  if(wayland_state == 0) goto wayland_state_alloc_failure;

  wayland_state->arena = arena;
  wayland_state->next_id = 1;
  wayland_state->wl_display = wayland_new_id(0);
  wayland_state->xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);

  if(!wayland_display_connect()) goto wayland_display_connect_failure;
  if(!wayland_display_get_registry()) goto wayland_display_get_registry_failure;
  if(!wayland_registry_bind_globals()) goto wayland_registry_bind_globals_failure;
  // TODO: dmabuf feedback
  if(!wayland_get_capabilities()) goto wayland_get_capabilities_failure;
  if(!wayland_initialize_input()) goto wayland_initialize_input_failure;
  if(!wayland_gl_init()) goto wayland_gl_init_failure;

  if(0)
  {
  wayland_gl_init_failure:
  wayland_initialize_input_failure:
  wayland_get_capabilities_failure:
  wayland_registry_bind_globals_failure:
  wayland_display_get_registry_failure:
  wayland_display_connect_failure:
  wayland_state_alloc_failure:
    arena_release(arena);
    return(0);
  }
  return(1);

#if 0
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
#endif
}


proc B32
wayland_initialize_input()
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);   

  // TODO: It would be more proper to wait until we receive capabilities from
  //       the seat before we try getting keyboard and mouse. That might also
  //       prevent us from having to call recvmsg in a loop below
  WaylandId *keyboard = wayland_new_id(0);
  if(wl_seat_get_keyboard(wayland_state->wl_seat->id, keyboard->id)) {
    wayland_state->wl_keyboard = keyboard;
  } else {
    result = 0;
  }

  WaylandId *pointer = wayland_new_id(0);
  if(wl_seat_get_pointer(wayland_state->wl_seat->id, pointer->id)) {
    wayland_state->wl_pointer = pointer;
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
  
    int bytes_received = recvmsg(wayland_state->display_socket_handle, &socket_msg, 0);
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
	if(object_id == wayland_state->wl_keyboard->id &&
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
      wayland_state->xkb_keymap =
	xkb_keymap_new_from_string(wayland_state->xkb_context,
				   keymap_shm,
				   XKB_KEYMAP_FORMAT_TEXT_V1,
				   XKB_KEYMAP_COMPILE_NO_FLAGS);
      wayland_state->xkb_state = xkb_state_new(wayland_state->xkb_keymap);
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

  WaylandId *wl_surface = wayland_new_id(window);
  if(wl_compositor_create_surface(wayland_state->wl_compositor->id, wl_surface->id)) {
    window->wl_surface = wl_surface;

    WaylandId *xdg_surface = wayland_new_id(window);
    if(xdg_wm_base_get_xdg_surface(wayland_state->xdg_wm_base->id, xdg_surface->id, wl_surface->id)) {
      window->xdg_surface = xdg_surface;

      WaylandId *toplevel = wayland_new_id(window);
      if(xdg_surface_get_toplevel(window->xdg_surface->id, toplevel->id)) {
	window->xdg_toplevel = toplevel;	
	
	if(!xdg_toplevel_set_title(window->xdg_toplevel->id, title)) {
	  fprintf(stderr, "ERROR: xdg_toplevel_set_title failed\n");
	}

	// NOTE: flip y 
	if(wl_surface_set_buffer_transform(window->wl_surface->id, 6)) {
	  if(wl_surface_commit(window->wl_surface->id)) {
	    // TODO: better logging
	    fprintf(stderr, "(init) surface committed\n");
	  } else {
	    // NOTE: surface commit failed
	    result = 0;
	  }
	} else {
	  // NOTE: set buffer transform failed
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

	  WaylandId *wl_shm_pool = wayland_new_id(window);
	  if(wl_shm_create_pool(wayland_state->wl_shm->id, wl_shm_pool->id, shared_memory_handle, size)) {
	    window->wl_shm_pool = wl_shm_pool;
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
wayland_create_shm_buffer(WaylandWindow *window)
{
  B32 result = 1;

  S32 stride = window->width*sizeof(U32);
  U32 format = 0x34324241; // NOTE: ABGR 32-bit little-endian // TODO: generate enums
  
  //Assert(!window->buffer_id[window->backbuffer_index]);
  /* WaylandTempId *buffer_id = wayland_temp_id(); */
  WaylandId *buffer = wayland_new_id(window);
  if(wl_shm_pool_create_buffer(window->wl_shm_pool->id, buffer->id,
			       window->backbuffer_index*window->shared_memory_size/2,
			       window->width, window->height, stride, format)) {
    window->buffers[window->backbuffer_index] = buffer;
  } else {
    // NOTE: buffer creation failed
    result = 0;
  }

  return(result);
}

proc B32
wayland_allocate_textures(WaylandWindow *window)
{
  B32 result = 1;
  
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(gl_debug_message_callback, 0);

  glEnable(GL_DEPTH_TEST);

  S32 texture_width = 1920, texture_height =  1080;

  for(U32 i = 0; i < ArrayCount(window->gl_framebuffer) && result; ++i) {
    GLuint color_texture, depth_texture;
    glGenTextures(1, &color_texture);
    glGenTextures(1, &depth_texture);
  
    glBindTexture(GL_TEXTURE_2D, color_texture);
    GlTextureDefaultParams();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		 texture_width, texture_height,
		 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glBindTexture(GL_TEXTURE_2D, depth_texture);
    GlTextureDefaultParams();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8,
		 texture_width, texture_height,
		 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);

    window->gl_framebuffer[i].color_texture = color_texture;
    window->gl_framebuffer[i].depth_texture = depth_texture;
    window->gl_framebuffer[i].fbo = fbo;
    window->gl_framebuffer[i].width = texture_width;
    window->gl_framebuffer[i].height = texture_height;
    
    EGLImage egl_image = eglCreateImageKHR(wayland_state->egl_display, wayland_state->egl_context,
					   EGL_GL_TEXTURE_2D, PtrFromInt(color_texture), 0);

    int num_planes, fourcc;
    EGLuint64KHR modifiers;
    if(eglExportDMABUFImageQueryMESA(wayland_state->egl_display, egl_image, &fourcc, &num_planes, &modifiers)) {
      Assert(num_planes == 1); // TODO: guarantee this somehow

      int fd;
      EGLint stride, offset;
      if(eglExportDMABUFImageMESA(wayland_state->egl_display, egl_image, &fd, &stride, &offset)) {
	U32 modifier_hi = modifiers >> 32;
	U32 modifier_lo = modifiers & U32_MAX;

	window->gl_framebuffer[i].egl_image = egl_image;
	window->gl_framebuffer[i].num_planes = num_planes;
	window->gl_framebuffer[i].fourcc = fourcc;
	window->gl_framebuffer[i].fd = fd;
	window->gl_framebuffer[i].stride = stride;
	window->gl_framebuffer[i].offset = offset;
	window->gl_framebuffer[i].modifier_hi = modifier_hi;
	window->gl_framebuffer[i].modifier_lo = modifier_lo;
      } else {
	result = 0;
      }
    } else {
      result = 0;
    }
  }
  
  glFlush(); // NOTE: avoid some bugs on some drivers

  return(result);
}

proc B32
wayland_create_gl_buffer(WaylandWindow *window)
{
  B32 result = 1;

  ArenaTemp scratch = arena_get_scratch(0, 0);

  WaylandId *params = wayland_new_id(window);
  if(zwp_linux_dmabuf_v1_create_params(wayland_state->zwp_linux_dmabuf_v1->id, params->id)) {
    window->zwp_linux_buffer_params_v1 = params;        

    GlFramebuffer gl_framebuffer = window->gl_framebuffer[window->backbuffer_index];
    if(zwp_linux_buffer_params_v1_add(window->zwp_linux_buffer_params_v1->id,
				      gl_framebuffer.fd,
				      0,
				      gl_framebuffer.offset,
				      gl_framebuffer.stride,
				      gl_framebuffer.modifier_hi,
				      gl_framebuffer.modifier_lo)) {

      /* WaylandTempId *buffer_id = wayland_temp_id(); */
      WaylandId *buffer = wayland_new_id(window);
      if(zwp_linux_buffer_params_v1_create_immed(window->zwp_linux_buffer_params_v1->id, buffer->id,
						 window->width, window->height, gl_framebuffer.fourcc, 0)) {
	window->buffers[window->backbuffer_index] = buffer; 
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

proc B32
wayland_create_buffer(WaylandWindow *window)
{
  B32 result = 1;
  if(window->render_target == RenderTarget_software) {
    result = wayland_create_shm_buffer(window);
  } else if(window->render_target == RenderTarget_hardware) {
    result = wayland_create_gl_buffer(window);
  }

  return(result);
}

proc void
wayland_log_error_(char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  str8_list_push_fv(wayland_state->arena, &wayland_state->error_list, fmt, args);
  va_end(args);
}

proc Buffer
wayland_poll_events(Arena *arena)
{
  Buffer result = {0};
  result.size = KB(4);
  result.mem = arena_push_array_z(arena, U8, result.size);

  S64 recv_size = recv(wayland_state->display_socket_handle,
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
wayland_open_window(String8 title, S32 width, S32 height, RenderTarget render_target)
{
  ArenaTemp scratch = arena_get_scratch(0, 0);
  
  U64 wayland_arena_pre_init_pos = arena_pos(wayland_state->arena);
  
  WaylandWindow *window = arena_push_struct(wayland_state->arena, WaylandWindow);
    
  if(wayland_create_surface(window, title)) {
    // TODO: pull out into another function?
    B32 acked_configure = 0;
    while(!acked_configure) {
      Buffer events = wayland_poll_events(scratch.arena);
      while(events.size) {
	WaylandMessageHeader *event_header = buf_consume_struct(&events, WaylandMessageHeader);//(WaylandMessageHeader *)events.mem;
	U32 object_id = event_header->object_id;
	U32 opcode = event_header->opcode;
	U32 message_size = event_header->message_size;
	U32 message_body_u32_count = (message_size - sizeof(WaylandMessageHeader)) / 4;
	U32 *event_body = buf_consume_array(&events, U32, message_body_u32_count);//(event_header + 1);

	if(object_id == wayland_state->xdg_wm_base->id &&
	   opcode == xdg_wm_base_ping_opcode) {
	  U32 serial = event_body[0];
	  xdg_wm_base_pong(wayland_state->xdg_wm_base->id, serial);
	}
	else if(object_id == window->xdg_surface->id &&
		opcode == xdg_surface_configure_opcode) {
	  U32 serial = event_body[0];
	  if(xdg_surface_ack_configure(window->xdg_surface->id, serial)) {
	    acked_configure = 1;
	  }
	}
	else if(object_id == wayland_state->wl_display->id &&
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

	/* events.mem += message_size; */
	/* events.size -= message_size; */
      }
    }
	
    U64 shared_memory_size = 1960*1080*sizeof(U32)*2;
    if(wayland_allocate_shared_memory(window, shared_memory_size)) {
      if(wayland_allocate_textures(window)) {
	//window->event_arena = arena_alloc();
	    
	window->width = width;
	window->height = height;
	window->render_target = render_target;
	if(wayland_end_frame(window)) {
	  // NOTE: initialization successful
	  DLLPushBack(wayland_state->first_window, wayland_state->last_window, window);
	} else {
	  // NOTE: failure to create buffer or commit surface
	  arena_pop_to(wayland_state->arena, wayland_arena_pre_init_pos);
	  window = 0;
	}
      } else {
	// NOTE: gl texture creation failure
	arena_pop_to(wayland_state->arena, wayland_arena_pre_init_pos);
	window = 0;
      }
    } else {
      // NOTE: shared memory allocation failure
      arena_pop_to(wayland_state->arena, wayland_arena_pre_init_pos);
      window = 0;
    }
  } else {
    // NOTE: surface creation failure
    arena_pop_to(wayland_state->arena, wayland_arena_pre_init_pos);
    window = 0;
  }
   
  arena_release_scratch(scratch);
  return(window);
}

proc U32*
wayland_get_render_pixels(WaylandWindow *window)
{
  U32 *result = (U32*)((U8*)window->shared_memory + window->backbuffer_index*window->shared_memory_size/2);
  return(result);
}

proc GlFramebuffer
wayland_get_framebuffer(WaylandWindow *window)
{
  GlFramebuffer result = window->gl_framebuffer[window->backbuffer_index];
  return(result);
}

#if 0
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
      else if(window->buffer_id[window->backbuffer_index] &&
	      object_id == window->buffer_id[window->backbuffer_index]->id &&
	      opcode == wl_buffer_release_opcode) {
	wayland_release_id(window->buffer_id[window->backbuffer_index]);
	wl_buffer_destroy(window->buffer_id[window->backbuffer_index]->id);
	window->buffer_id[window->backbuffer_index] = 0;
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
#endif

proc B32
wayland_end_frame(WaylandWindow *window)
{
  B32 result = 1;

  Assert(!window->frame_callback);
  if(wayland_create_buffer(window)) {
    if(wl_surface_attach(window->wl_surface->id, window->buffers[window->backbuffer_index]->id, 0, 0)) {
      if(wl_surface_damage(window->wl_surface->id, 0, 0, window->width, window->height)) {
	
	//window->frame_callback_id = wayland_temp_id();
	WaylandId *frame_callback = wayland_new_id(window);
	if(wl_surface_frame(window->wl_surface->id, frame_callback->id)) {
	  window->frame_callback = frame_callback;
	  DLLPushBack(wayland_state->first_frame_callback, wayland_state->last_frame_callback, frame_callback);
	  ++wayland_state->frame_callback_count;
	  
	  if(wl_surface_commit(window->wl_surface->id)) {
	    ++window->frame_index;
	    
	    ++window->backbuffer_index;
	    window->backbuffer_index %= ArrayCount(window->buffers);

	    //arena_clear(window->event_arena);
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
  wl_shm_pool_destroy(window->wl_shm_pool->id);
  wayland_release_id(window->wl_shm_pool);

  xdg_toplevel_destroy(window->xdg_toplevel->id);
  wayland_release_id(window->xdg_toplevel);

  xdg_surface_destroy(window->xdg_surface->id);
  wayland_release_id(window->xdg_surface);

  wl_surface_destroy(window->wl_surface->id);
  wayland_release_id(window->wl_surface);

  zwp_linux_buffer_params_v1_destroy(window->zwp_linux_buffer_params_v1->id);
  wayland_release_id(window->zwp_linux_buffer_params_v1);

  munmap(window->shared_memory, window->shared_memory_size);  

  /* xkb_keymap_unref(window->xkb_keymap); */
  /* xkb_state_unref(window->xkb_state); */

  for(U32 i = 0; i < ArrayCount(window->gl_framebuffer); ++i) {
    glDeleteTextures(1, &window->gl_framebuffer[i].color_texture);
    glDeleteTextures(1, &window->gl_framebuffer[i].depth_texture);
    glDeleteFramebuffers(1, &window->gl_framebuffer[i].fbo);
  }

  ZeroStruct(window);
}

// NOTE: general interface
proc B32
os_gfx_init(void)
{
  B32 result = wayland_init();
  return(result);
}

proc Os_Handle
os_open_window(String8 name, S32 width, S32 height)
{
  Os_Handle result = {0};
  result.handle = wayland_open_window(name, width, height, RenderTarget_hardware);
  return(result);
}

proc void
os_close_window(Os_Handle window)
{
  wayland_close_window(window.handle);
}

#if 1
proc Os_EventList
os_events(Arena *arena)
{
  Os_EventList result = {0};

  ArenaTemp scratch = arena_get_scratch(&arena, 1);

  U32 counter = 0;
  U32 max_count = 100;
  while(wayland_state->frame_callback_count > 0 && counter < max_count) {
    ++counter;
    
    Buffer msgs = wayland_poll_events(scratch.arena);
    while(msgs.size) {

      Os_Event *event = 0;
      
      WaylandMessageHeader *header = buf_consume_struct(&msgs, WaylandMessageHeader);
      U32 object_id = header->object_id;
      U32 opcode = header->opcode;
      U32 size = header->message_size;
      //U32 *body = buf_consume_array(&msgs, U32, message_body_u32_count);

      U32 body_size = size - sizeof(WaylandMessageHeader);
      Buffer body = {.size = body_size, .mem = buf_consume_array(&msgs, U8, body_size)};

      // NOTE: check if message id is a global id
      if(object_id == wayland_state->xdg_wm_base->id)
      {
	if(opcode == xdg_wm_base_pong_opcode)
	{
	  U32 serial = *buf_consume_struct(&body, U32);
	  if(!xdg_wm_base_pong(wayland_state->xdg_wm_base->id, serial)) {
	    // TODO: log error
	  }
	}
	else
	{
	  // TODO: unhandled opcode
	}
      }
      else if(object_id == wayland_state->wl_pointer->id) {
	if(opcode == wl_pointer_motion_opcode) {
	  U32 time = *buf_consume_struct(&body, U32);
	  U32 surface_x__fixed = *buf_consume_struct(&body, U32);
	  U32 surface_y__fixed = *buf_consume_struct(&body, U32);
	  Unused(time);

	  R32 surface_x = (R32)surface_x__fixed / 256.f;
	  R32 surface_y = (R32)surface_y__fixed / 256.f;
	  event = os_event_list_push_new(arena, &result, Os_EventKind_move);
	  event->pos = v2(surface_x, surface_y);
	}
	else if(opcode == wl_pointer_button_opcode) {
	  U32 serial = *buf_consume_struct(&body, U32);
	  U32 time = *buf_consume_struct(&body, U32);
	  U32 button = *buf_consume_struct(&body, U32);
	  U32 state = *buf_consume_struct(&body, U32);
	  Unused(serial);
	  Unused(time);

	  event = os_event_list_push_new(arena, &result,
					 state ? Os_EventKind_press : Os_EventKind_release);
	  Wayland_Key wayland_key = Wayland_Key_mouse_left + (button ^ WAYLAND_BUTTON_MOUSE_MASK);
	  event->key = os_key_from_wayland_key[wayland_key];
	}
	else if(opcode == wl_pointer_enter_opcode) {
	  U32 serial = *buf_consume_struct(&body, U32);
	  U32 wl_surface_id = *buf_consume_struct(&body, U32);
	  U32 surface_x__fixed = *buf_consume_struct(&body, U32);
	  U32 surface_y__fixed = *buf_consume_struct(&body, U32);
	  Unused(serial);
	  
	  R32 surface_x = (R32)surface_x__fixed / 256.f;
	  R32 surface_y = (R32)surface_y__fixed / 256.f;
	  Unused(surface_x);
	  Unused(surface_y);

	  WaylandId *wl_surface = wayland_id_from_list(wayland_state->first_wl_surface, wl_surface_id);
	  Assert(wl_surface);
	  WaylandWindow *window = wl_surface->window;
	  wayland_state->focused_window = window;
	}
	else {
	  // TODO: unhandled opcode
	}
      }
      else if(object_id == wayland_state->wl_keyboard->id) {
	if(opcode == wl_keyboard_key_opcode) {
	  U32 serial = *buf_consume_struct(&body, U32);
	  U32 time = *buf_consume_struct(&body, U32);
	  U32 key = *buf_consume_struct(&body, U32);
	  U32 state = *buf_consume_struct(&body, U32);
	  Unused(serial);
	  Unused(time);

	  Assert(wayland_state->xkb_state);
	  Assert(wayland_state->xkb_keymap);
	  U32 keycode = key + 8;
	  xkb_keysym_t keysym = xkb_state_key_get_one_sym(wayland_state->xkb_state, keycode);

	  event = os_event_list_push_new(arena, &result,
					 state ? Os_EventKind_press : Os_EventKind_release);
	  Wayland_Key wayland_key = Wayland_Key_keyboard_backspace + (keysym ^ WAYLAND_BUTTON_KEY_MASK);
	  event->key = os_key_from_wayland_key[wayland_key];
	}
	else if(opcode == wl_keyboard_enter_opcode) {
	  U32 serial = *buf_consume_struct(&body, U32);
	  U32 wl_surface_id = *buf_consume_struct(&body, U32);
	  U32 keys_array_count = *buf_consume_struct(&body, U32);
	  U8 *keys_array = buf_consume_array(&body, U8, keys_array_count);
	  Unused(serial);
	  Unused(keys_array);

	  WaylandId *wl_surface = wayland_id_from_list(wayland_state->first_wl_surface, wl_surface_id);
	  Assert(wl_surface);
	  WaylandWindow *window = wl_surface->window;
	  wayland_state->focused_window = window;
	}
	else {
	  // TODO: unhandled opcode
	}
      }
      else if(object_id == wayland_state->wl_display->id) {
	if(opcode == wl_display_error_opcode) {
	  U32 error_object_id = *buf_consume_struct(&body, U32);
	  U32 error_code = *buf_consume_struct(&body, U32);
	  U32 error_string_count = *buf_consume_struct(&body, U32);
	  U8 *error_string = buf_consume_array(&body, U8, error_string_count);
	  // TODO: better logging
	  fprintf(stderr, "ERROR: object %u, code %u: %.*s\n",
		  error_object_id, error_code, (int)error_string_count, error_string);
	}else {
	  // TODO: unhandled opcode
	}
      }
      // NOTE: check which per-window id list the message id is on
      else
      {
	WaylandId *xdg_surface = wayland_id_from_list(wayland_state->first_xdg_surface, object_id);
	if(xdg_surface) {
	  if(opcode == xdg_surface_configure_opcode) {
	    U32 serial = *buf_consume_struct(&body, U32);
	    if(xdg_surface_ack_configure(xdg_surface->id, serial)) {
	      continue;
	    }else {
	      // TODO: log error
	    }
	  }
	  else {
	    // TODO: unhandled opcode
	  }
	}

	WaylandId *xdg_toplevel = wayland_id_from_list(wayland_state->first_xdg_toplevel, object_id);
	if(xdg_toplevel) {
	  WaylandWindow *window = xdg_toplevel->window;
	  if(opcode == xdg_toplevel_close_opcode) {
	    event = os_event_list_push_new(arena, &result, Os_EventKind_close);
	    continue;
	  }
	  else if(opcode == xdg_toplevel_configure_opcode) {
	    S32 width = *buf_consume_struct(&body, S32);
	    S32 height = *buf_consume_struct(&body, S32);
	    
	    U32 states_size = body.size;
	    Buffer states = {.size = states_size, .mem = buf_consume_array(&body, U8, states_size)};
	    Unused(states);

	    window->width = width;
	    window->height = height;
	  }
	}

	WaylandId *wl_buffer = wayland_id_from_list(wayland_state->first_wl_buffer, object_id);
	if(wl_buffer) {
	  WaylandWindow *window = wl_buffer->window;
	  if(opcode == wl_buffer_release_opcode) {
	    --wayland_state->wl_buffer_count;
	    wayland_release_id(window->buffers[window->backbuffer_index]);
	    wl_buffer_destroy(window->buffers[window->backbuffer_index]->id);
	    window->buffers[window->backbuffer_index] = 0;
	    continue;
	  }else {
	    // TODO: unhandled opcode
	  }
	}

	WaylandId *frame_callback = wayland_id_from_list(wayland_state->first_frame_callback, object_id);
	if(frame_callback) {
	  WaylandWindow *window = frame_callback->window;
	  if(opcode == wl_callback_done_opcode) {
	    U32 data = *buf_consume_struct(&body, U32);
	    
	    window->last_frame_ms_elapsed = data - window->last_frame_timestamp;
	    window->last_frame_timestamp = data;

	    DLLRemove(wayland_state->first_frame_callback, wayland_state->last_frame_callback, frame_callback);
	    --wayland_state->frame_callback_count;
	    wayland_release_id(frame_callback);
	    window->frame_callback = 0;
	    continue;
	  }else {
	    // TOOD: unhandled opcode
	  }
	}
      }
    }
  }

  arena_release_scratch(scratch);
  return(result);
}
#else
proc Os_EventList
os_events_from_window(Os_Handle window)
{
  Os_EventList result = {0};
  ArenaTemp scratch = arena_get_scratch(0, 0);     

  WaylandWindow *wayland_window = window.handle;

  U32 counter = 0;
  U32 max_count = 100;
  while(wayland_window->frame_callback_id && counter < max_count) {    
    // TODO: we need a way of only getting the events related to the given window
    Buffer msgs = wayland_poll_events(scratch.arena);
    while(msgs.size) {
      
      //Os_Event event = {0};
      Os_Event *event = 0;

      WaylandMessageHeader *header = (WaylandMessageHeader*)msgs.mem;
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
      else if(object_id == wayland_window->xdg_surface_id &&
	      opcode == xdg_surface_configure_opcode) {
	U32 serial = body[0];
	if(xdg_surface_ack_configure(wayland_window->xdg_surface_id, serial)) {
	  fprintf(stderr, "**acked configure**\n");
	}
      }
      else if(object_id == wayland_window->xdg_toplevel_id &&
	      opcode == xdg_toplevel_close_opcode) {
	/* event.kind = Os_EventKind_close; */
	/* os_event_list_push(wayland_window->event_arena, &result, event); */
	event = os_event_list_push_new(wayland_window->event_arena, &result, Os_EventKind_close);
      }
      else if(object_id == wayland_window->xdg_toplevel_id &&
	      opcode == xdg_toplevel_configure_opcode) {
	S32 width = *(S32*)(body + 0);
	S32 height = *(S32*)(body + 1);
	BufferU32 states = {.count = body[2]/sizeof(U32), .mem = body + 3};

	wayland_window->width = width;
	wayland_window->height = height;
	fprintf(stderr, "xdg_toplevel configured: width=%u, height=%u, %lu states\n",
		width, height, states.count);
      }
      else if(wayland_window->frame_callback_id &&
	      object_id == wayland_window->frame_callback_id->id &&
	      opcode == wl_callback_done_opcode) {
	U32 data = body[0];
	wayland_window->last_frame_ms_elapsed = data - wayland_window->last_frame_timestamp;
	wayland_window->last_frame_timestamp = data;

	//SLLStackPush(wayland_state.id_freelist, wayland_window->frame_callback_id);
	wayland_release_id(wayland_window->frame_callback_id);
	wayland_window->frame_callback_id = 0;
      }
      else if(wayland_window->buffer_id[wayland_window->backbuffer_index] &&
	      object_id == wayland_window->buffer_id[wayland_window->backbuffer_index]->id &&
	      opcode == wl_buffer_release_opcode) {
	wayland_release_id(wayland_window->buffer_id[wayland_window->backbuffer_index]);
	wl_buffer_destroy(wayland_window->buffer_id[wayland_window->backbuffer_index]->id);
	wayland_window->buffer_id[wayland_window->backbuffer_index] = 0;
      }
      // NOTE: mouse events
      else if(object_id == wayland_window->wl_pointer_id &&
	      opcode == wl_pointer_motion_opcode) {
	U32 time = body[0];
	U32 surface_x__fixed = body[1];
	U32 surface_y__fixed = body[2];
	R32 surface_x = (R32)surface_x__fixed / 256.f;
	R32 surface_y = (R32)surface_y__fixed / 256.f;
	Unused(time);

	/* event.kind = Os_EventKind_move; */
	/* event.data[0].flt = surface_x; */
	/* event.data[1].flt = surface_y; */
	/* //event.position = (V2){.x = surface_x, .y = surface_y}; */
	/* os_event_list_push(wayland_window->event_arena, &result, event); */
	event = os_event_list_push_new(wayland_window->event_arena, &result, Os_EventKind_move);
	event->pos = v2(surface_x, surface_y);
      }
      else if(object_id == wayland_window->wl_pointer_id &&
	      opcode == wl_pointer_button_opcode) {
	U32 serial = body[0];
	U32 time = body[1];
	U32 button = body[2];
	U32 state = body[3];
	Unused(serial);
	Unused(time);

	/* event.kind = state ? Os_EventKind_press : Os_EventKind_release; */
	/* //event.button = button; // TODO: this might be wrong */
	/* event.data[0].uint = button; */
	/* os_event_list_push(wayland_window->event_arena, &result, event); */
	event = os_event_list_push_new(wayland_window->event_arena, &result,
				       state ? Os_EventKind_press : Os_EventKind_release);
	Wayland_Key wayland_key = Wayland_Key_mouse_left + (button ^ WAYLAND_BUTTON_MOUSE_MASK);
	event->key = os_key_from_wayland_key[wayland_key];
      }
      // NOTE: keyboard events
      else if(object_id == wayland_window->wl_keyboard_id &&
	      opcode == wl_keyboard_key_opcode) {
	U32 serial = body[0];
	U32 time = body[1];
	U32 key = body[2];
	U32 state = body[3];

	if(wayland_window->xkb_state != 0 && wayland_window->xkb_keymap != 0) {
	  U32 keycode = key + 8;
	  xkb_keysym_t keysym = xkb_state_key_get_one_sym(wayland_window->xkb_state, keycode);
	  /* U64 key_name_buffer_size = 128; */
	  /* U8 *key_name_buffer = arena_push_array_z(wayland_window->event_arena, U8, 128); */
	  /* int key_name_length = xkb_keysym_get_name(keysym, (char *)key_name_buffer, key_name_buffer_size); */
	  /* String8 key_name = {.string = key_name_buffer, .count = key_name_length}; */
	  Unused(serial);
	  Unused(time);

	  /* event.kind = state ? Os_EventKind_press : Os_EventKind_release; */
	  /* //event.button = keysym; // TODO: this might be wrong */
	  /* event.data[0].uint = keysym; */
	  /* os_event_list_push(wayland_window->event_arena, &result, event); */
	  event = os_event_list_push_new(wayland_window->event_arena, &result,
					 state ? Os_EventKind_press : Os_EventKind_release);
	  Wayland_Key wayland_key = Wayland_Key_keyboard_backspace + (keysym ^ WAYLAND_BUTTON_KEY_MASK);
	  event->key = os_key_from_wayland_key[wayland_key];

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

      msgs.size -= size;
      msgs.mem  += size;
    }

    ++counter;
  }
  
  arena_release_scratch(scratch);
  return(result);
}
#endif

proc V2S32
os_window_get_dim(Os_Handle window)
{
  WaylandWindow *wayland_window = window.handle;
  V2S32 result = v2s32(wayland_window->width, wayland_window->height);
  return(result);
}

// TODO: move to render module
proc void
os_window_begin_frame(Os_Handle window)
{
  GlFramebuffer framebuffer = wayland_get_framebuffer(window.handle);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);
}

proc void
os_window_end_frame(Os_Handle window)
{
  wayland_end_frame(window.handle);
}
