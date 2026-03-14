/** TODO:
 * support changing cursor icon
 * drag to resize
 * drag and drop
 * support server-side window decoration (and client side too I guess)
 * pull out common code in initialization event handling functions (or generate it)
 * (wip) improve event handling interface
 * remove per-window interfaces from global lists when window is destroyed or initialization fails
 * log messages/errors somewhere besides the console
 */

#include "gfx.wayland.generated.c"
//#include "gfx.wayland.ogl.c" // TODO: alternate backends

// -----------------------------------------------------------------------------
// helpers

proc Os_Handle
wayland__os_handle_from_window(Wayland_Window *window)
{
  Os_Handle result = {0};
  result.handle = window;
  return(result);
}

proc Wayland_Window*
wayland__window_from_os_handle(Os_Handle handle)
{
  Wayland_Window *window = handle.handle;
  return(window);
}

// -----------------------------------------------------------------------------
// id management

proc Wayland_Id*
wayland__new_id(Wayland_Window *window)
{
  Assert(wayland_state->next_id <= WAYLAND_MAX_CLIENT_OBJECT_ID);
  U32 id = wayland_state->next_id++;

  Wayland_Id *result = wayland_state->id_freelist;
  if(result)
  {
    SLLStackPop(wayland_state->id_freelist);
  }
  else
  {
    result = arena_push_struct(wayland_state->arena, Wayland_Id);
  }
  Assert(result);

  result->window = window;
  result->id = id;
  return(result);
}

proc void
wayland__release_id(Wayland_Id *id)
{
  SLLStackPush(wayland_state->id_freelist, id);
}

// TODO: accelerate this lookup
proc Wayland_Id*
wayland__id_from_list(Wayland_Id *ids, U32 id)
{
  Wayland_Id *result = 0;
  for(Wayland_Id *node = ids; node; node = node->next)
  {
    if(node->id == id)
    {
      result = node;
      break;
    }
  }
  return(result);
}

// -----------------------------------------------------------------------------
// initialization helpers

proc B32
wayland__display_connect(void)
{
  B32 result = 1;
  int socket_handle = -1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  // NOTE: get path to wayland display
  char *xdg_runtime_dir = getenv("XDG_RUNTIME_DIR");
  char *wayland_display = getenv("WAYLAND_DISPLAY");
  if(!wayland_display) wayland_display = "wayland-0";
  if(xdg_runtime_dir == 0 || wayland_display == 0) goto wayland__display_connect_failure;

  // NOTE: open unix socket to wayland_display
  {
    String8 wayland_display_path =
      str8_concat(scratch.arena, Str8Cstr(xdg_runtime_dir), Str8Cstr(wayland_display), Str8Lit("/"));
    socket_handle = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un socket_addr = {.sun_family = AF_UNIX};
    CopySize(socket_addr.sun_path, wayland_display_path.string, wayland_display_path.count);
    if(socket_handle == -1) goto wayland__display_connect_failure;

    int connection_result = connect(socket_handle, (struct sockaddr *)&socket_addr, sizeof(socket_addr));
    if(connection_result == -1) goto wayland__display_connect_failure;
  }
  wayland_state->display_socket_handle = socket_handle;

  if(0){
wayland__display_connect_failure:
    if(socket_handle != -1) close(socket_handle);
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wayland__display_get_registry(void)
{
  Wayland_Id *registry = wayland__new_id(0);
  B32 result = wl_display_get_registry(wayland_state->wl_display->id, registry->id);
  if(result)
  {
    wayland_state->wl_registry = registry;
  }

  return(result);
}

// TODO: all these initialization functions that poll for and handle events
//       should call a common routine. Or maybe generated routines?
proc B32
wayland__registry_bind_globals(void)
{
  ArenaTemp scratch = arena_get_scratch(0, 0);

  Buffer msg_buf = wayland__poll_events(scratch.arena);
  while(msg_buf.size)
  {
    /* WaylandMessageHeader *header = (WaylandMessageHeader*)message_buffer.mem; */
    /* U32 object_id = header->object_id; */
    /* U32 opcode = header->opcode; */
    /* U32 message_size = header->message_size; */
    Wayland_MessageHeader *header = buf_consume_struct(&msg_buf, Wayland_MessageHeader);
    U32 object_id = header->object_id;
    U32 opcode = header->opcode;
    U32 msg_sz = header->message_size;

    //U32 *event_body = (U32*)(header + 1);
    //U32 *msg_body = buf_consume_array(&msg_buf, U32, msg_sz/sizeof(*msg_body));

    if(object_id == wayland_state->wl_registry->id &&
       opcode == wl_registry_global_opcode)
    {
      /* U32 name = msg_body[0]; */
      /* U32 interface_string_count = msg_body[1]; */
      /* U8 *interface_string = (U8*)(event_body + 2); */
      /* U32 version = *(U32*)(interface_string + AlignPow2(interface_string_count, 4)); */
      U32 name = *buf_consume_struct(&msg_buf, U32);
      U32 interface_string_count = *buf_consume_struct(&msg_buf, U32);
      U8 *interface_string = buf_consume_array(&msg_buf, U8, AlignPow2(interface_string_count, 4));
      U32 version = *buf_consume_struct(&msg_buf, U32);

      String8 message_interface = {.count = (U64)interface_string_count - 1, .string = interface_string};
      String8 wl_shm_str = Str8Lit("wl_shm");
      String8 xdg_wm_base_str = Str8Lit("xdg_wm_base");
      String8 wl_compositor_str = Str8Lit("wl_compositor");
      String8 wl_seat_str = Str8Lit("wl_seat");
      String8 zwp_linux_dmabuf_v1_str = Str8Lit("zwp_linux_dmabuf_v1");

      if(str8s_are_equal(message_interface, wl_shm_str))
      {
        Wayland_Id *shm = wayland__new_id(0);
        if(wl_registry_bind(wayland_state->wl_registry->id, name, wl_shm_str, version, shm->id))
	{
          wayland_state->wl_shm = shm;
        }
      }
      else if(str8s_are_equal(message_interface, xdg_wm_base_str))
      {
        Wayland_Id *wm_base = wayland__new_id(0);
        if(wl_registry_bind(wayland_state->wl_registry->id, name, xdg_wm_base_str, version, wm_base->id))
	{
          wayland_state->xdg_wm_base = wm_base;
        }
      }
      else if(str8s_are_equal(message_interface, wl_compositor_str))
      {
        Wayland_Id *compositor = wayland__new_id(0);
        if(wl_registry_bind(wayland_state->wl_registry->id, name, wl_compositor_str, version, compositor->id))
	{
          wayland_state->wl_compositor = compositor;
        }
      }
      else if(str8s_are_equal(message_interface, wl_seat_str))
      {
        Wayland_Id *seat = wayland__new_id(0);
        if(wl_registry_bind(wayland_state->wl_registry->id, name, wl_seat_str, version, seat->id))
	{
          wayland_state->wl_seat = seat;
        }
      }
      else if(str8s_are_equal(message_interface, zwp_linux_dmabuf_v1_str))
      {
        Wayland_Id *linux_dmabuf = wayland__new_id(0);
        if(wl_registry_bind(wayland_state->wl_registry->id, name, zwp_linux_dmabuf_v1_str, version, linux_dmabuf->id))
	{
          wayland_state->zwp_linux_dmabuf_v1 = linux_dmabuf;
        }
      }
      else
      {
        fprintf(stderr, "unbound available global: name=%u, interface=%.*s, version=%u\n",
                name, (int)interface_string_count, interface_string, version);
      }
    }
    else
    {
      // TODO: better logging
      fprintf(stderr, "unhandled message(binding globals): object=%u, opcode=%u, length=%u\n",
              object_id, opcode, msg_sz);
      buf_consume_size(&msg_buf, msg_sz - sizeof(*header));
    }

    /* message_buffer.size -= message_size; */
    /* message_buffer.mem += message_size; */
  }

  // TODO: maybe we don't need to say dmabuf is necessary here and fallback to
  // software rendering if it isn't present
  B32 result = (wayland_state->wl_shm &&
                wayland_state->xdg_wm_base &&
                wayland_state->wl_compositor &&
                wayland_state->wl_seat &&
                wayland_state->zwp_linux_dmabuf_v1);
  arena_release_scratch(scratch);
  return(result);
}

proc B32
wayland__get_capabilities(void)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  int dmabuf_format_table_fd = -1;
  U32 dmabuf_format_table_size = 0;

  Assert(wayland_state->sync);
  B32 sync_received = 0;
  while(!sync_received)
  {
    //Buffer message_buffer = wayland_poll_events(scratch.arena);
    U64 data_buffer_size = KB(4);
    U64 control_buffer_size = KB(1);
    U8 *data_buffer = arena_push_array_z(scratch.arena, U8, data_buffer_size);
    U8 *control_buffer = arena_push_array_z(scratch.arena, U8, control_buffer_size);

    struct iovec io = {.iov_base = data_buffer, .iov_len = data_buffer_size};
    struct msghdr socket_msg = {.msg_iov = &io, .msg_iovlen = 1,
                                .msg_control = control_buffer, .msg_controllen = control_buffer_size};

    int bytes_received = recvmsg(wayland_state->display_socket_handle, &socket_msg, 0);
    if(bytes_received)
    {
      U32 bytes_remaining = bytes_received;
      while(bytes_remaining)
      {
        Wayland_MessageHeader *header = (Wayland_MessageHeader*)data_buffer;
        U32 object_id = header->object_id;
        U32 opcode = header->opcode;
        U32 message_size = header->message_size;

        U32 *body = (U32*)(header + 1);

        // TODO: get shm formats, seat capabilities
        if(object_id == wayland_state->wl_display->id &&
           opcode == wl_display_error_opcode)
	{
          U32 error_object_id = body[0];
          U32 error_code = body[1];
          U32 error_string_count = body[2];
          U8 *error_string = (U8 *)(body + 3);
          // TODO: better logging
          fprintf(stderr, "ERROR: object %u, code %u: %.*s\n",
                  error_object_id, error_code, (int)error_string_count, error_string);
        }
        else if(object_id == wayland_state->sync->id &&
                opcode == wl_callback_done_opcode)
	{
          sync_received = 1;
          wayland__release_id(wayland_state->sync);
        }
        else if(object_id == wayland_state->zwp_linux_dmabuf_feedback_v1->id &&
                opcode == zwp_linux_dmabuf_feedback_v1_format_table_opcode)
	{
          dmabuf_format_table_size = body[0];
        }
        else
	{
          fprintf(stderr, "unhandled message: object=%u, opcode=%u, length=%u\n", object_id, opcode, message_size);
        }

        bytes_remaining -= message_size;
        data_buffer += message_size;
      }

      for(struct cmsghdr *cmsg = CMSG_FIRSTHDR(&socket_msg); cmsg; cmsg = CMSG_NXTHDR(&socket_msg, cmsg))
      {
        if(cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_RIGHTS)
	{
          dmabuf_format_table_fd = *(int*)CMSG_DATA(cmsg);
        }
      }
    }
  }
  wayland_state->sync = 0;

  if(dmabuf_format_table_fd != -1)
  {
    if(dmabuf_format_table_size)
    {
      void *dmabuf_format_table = mmap(0, dmabuf_format_table_size,
                                       PROT_READ, MAP_PRIVATE,
                                       dmabuf_format_table_fd, 0);
      if(dmabuf_format_table != (void*)-1)
      {
        wayland_state->dmabuf_format_table = dmabuf_format_table;
        wayland_state->dmabuf_format_table_size = dmabuf_format_table_size;
        close(dmabuf_format_table_fd);
      }
    }
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wayland__initialize_input(void)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  // TODO: It would be more proper to wait until we receive capabilities from
  //       the seat before we try getting keyboard and mouse. That might also
  //       prevent us from having to call recvmsg in a loop below
  Wayland_Id *keyboard = wayland__new_id(0);
  if(wl_seat_get_keyboard(wayland_state->wl_seat->id, keyboard->id))
  {
    wayland_state->wl_keyboard = keyboard;
  }
  else
  {
    result = 0;
  }

  Wayland_Id *pointer = wayland__new_id(0);
  if(wl_seat_get_pointer(wayland_state->wl_seat->id, pointer->id))
  {
    wayland_state->wl_pointer = pointer;
  }
  else
  {
    result = 0;
  }

  int keymap_fd = -1;
  U32 keymap_format = 0;
  U32 keymap_size = 0;

  while(keymap_fd == -1)
  {
    fprintf(stderr, "DEBUG(wayland_initialize_input): attempting to receive keymap file descriptor...\n");

    U64 data_buffer_size = 256;
    U64 control_buffer_size = 256;
    U8 *data_buffer = arena_push_array_z(scratch.arena, U8, data_buffer_size);
    U8 *control_buffer = arena_push_array_z(scratch.arena, U8, control_buffer_size);

    struct iovec io = {.iov_base = data_buffer, .iov_len = data_buffer_size};
    struct msghdr socket_msg = {.msg_iov = &io, .msg_iovlen = 1,
                                .msg_control = control_buffer, .msg_controllen = control_buffer_size};

    int bytes_received = recvmsg(wayland_state->display_socket_handle, &socket_msg, 0);
    if(bytes_received != -1)
    {
      // NOTE: process regular data
      U32 bytes_remaining = bytes_received;
      while(bytes_remaining)
      {
        Wayland_MessageHeader *event_header = (Wayland_MessageHeader*)data_buffer;
        U32 object_id = event_header->object_id;
        U32 opcode = event_header->opcode;
        U32 message_size = event_header->message_size;
        Assert(message_size < data_buffer_size);
        U32 *event_body = (U32 *)(event_header + 1);

        // TODO: handle keyboard repeat info event
        if(object_id == wayland_state->wl_keyboard->id &&
           opcode == wl_keyboard_keymap_opcode)
	{
          keymap_format = event_body[0];
          keymap_size = event_body[1];
          fprintf(stderr, "%s (size=%u)\n", keymap_format ? "got xkb keymap" : "no keymap", keymap_size);
        }
	else
	{
          fprintf(stderr, "unhandled message: object=%u, opcode=%u, length=%u\n", object_id, opcode, message_size);
        }

        bytes_remaining -= message_size;
        data_buffer += message_size;
      }

      // NOTE: process ancillary data
      for(struct cmsghdr *cmsg = CMSG_FIRSTHDR(&socket_msg); cmsg; cmsg = CMSG_NXTHDR(&socket_msg, cmsg))
      {
        fprintf(stderr, "DEBUG(wayland_initialize_input): recvmsg ancillary data loop iteration\n");
        if(cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_RIGHTS)
	{
          fprintf(stderr, "DEBUG(wayland_initialize_input): reading file descriptor\n");
          keymap_fd = *(int *)CMSG_DATA(cmsg);
        }
      }
    }
    else
    {
      fprintf(stderr, "ERROR(wayland_initialize_input): recvmsg failed\n");
    }
  }

  if(keymap_fd != -1)
  {
    if(keymap_format)
    {
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
    }
    else
    {
      fprintf(stderr, "ERROR: no xkb format\n");
    }
  }
  else
  {
    fprintf(stderr, "ERROR: no xkb keymap file descriptor\n");
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wayland_init(void)
{
  Arena *arena  = arena_alloc();
  wayland_state = arena_push_struct(arena, WaylandState);
  if(wayland_state == 0) goto wayland_init_failure;

  wayland_state->arena = arena;
  wayland_state->next_id = 1;
  wayland_state->wl_display = wayland__new_id(0);
  wayland_state->xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);

  if(!wayland__display_connect()) goto wayland_init_failure;

  if(!wayland__display_get_registry()) goto wayland_init_failure;

  if(!wayland__registry_bind_globals()) goto wayland_init_failure;

  Wayland_Id *feedback = wayland__new_id(0);
  if(zwp_linux_dmabuf_v1_get_default_feedback(wayland_state->zwp_linux_dmabuf_v1->id, feedback->id))
  {
    wayland_state->zwp_linux_dmabuf_feedback_v1 = feedback;
  }

  Wayland_Id *sync = wayland__new_id(0);
  if(wl_display_sync(wayland_state->wl_display->id, sync->id))
  {
    wayland_state->sync = sync;
  }

  if(!wayland__get_capabilities()) goto wayland_init_failure;

  if(!wayland__initialize_input()) goto wayland_init_failure;

  //if(!wayland_gl_init()) goto wayland_init_failure;

  if(0){
  wayland_init_failure:
    // TODO: free stuff if we failed
    arena_release(arena);
    return(0);
  }
  return(1);
}

proc void
wayland_uninit(void)
{
  // TODO
}

// -----------------------------------------------------------------------------
// window helpers

proc B32
wayland_create_surface(Wayland_Window *window, String8 title)
{
  B32 result = 1;

  Wayland_Id *wl_surface = wayland__new_id(window);
  if(wl_compositor_create_surface(wayland_state->wl_compositor->id, wl_surface->id))
  {
    window->wl_surface = wl_surface;
    DLLPushBack(wayland_state->first_wl_surface, wayland_state->last_wl_surface, wl_surface);
    ++wayland_state->wl_surface_count;

    Wayland_Id *xdg_surface = wayland__new_id(window);
    if(xdg_wm_base_get_xdg_surface(wayland_state->xdg_wm_base->id, xdg_surface->id, wl_surface->id))
    {
      window->xdg_surface = xdg_surface;
      DLLPushBack(wayland_state->first_xdg_surface, wayland_state->last_xdg_surface, xdg_surface);
      ++wayland_state->xdg_surface_count;

      Wayland_Id *toplevel = wayland__new_id(window);
      if(xdg_surface_get_toplevel(window->xdg_surface->id, toplevel->id))
      {
        window->xdg_toplevel = toplevel;
        DLLPushBack(wayland_state->first_xdg_toplevel, wayland_state->last_xdg_toplevel, toplevel);
        ++wayland_state->xdg_toplevel_count;

        if(!xdg_toplevel_set_title(window->xdg_toplevel->id, title))
	{
          fprintf(stderr, "ERROR: xdg_toplevel_set_title failed\n");
        }

        // NOTE: flip y
        if(wl_surface_set_buffer_transform(window->wl_surface->id, 6))
	{
          if(wl_surface_commit(window->wl_surface->id))
	  {
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

/* proc B32 */
/* wayland_allocate_shared_memory(Wayland_Window *window, U64 size) */
/* { */
/*   B32 result = 1; */
/*   int shared_memory_handle = -1; */
/*   void *shared_memory = MAP_FAILED; */
/*   B32 shm_create_pool_result = 0; */

/*   ArenaTemp scratch = arena_get_scratch(0, 0); */

/*   // TODO: use memfd_create() on linux instead of creating a file by hand? */
/*   char *filename = "fhgjkaspytqwczvxbteqiomzxcvalwxpm"; */
/*   shared_memory_handle = shm_open(filename, O_RDWR | O_EXCL | O_CREAT, S_IRUSR | S_IWUSR); */
/*   if(shared_memory_handle == -1) goto wayland_allocate_shared_memory_failure; */

/*   int shm_unlink_result = shm_unlink(filename); */
/*   if(shm_unlink_result == -1) goto wayland_allocate_shared_memory_failure; */
/*   int ftruncate_result = ftruncate(shared_memory_handle, size); */
/*   if(ftruncate_result == -1) goto wayland_allocate_shared_memory_failure; */
/*   shared_memory = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_handle, 0); */
/*   if(shared_memory == MAP_FAILED) goto wayland_allocate_shared_memory_failure; */
/*   window->shared_memory = shared_memory; */
/*   window->shared_memory_size = size; */

/*   Wayland_Id *wl_shm_pool = wayland__new_id(window); */
/*   shm_create_pool_result = */
/*     wl_shm_create_pool(wayland_state->wl_shm->id, wl_shm_pool->id, shared_memory_handle, size); */
/*   if(!shm_create_pool_result) goto wayland_allocate_shared_memory_failure; */
/*   window->wl_shm_pool = wl_shm_pool; */
/*   DLLPushBack(wayland_state->first_wl_shm_pool, wayland_state->last_wl_shm_pool, wl_shm_pool); */
/*   ++wayland_state->wl_shm_pool_count; */

/* wayland_allocate_shared_memory_failure: */
/*   if(shared_memory_handle != -1) */
/*   { */
/*     close(shared_memory_handle); */
/*   } */
/*   if(shared_memory != MAP_FAILED) */
/*   { */
/*     munmap(shared_memory, size); */
/*     window->shared_memory = 0; */
/*     window->shared_memory_size = 0; */
/*   } */
/*   if(!shm_create_pool_result) */
/*   { */
/*     wayland__release_id(wl_shm_pool); */
/*   } */

/*   arena_release_scratch(scratch); */
/*   return(result); */
/* } */

/* proc B32 */
/* wayland_create_shm_buffer(Wayland_Window *window) */
/* { */
/*   B32 result = 1; */

/*   S32 stride = window->width*sizeof(U32); */
/*   U32 format = 0x34324241; // NOTE: ABGR 32-bit little-endian // TODO: generate enums */

/*   //Assert(!window->buffer_id[window->backbuffer_index]); */
/*   Wayland_Id *buffer = wayland__new_id(window); */
/*   if(wl_shm_pool_create_buffer(window->wl_shm_pool->id, buffer->id, */
/*                                window->backbuffer_index*window->shared_memory_size/2, */
/*                                window->width, window->height, stride, format)) */
/*   { */
/*     window->buffers[window->backbuffer_index] = buffer; */
/*     DLLPushBack(wayland_state->first_wl_buffer, wayland_state->last_wl_buffer, buffer); */
/*     ++wayland_state->wl_buffer_count; */
/*   } */
/*   else */
/*   { */
/*     // NOTE: buffer creation failed */
/*     result = 0; */
/*   } */

/*   return(result); */
/* } */

proc void
wayland_log_error_(char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  str8_list_push_fv(wayland_state->arena, &wayland_state->error_list, fmt, args);
  va_end(args);
}

proc Buffer
wayland__poll_events(Arena *arena)
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

proc Os_Key
wayland_os_key_from_linux_event_code(U32 code)
{
  local Os_Key code_map[] = {
    [BTN_LEFT]              = Os_Key_mouse_left,
    [BTN_MIDDLE]            = Os_Key_mouse_middle,
    [BTN_RIGHT]             = Os_Key_mouse_right,
    [KEY_ESC] = Os_Key_esc,
    [KEY_1] = Os_Key_1,
    [KEY_2] = Os_Key_2,
    [KEY_3] = Os_Key_3,
    [KEY_4] = Os_Key_4,
    [KEY_5] = Os_Key_5,
    [KEY_6] = Os_Key_6,
    [KEY_7] = Os_Key_7,
    [KEY_8] = Os_Key_8,
    [KEY_9] = Os_Key_9,
    [KEY_0] = Os_Key_0,
    [KEY_MINUS] = Os_Key_hyphen,
    [KEY_EQUAL] = Os_Key_equals,
    [KEY_BACKSPACE] = Os_Key_backspace,
    [KEY_TAB] = Os_Key_tab,
    [KEY_Q] = Os_Key_q,
    [KEY_W] = Os_Key_w,
    [KEY_E] = Os_Key_e,
    [KEY_R] = Os_Key_r,
    [KEY_T] = Os_Key_t,
    [KEY_Y] = Os_Key_y,
    [KEY_U] = Os_Key_u,
    [KEY_I] = Os_Key_i,
    [KEY_O] = Os_Key_o,
    [KEY_P] = Os_Key_p,
    [KEY_LEFTBRACE] = Os_Key_l_bracket,
    [KEY_RIGHTBRACE] = Os_Key_r_bracket,
    [KEY_ENTER] = Os_Key_enter,
    [KEY_LEFTCTRL] = Os_Key_control,
    [KEY_RIGHTCTRL] = Os_Key_control,
    [KEY_LEFTALT] = Os_Key_alt,
    [KEY_RIGHTALT] = Os_Key_alt,
    [KEY_A] = Os_Key_a,
    [KEY_S] = Os_Key_s,
    [KEY_D] = Os_Key_d,
    [KEY_F] = Os_Key_f,
    [KEY_G] = Os_Key_g,
    [KEY_H] = Os_Key_h,
    [KEY_J] = Os_Key_j,
    [KEY_K] = Os_Key_k,
    [KEY_L] = Os_Key_l,
    [KEY_SEMICOLON] = Os_Key_semicolon,
    [KEY_APOSTROPHE] = Os_Key_quote,
    [KEY_GRAVE] = Os_Key_backtick,
    [KEY_LEFTSHIFT] = Os_Key_shift,
    [KEY_RIGHTSHIFT] = Os_Key_shift,
    [KEY_BACKSLASH] = Os_Key_backslash,
    [KEY_Z] = Os_Key_z,
    [KEY_X] = Os_Key_x,
    [KEY_C] = Os_Key_c,
    [KEY_V] = Os_Key_v,
    [KEY_B] = Os_Key_b,
    [KEY_N] = Os_Key_n,
    [KEY_M] = Os_Key_m,
    [KEY_COMMA] = Os_Key_comma,
    [KEY_DOT] = Os_Key_period,
    [KEY_SLASH] = Os_Key_slash,
    [KEY_SPACE] = Os_Key_space,
    [KEY_CAPSLOCK] = Os_Key_capslock,
    [KEY_F1] = Os_Key_f1,
    [KEY_F2] = Os_Key_f2,
    [KEY_F3] = Os_Key_f3,
    [KEY_F4] = Os_Key_f4,
    [KEY_F5] = Os_Key_f5,
    [KEY_F6] = Os_Key_f6,
    [KEY_F7] = Os_Key_f7,
    [KEY_F8] = Os_Key_f8,
    [KEY_F9] = Os_Key_f9,
    [KEY_F10] = Os_Key_f10,
    [KEY_F11] = Os_Key_f11,
    [KEY_F12] = Os_Key_f12,
    [KEY_F12] = Os_Key_f12,
    [KEY_KP0] = Os_Key_numpad_0,
    [KEY_KP1] = Os_Key_numpad_1,
    [KEY_KP2] = Os_Key_numpad_2,
    [KEY_KP3] = Os_Key_numpad_3,
    [KEY_KP4] = Os_Key_numpad_4,
    [KEY_KP5] = Os_Key_numpad_5,
    [KEY_KP6] = Os_Key_numpad_6,
    [KEY_KP7] = Os_Key_numpad_7,
    [KEY_KP8] = Os_Key_numpad_8,
    [KEY_KP9] = Os_Key_numpad_9,
    [KEY_KPASTERISK] = Os_Key_numpad_star,
    [KEY_KPMINUS] = Os_Key_numpad_minus,
    [KEY_KPPLUS] = Os_Key_numpad_plus,
    [KEY_KPDOT] = Os_Key_numpad_dot,
    [KEY_KPENTER] = Os_Key_numpad_enter,
    [KEY_KPSLASH] = Os_Key_numpad_slash,
    [KEY_HOME] = Os_Key_home,
    [KEY_UP] = Os_Key_up,
    [KEY_PAGEUP] = Os_Key_page_up,
    [KEY_LEFT] = Os_Key_left,
    [KEY_RIGHT] = Os_Key_right,
    [KEY_END] = Os_Key_end,
    [KEY_DOWN] = Os_Key_down,
    [KEY_PAGEDOWN] = Os_Key_page_down,
    [KEY_DELETE] = Os_Key_delete,
    [KEY_PAUSE] = Os_Key_pause,
    [KEY_MUTE] = Os_Key_mute,
    [KEY_VOLUMEDOWN] = Os_Key_volume_down,
    [KEY_VOLUMEUP] = Os_Key_volume_up,
    [KEY_LEFTMETA] = Os_Key_super,
    [KEY_RIGHTMETA] = Os_Key_super,
    [KEY_SLEEP] = Os_Key_sleep,
  };

  Os_Key result = Os_Key_null;
  if(code < ArrayCount(code_map)) result = code_map[code];
  return(result);
}

proc Os_Key
wayland_os_key_from_xkb_keysym(xkb_keysym_t keysym)
{
  local Os_Key keysym_map[]      = {
    [XKB_KEY_BackSpace & 0xFF]   = Os_Key_backspace,
    [XKB_KEY_Tab & 0xFF]         = Os_Key_tab,
    [XKB_KEY_Return & 0xFF]      = Os_Key_enter,
    [XKB_KEY_Pause & 0xFF]       = Os_Key_pause,
    [XKB_KEY_Escape & 0xFF]      = Os_Key_esc,
    [XKB_KEY_Delete & 0xFF]      = Os_Key_delete,
    [XKB_KEY_Left & 0xFF]        = Os_Key_left,
    [XKB_KEY_Up & 0xFF]          = Os_Key_up,
    [XKB_KEY_Right & 0xFF]       = Os_Key_right,
    [XKB_KEY_Down & 0xFF]        = Os_Key_down,
    [XKB_KEY_Page_Up & 0xFF]     = Os_Key_page_up,
    [XKB_KEY_Page_Down & 0xFF]   = Os_Key_page_down,
    [XKB_KEY_KP_Enter & 0xFF]    = Os_Key_numpad_enter,
    [XKB_KEY_KP_Multiply & 0xFF] = Os_Key_numpad_star,
    [XKB_KEY_KP_Add & 0xFF]      = Os_Key_numpad_plus,
    [XKB_KEY_KP_Subtract & 0xFF] = Os_Key_numpad_minus,
    [XKB_KEY_KP_Decimal & 0xFF]  = Os_Key_numpad_dot,
    [XKB_KEY_KP_Divide & 0xFF]   = Os_Key_numpad_slash,
    [XKB_KEY_KP_0 & 0xFF]        = Os_Key_numpad_0,
    [XKB_KEY_KP_1 & 0xFF]        = Os_Key_numpad_1,
    [XKB_KEY_KP_2 & 0xFF]        = Os_Key_numpad_2,
    [XKB_KEY_KP_3 & 0xFF]        = Os_Key_numpad_3,
    [XKB_KEY_KP_4 & 0xFF]        = Os_Key_numpad_4,
    [XKB_KEY_KP_5 & 0xFF]        = Os_Key_numpad_5,
    [XKB_KEY_KP_6 & 0xFF]        = Os_Key_numpad_6,
    [XKB_KEY_KP_7 & 0xFF]        = Os_Key_numpad_7,
    [XKB_KEY_KP_8 & 0xFF]        = Os_Key_numpad_8,
    [XKB_KEY_KP_9 & 0xFF]        = Os_Key_numpad_9,
    [XKB_KEY_F1 & 0xFF]          = Os_Key_f1,
    [XKB_KEY_F2 & 0xFF]          = Os_Key_f2,
    [XKB_KEY_F3 & 0xFF]          = Os_Key_f3,
    [XKB_KEY_F4 & 0xFF]          = Os_Key_f4,
    [XKB_KEY_F5 & 0xFF]          = Os_Key_f5,
    [XKB_KEY_F6 & 0xFF]          = Os_Key_f6,
    [XKB_KEY_F7 & 0xFF]          = Os_Key_f7,
    [XKB_KEY_F8 & 0xFF]          = Os_Key_f8,
    [XKB_KEY_F9 & 0xFF]          = Os_Key_f9,
    [XKB_KEY_F10 & 0xFF]         = Os_Key_f10,
    [XKB_KEY_F11 & 0xFF]         = Os_Key_f11,
    [XKB_KEY_F12 & 0xFF]         = Os_Key_f12,
    [XKB_KEY_Shift_L & 0xFF]     = Os_Key_shift,
    [XKB_KEY_Shift_R & 0xFF]     = Os_Key_shift,
    [XKB_KEY_Control_L & 0xFF]   = Os_Key_control,
    [XKB_KEY_Control_R & 0xFF]   = Os_Key_control,
    [XKB_KEY_Caps_Lock & 0xFF]   = Os_Key_capslock,
    [XKB_KEY_Alt_L & 0xFF]       = Os_Key_alt,
    [XKB_KEY_Alt_R & 0xFF]       = Os_Key_alt,
    [XKB_KEY_Super_L & 0xFF]     = Os_Key_super,
    [XKB_KEY_Super_R & 0xFF]     = Os_Key_super,
  };

  Os_Key result = Os_Key_null;
  U32 keysym_idx = keysym & 0xFF;
  if(keysym_idx < ArrayCount(keysym_map)) result = keysym_map[keysym_idx];
  return(result);
}

proc Os_Handle
wayland_os_handle_from_window(Wayland_Window *window)
{
  Os_Handle result = {0};
  result.handle = window;
  return(result);
}

proc Wayland_Window*
wayland_window_from_os_handle(Os_Handle handle)
{
  Wayland_Window *result = handle.handle;
  return(result);
}

proc Wayland_Window*
wayland_window_open(String8 title, S32 width, S32 height)
{
  ArenaTemp scratch = arena_get_scratch(0, 0);

  // TODO: function for allocating window?
  Wayland_Window *window = wayland_state->window_freelist;
  if(window != 0)
  {
    SLLStackPop(wayland_state->window_freelist);
  }
  else
  {
    window = arena_push_struct(wayland_state->arena, Wayland_Window);
  }
  Assert(window);

  if(!wayland_create_surface(window, title)) goto wayland_window_open_failure;

  // TODO: pull out into another function?
  B32 acked_configure = 0;
  while(!acked_configure)
  {
    Buffer events = wayland__poll_events(scratch.arena);
    while(events.size)
    {
      Wayland_MessageHeader *event_header = buf_consume_struct(&events, Wayland_MessageHeader);
      U32 object_id = event_header->object_id;
      U32 opcode = event_header->opcode;
      U32 message_size = event_header->message_size;
      /* U32 message_body_u32_count = (message_size - sizeof(WaylandMessageHeader)) / 4; */
      /* U32 *event_body = buf_consume_array(&events, U32, message_body_u32_count); */

      if(object_id == wayland_state->xdg_wm_base->id &&
         opcode == xdg_wm_base_ping_opcode)
      {
        //U32 serial = event_body[0];
	U32 serial = *buf_consume_struct(&events, U32);
        xdg_wm_base_pong(wayland_state->xdg_wm_base->id, serial);
      }
      else if(object_id == window->xdg_surface->id &&
              opcode == xdg_surface_configure_opcode)
      {
        //U32 serial = event_body[0];
	U32 serial = *buf_consume_struct(&events, U32);
        if(xdg_surface_ack_configure(window->xdg_surface->id, serial))
	{
          acked_configure = 1;
        }
      }
      else if(object_id == wayland_state->wl_display->id &&
              opcode == wl_display_error_opcode)
      {
        /* U32 error_object_id = event_body[0]; */
        /* U32 error_code = event_body[1]; */
        /* U32 error_string_count = event_body[2]; */
        /* U8 *error_string = (U8 *)(event_body + 3); */
	U32 error_object_id = *buf_consume_struct(&events, U32);
	U32 error_code = *buf_consume_struct(&events, U32);
	U32 error_string_count = *buf_consume_struct(&events, U32);
	U8 *error_string = buf_consume_array(&events, U8, AlignPow2(error_string_count, 4));
        // TODO: better logging
        fprintf(stderr, "ERROR: object %u, code %u: %.*s\n",
                error_object_id, error_code, (int)error_string_count, error_string);
      }
      else
      {
        // TODO: better logging
        fprintf(stderr, "unhandled message: object=%u, opcode=%u, length=%u\n", object_id, opcode, message_size);
	buf_consume_size(&events, message_size - sizeof(*event_header));
      }
    }
  }

  // TODO: move to software render init
  /* U64 shared_memory_size = 1960*1080*sizeof(U32)*2; */
  /* if(!wayland_allocate_shared_memory(window, shared_memory_size)) goto wayland_window_open_failure; */

  //if(!wayland_allocate_textures(window)) goto wayland_allocate_textures_failure;

  // NOTE: open all supported backends (we want all backends ready in case we switch to it)
  /* for(U32 backend_idx = 0; backend_idx < Wayland_Backend_Count; ++backend_idx) */
  /* { */
  /*   U32 backend_flag = (1 << backend_idx); */
  /*   if(wayland_state->supported_backends & backend_flag) */
  /*   { */
  /*     wayland_state->backends[backend_idx].open_window(window); */
  /*   } */
  /* } */

  window->width = width;
  window->height = height;
  //window->render_target = render_target;
  window->enabled_backend = Wayland_Backend_opengl; // TODO: configure default

#if WAYLAND_BACKEND & WAYLAND_BACKEND_SOFTWARE
  if(wayland_state->supported_backends & Wayland_BackendFlag_software)
  {
    window->backends[Wayland_Backend_software] = wayland_software_window_open(window);
  }
#endif
#if WAYLAND_BACKEND & WAYLAND_BACKEND_OPENGL
  if(wayland_state->supported_backends & Wayland_BackendFlag_opengl)
  {
    window->backends[Wayland_Backend_opengl] = wayland_gl_window_open(window);
  }
#endif
#if WAYLAND_BACKEND & WAYLAND_BACKEND_VULKAN
  if(wayland_state->supported_backends & Wayland_BackendFlag_vulkan)
  {
    window->backends[Wayland_Backend_vulkan] = wayland_vulkan_window_open(window);
  }
#endif

  if(!wayland_window_end_frame(window)) goto wayland_window_open_failure;

  DLLPushBack(wayland_state->first_window, wayland_state->last_window, window);
  ++wayland_state->window_count;

  if(0) {
  wayland_window_open_failure:
    ZeroStruct(window);
    // TODO: function for destroying window?
    SLLStackPush(wayland_state->window_freelist, window);
    window = 0;
  }

  arena_release_scratch(scratch);
  return(window);
}

/* proc U32* */
/* wayland_get_render_pixels(Wayland_Window *window) */
/* { */
/*   U32 *result = (U32*)((U8*)window->shared_memory + window->backbuffer_index*window->shared_memory_size/2); */
/*   return(result); */
/* } */

/* proc GlFramebuffer */
/* wayland_get_framebuffer(Wayland_Window *window) */
/* { */
/*   GlFramebuffer result = window->gl_framebuffer[window->backbuffer_index]; */
/*   return(result); */
/* } */

proc B32
wayland_create_buffer(Wayland_Window *window)
{
  Wayland_CreateBufferProc *create_buffer = wayland_state->backends[window->enabled_backend].create_buffer;
  /* switch(window->enabled_backend) */
  /* { */
  /*   case Wayland_Backend_software: */
  /*   { */
  /*     create_buffer = wayland_state->backends[Wayland_Backend_software].create_buffer; */
  /*     Assert(create_buffer); */
  /*     result = create_buffer(window); */
  /*   }break; */

  /*   case Wayland_Backend_opengl: */
  /*   { */
  /*     create_buffer = wayland_state->backends[Wayland_Backend_opengl].create_buffer; */
  /*     Assert(create_buffer); */
  /*     result = create_buffer(window); */
  /*   }break; */

  /*   case Wayland_Backend_vulkan: */
  /*   { */
  /*     create_buffer = wayland_state->backends[Wayland_Backend_vulkan].create_buffer; */
  /*     Assert(create_buffer); */
  /*     result = create_buffer(window); */
  /*   }break; */

  /*   UnreachableDefault(); */
  /* } */

  B32 result = create_buffer(window);
  return(result);
}

proc B32
wayland_window_end_frame(Wayland_Window *window)
{
  B32 result = 1;

  Assert(!window->frame_callback);
  if(wayland_create_buffer(window))
  {
    if(wl_surface_attach(window->wl_surface->id, window->buffers[window->backbuffer_index]->id, 0, 0))
    {
      if(wl_surface_damage(window->wl_surface->id, 0, 0, window->width, window->height))
      {
        Wayland_Id *frame_callback = wayland__new_id(window);
        if(wl_surface_frame(window->wl_surface->id, frame_callback->id))
	{
          window->frame_callback = frame_callback;
          DLLPushBack(wayland_state->first_frame_callback, wayland_state->last_frame_callback, frame_callback);
          ++wayland_state->frame_callback_count;

          if(wl_surface_commit(window->wl_surface->id))
	  {
            ++window->frame_index;
            ++window->backbuffer_index;
            window->backbuffer_index %= ArrayCount(window->buffers);
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
wayland_window_close(Wayland_Window *window)
{
  xdg_toplevel_destroy(window->xdg_toplevel->id);
  wayland__release_id(window->xdg_toplevel);

  xdg_surface_destroy(window->xdg_surface->id);
  wayland__release_id(window->xdg_surface);

  wl_surface_destroy(window->wl_surface->id);
  wayland__release_id(window->wl_surface);

  ZeroStruct(window);
  SLLStackPush(wayland_state->window_freelist, window);
  --wayland_state->window_count;
}

proc B32
wayland_backend_is_supported(Wayland_Backend backend)
{
  B32 result = wayland_state->supported_backends & (1 << backend);
  return(result);
}

proc void
wayland_window_set_backend(Wayland_Window *window, Wayland_Backend backend)
{
  if(wayland_backend_is_supported(backend))
  {
    window->enabled_backend = backend;
  }
}

/* // NOTE: general interface */
/* proc B32 */
/* os_gfx_init(void) */
/* { */
/*   B32 result = wayland_init(); */
/*   return(result); */
/* } */

/* proc Os_Handle */
/* os_open_window(String8 name, S32 width, S32 height) */
/* { */
/*   Wayland_Window *window = wayland_open_window(name, width, height, RenderTarget_hardware); */
/*   Os_Handle result = wayland_os_handle_from_window(window); */
/*   return(result); */
/* } */

/* proc void */
/* os_close_window(Os_Handle window) */
/* { */
/*   Wayland_Window *wayland_window = wayland_window_from_os_handle(window); */
/*   wayland_close_window(wayland_window); */
/* } */

proc Os_EventList
wayland_events(Arena *arena)
{
  Os_EventList result = {0};

  ArenaTemp scratch = arena_get_scratch(&arena, 1);

  U32 counter = 0;
  U32 max_count = 100;
  while(wayland_state->frame_callback_count > 0 && counter < max_count)
  {
    ++counter;

    Buffer msgs = wayland__poll_events(scratch.arena);
    while(msgs.size)
    {
      Os_Event *event = 0;

      // DEBUG
      fprintf(stderr, "msgs: size=%lu\n", msgs.size);

      Wayland_MessageHeader *header = buf_consume_struct(&msgs, Wayland_MessageHeader);
      U32 object_id = header->object_id;
      U32 opcode = header->opcode;
      U32 size = header->message_size;
      Unused(size);
      //U32 *body = buf_consume_array(&msgs, U32, message_body_u32_count);

      fprintf(stderr, "header: object_id=%u, opcode=%u, size=%u\n",
	      object_id, opcode, size);

      /* U32 body_size = size - sizeof(Wayland_MessageHeader); */
      /* Buffer body = {.size = body_size, .mem = buf_consume_array(&msgs, U8, body_size)}; */

      // NOTE: check if message id is a global id
      if(object_id == wayland_state->xdg_wm_base->id)
      {
        if(opcode == xdg_wm_base_pong_opcode)
        {
	  fprintf(stderr, "xdg_wm_base_pong\n");
          U32 serial = *buf_consume_struct(&msgs, U32);
          if(!xdg_wm_base_pong(wayland_state->xdg_wm_base->id, serial))
	  {
            // TODO: log error
          }
	  continue;
        }
        else
        {
          // TODO: unhandled opcode
        }
      }
      else if(object_id == wayland_state->wl_pointer->id)
      {
        Os_Handle window = wayland__os_handle_from_window(wayland_state->focused_window);
        if(opcode == wl_pointer_motion_opcode)
        {
	  fprintf(stderr, "pointer_motion\n");
          U32 time = *buf_consume_struct(&msgs, U32);
          U32 surface_x__fixed = *buf_consume_struct(&msgs, U32);
          U32 surface_y__fixed = *buf_consume_struct(&msgs, U32);
          Unused(time);

          R32 surface_x = (R32)surface_x__fixed / 256.f;
          R32 surface_y = (R32)surface_y__fixed / 256.f;
          event = gfx__event_list_push_new(arena, &result, Os_EventKind_move);
          event->pos = v2(surface_x, surface_y);
          event->window = window;
	  continue;
        }
        else if(opcode == wl_pointer_button_opcode)
        {
	  fprintf(stderr, "pointer_button\n");
          U32 serial = *buf_consume_struct(&msgs, U32);
          U32 time = *buf_consume_struct(&msgs, U32);
          U32 button = *buf_consume_struct(&msgs, U32);
          U32 state = *buf_consume_struct(&msgs, U32);
          Unused(serial);
          Unused(time);

          event = gfx__event_list_push_new(arena, &result,
                                         state ? Os_EventKind_press : Os_EventKind_release);
          event->key = wayland_os_key_from_linux_event_code(button);
          event->window = window;
	  continue;
        }
        else if(opcode == wl_pointer_enter_opcode)
        {
	  fprintf(stderr, "pointer_enter\n");
          U32 serial = *buf_consume_struct(&msgs, U32);
          U32 wl_surface_id = *buf_consume_struct(&msgs, U32);
          U32 surface_x__fixed = *buf_consume_struct(&msgs, U32);
          U32 surface_y__fixed = *buf_consume_struct(&msgs, U32);
          Unused(serial);

          R32 surface_x = (R32)surface_x__fixed / 256.f;
          R32 surface_y = (R32)surface_y__fixed / 256.f;
          Unused(surface_x);
          Unused(surface_y);

          Wayland_Id *wl_surface = wayland__id_from_list(wayland_state->first_wl_surface, wl_surface_id);
          Assert(wl_surface);
          Wayland_Window *focused_window = wl_surface->window;
          wayland_state->focused_window = focused_window;
	  continue;
        }
        else
	{
          // TODO: unhandled opcode
        }
      }
      else if(object_id == wayland_state->wl_keyboard->id)
      {
        Os_Handle window = wayland__os_handle_from_window(wayland_state->focused_window);
        if(opcode == wl_keyboard_key_opcode)
        {
	  fprintf(stderr, "keyboard_key\n");
          U32 serial = *buf_consume_struct(&msgs, U32);
          U32 time = *buf_consume_struct(&msgs, U32);
          U32 key = *buf_consume_struct(&msgs, U32);
          U32 state = *buf_consume_struct(&msgs, U32);
          Unused(serial);
          Unused(time);

          Assert(wayland_state->xkb_state);
          Assert(wayland_state->xkb_keymap);
          U32 keycode = key + 8;
          xkb_keysym_t keysym = xkb_state_key_get_one_sym(wayland_state->xkb_state, keycode);
          Unused(keysym); // TODO: incorporate the key mapping into the event in some way

          event = gfx__event_list_push_new(arena, &result,
                                         state ? Os_EventKind_press : Os_EventKind_release);
          event->key = wayland_os_key_from_linux_event_code(key);
          event->window = window;
	  continue;
        }
        else if(opcode == wl_keyboard_enter_opcode)
        {
	  fprintf(stderr, "keyboard_enter\n");
          U32 serial = *buf_consume_struct(&msgs, U32);
          U32 wl_surface_id = *buf_consume_struct(&msgs, U32);
          U32 keys_array_count = *buf_consume_struct(&msgs, U32);
          U8 *keys_array = buf_consume_array(&msgs, U8, keys_array_count);
          Unused(serial);
          Unused(keys_array);

          Wayland_Id *wl_surface = wayland__id_from_list(wayland_state->first_wl_surface, wl_surface_id);
          Assert(wl_surface);
          Wayland_Window *focused_window = wl_surface->window;
          wayland_state->focused_window = focused_window;
	  continue;
        }
        else
	{
          // TODO: unhandled opcode
        }
      }
      else if(object_id == wayland_state->wl_display->id)
      {
        if(opcode == wl_display_error_opcode)
	{
	  fprintf(stderr, "display_error\n");
          U32 error_object_id = *buf_consume_struct(&msgs, U32);
          U32 error_code = *buf_consume_struct(&msgs, U32);
          U32 error_string_count = *buf_consume_struct(&msgs, U32);
          U8 *error_string = buf_consume_array(&msgs, U8, error_string_count);
          // TODO: better logging
          fprintf(stderr, "ERROR: object %u, code %u: %.*s\n",
                  error_object_id, error_code, (int)error_string_count, error_string);
	  continue;
        }
	else
	{
          // TODO: unhandled opcode
        }
      }
      // NOTE: check which per-window id list the message id is on
      else
      {
        Wayland_Id *xdg_surface = wayland__id_from_list(wayland_state->first_xdg_surface, object_id);
        if(xdg_surface)
	{
          if(opcode == xdg_surface_configure_opcode)
	  {
            U32 serial = *buf_consume_struct(&msgs, U32);
            if(xdg_surface_ack_configure(xdg_surface->id, serial))
	    {
	      fprintf(stderr, "xdg_surface_configure\n");
              continue;
            }
	    else
	    {
              // TODO: log error
            }
          }
          else
	  {
            // TODO: unhandled opcode
          }
        }

        Wayland_Id *xdg_toplevel = wayland__id_from_list(wayland_state->first_xdg_toplevel, object_id);
        if(xdg_toplevel)
	{
          Wayland_Window *window = xdg_toplevel->window;
          if(opcode == xdg_toplevel_close_opcode)
	  {
	    fprintf(stderr, "toplevel_close\n");
            event = gfx__event_list_push_new(arena, &result, Os_EventKind_close);
            event->window = wayland__os_handle_from_window(window);
            continue;
          }
          else if(opcode == xdg_toplevel_configure_opcode)
	  {
	    fprintf(stderr, "toplevel_configure\n");
            S32 width = *buf_consume_struct(&msgs, S32);
            S32 height = *buf_consume_struct(&msgs, S32);
	    U32 states_size = *buf_consume_struct(&msgs, U32);
	    U8 *states = buf_consume_array(&msgs, U8, AlignPow2(states_size, 4));
	    Unused(states);

            /* U32 states_size = body.size; */
            /* Buffer states = {.size = states_size, .mem = buf_consume_array(&body, U8, states_size)}; */
            /* Unused(states); */

            window->width = width;
            window->height = height;
	    continue;
          }
        }

        Wayland_Id *wl_buffer = wayland__id_from_list(wayland_state->first_wl_buffer, object_id);
        if(wl_buffer)
	{
          Wayland_Window *window = wl_buffer->window;
          if(opcode == wl_buffer_release_opcode)
	  {
	    fprintf(stderr, "buffer_release (id=%u)\n", object_id);
	    DLLRemove(wayland_state->first_wl_buffer, wayland_state->last_wl_buffer, wl_buffer);
	    --wayland_state->wl_buffer_count;
            wayland__release_id(window->buffers[!window->backbuffer_index]);
            wl_buffer_destroy(window->buffers[!window->backbuffer_index]->id);
            window->buffers[!window->backbuffer_index] = 0;
            continue;
          }
	  else
	  {
            // TODO: unhandled opcode
          }
        }

        Wayland_Id *frame_callback =
	  wayland__id_from_list(wayland_state->first_frame_callback, object_id);
        if(frame_callback)
	{
          Wayland_Window *window = frame_callback->window;
          if(opcode == wl_callback_done_opcode)
	  {
	    fprintf(stderr, "frame_callback_done\n");
            U32 data = *buf_consume_struct(&msgs, U32);

            window->last_frame_ms_elapsed = data - window->last_frame_timestamp;
            window->last_frame_timestamp = data;

            DLLRemove(wayland_state->first_frame_callback, wayland_state->last_frame_callback, frame_callback);
            --wayland_state->frame_callback_count;
            wayland__release_id(frame_callback);
            window->frame_callback = 0;
            continue;
          }
	  else
	  {
            // TOOD: unhandled opcode
          }
        }
      }

      // NOTE: consume unhandled message
      fprintf(stderr, "unhandled message, consuming %lu bytes (msgs size=%lu)\n",
	      size - sizeof(*header), msgs.size);
      buf_consume_size(&msgs, size - sizeof(*header));
      fprintf(stderr, "msgs size post = %lu)\n", msgs.size);
    }
  }

  arena_release_scratch(scratch);
  return(result);
}

proc V2S32
wayland_window_get_dim(Wayland_Window *window)
{
  V2S32 result = v2s32(window->width, window->height);
  return(result);
}

// TODO: move to render module
proc B32
wayland_window_begin_frame(Wayland_Window *window)
{
  Wayland_WindowBeginFrameProc *begin_frame = wayland_state->backends[window->enabled_backend].begin_frame;
  B32 result = begin_frame(window);
  return(result);
}

/* proc void */
/* os_window_end_frame(Os_Handle window) */
/* { */
/*   Wayland_Window *wayland_window = wayland_window_from_os_handle(window); */
/*   wayland_end_frame(wayland_window); */
/* } */
