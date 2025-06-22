global U32 wl_display_sync_opcode                               =  0; // request
global U32 wl_display_get_registry_opcode                       =  1; // request
global U32 wl_display_error_opcode                              =  0; // event
global U32 wl_display_delete_id_opcode                          =  1; // event
global U32 wl_registry_bind_opcode                              =  0; // request
global U32 wl_registry_global_opcode                            =  0; // event
global U32 wl_registry_global_remove_opcode                     =  1; // event
global U32 wl_callback_done_opcode                              =  0; // event
global U32 wl_compositor_create_surface_opcode                  =  0; // request
global U32 wl_compositor_create_region_opcode                   =  1; // request
global U32 wl_shm_pool_create_buffer_opcode                     =  0; // request
global U32 wl_shm_pool_destroy_opcode                           =  1; // request
global U32 wl_shm_pool_resize_opcode                            =  2; // request
global U32 wl_shm_create_pool_opcode                            =  0; // request
global U32 wl_shm_format_opcode                                 =  0; // event
global U32 wl_shm_release_opcode                                =  1; // request
global U32 wl_buffer_destroy_opcode                             =  0; // request
global U32 wl_buffer_release_opcode                             =  0; // event
global U32 wl_data_offer_accept_opcode                          =  0; // request
global U32 wl_data_offer_receive_opcode                         =  1; // request
global U32 wl_data_offer_destroy_opcode                         =  2; // request
global U32 wl_data_offer_offer_opcode                           =  0; // event
global U32 wl_data_offer_finish_opcode                          =  3; // request
global U32 wl_data_offer_set_actions_opcode                     =  4; // request
global U32 wl_data_offer_source_actions_opcode                  =  1; // event
global U32 wl_data_offer_action_opcode                          =  2; // event
global U32 wl_data_source_offer_opcode                          =  0; // request
global U32 wl_data_source_destroy_opcode                        =  1; // request
global U32 wl_data_source_target_opcode                         =  0; // event
global U32 wl_data_source_send_opcode                           =  1; // event
global U32 wl_data_source_cancelled_opcode                      =  2; // event
global U32 wl_data_source_set_actions_opcode                    =  2; // request
global U32 wl_data_source_dnd_drop_performed_opcode             =  3; // event
global U32 wl_data_source_dnd_finished_opcode                   =  4; // event
global U32 wl_data_source_action_opcode                         =  5; // event
global U32 wl_data_device_start_drag_opcode                     =  0; // request
global U32 wl_data_device_set_selection_opcode                  =  1; // request
global U32 wl_data_device_data_offer_opcode                     =  0; // event
global U32 wl_data_device_enter_opcode                          =  1; // event
global U32 wl_data_device_leave_opcode                          =  2; // event
global U32 wl_data_device_motion_opcode                         =  3; // event
global U32 wl_data_device_drop_opcode                           =  4; // event
global U32 wl_data_device_selection_opcode                      =  5; // event
global U32 wl_data_device_release_opcode                        =  2; // request
global U32 wl_data_device_manager_create_data_source_opcode     =  0; // request
global U32 wl_data_device_manager_get_data_device_opcode        =  1; // request
global U32 wl_shell_get_shell_surface_opcode                    =  0; // request
global U32 wl_shell_surface_pong_opcode                         =  0; // request
global U32 wl_shell_surface_move_opcode                         =  1; // request
global U32 wl_shell_surface_resize_opcode                       =  2; // request
global U32 wl_shell_surface_set_toplevel_opcode                 =  3; // request
global U32 wl_shell_surface_set_transient_opcode                =  4; // request
global U32 wl_shell_surface_set_fullscreen_opcode               =  5; // request
global U32 wl_shell_surface_set_popup_opcode                    =  6; // request
global U32 wl_shell_surface_set_maximized_opcode                =  7; // request
global U32 wl_shell_surface_set_title_opcode                    =  8; // request
global U32 wl_shell_surface_set_class_opcode                    =  9; // request
global U32 wl_shell_surface_ping_opcode                         =  0; // event
global U32 wl_shell_surface_configure_opcode                    =  1; // event
global U32 wl_shell_surface_popup_done_opcode                   =  2; // event
global U32 wl_surface_destroy_opcode                            =  0; // request
global U32 wl_surface_attach_opcode                             =  1; // request
global U32 wl_surface_damage_opcode                             =  2; // request
global U32 wl_surface_frame_opcode                              =  3; // request
global U32 wl_surface_set_opaque_region_opcode                  =  4; // request
global U32 wl_surface_set_input_region_opcode                   =  5; // request
global U32 wl_surface_commit_opcode                             =  6; // request
global U32 wl_surface_enter_opcode                              =  0; // event
global U32 wl_surface_leave_opcode                              =  1; // event
global U32 wl_surface_set_buffer_transform_opcode               =  7; // request
global U32 wl_surface_set_buffer_scale_opcode                   =  8; // request
global U32 wl_surface_damage_buffer_opcode                      =  9; // request
global U32 wl_surface_offset_opcode                             = 10; // request
global U32 wl_surface_preferred_buffer_scale_opcode             =  2; // event
global U32 wl_surface_preferred_buffer_transform_opcode         =  3; // event
global U32 wl_seat_capabilities_opcode                          =  0; // event
global U32 wl_seat_get_pointer_opcode                           =  0; // request
global U32 wl_seat_get_keyboard_opcode                          =  1; // request
global U32 wl_seat_get_touch_opcode                             =  2; // request
global U32 wl_seat_name_opcode                                  =  1; // event
global U32 wl_seat_release_opcode                               =  3; // request
global U32 wl_pointer_set_cursor_opcode                         =  0; // request
global U32 wl_pointer_enter_opcode                              =  0; // event
global U32 wl_pointer_leave_opcode                              =  1; // event
global U32 wl_pointer_motion_opcode                             =  2; // event
global U32 wl_pointer_button_opcode                             =  3; // event
global U32 wl_pointer_axis_opcode                               =  4; // event
global U32 wl_pointer_release_opcode                            =  1; // request
global U32 wl_pointer_frame_opcode                              =  5; // event
global U32 wl_pointer_axis_source_opcode                        =  6; // event
global U32 wl_pointer_axis_stop_opcode                          =  7; // event
global U32 wl_pointer_axis_discrete_opcode                      =  8; // event
global U32 wl_pointer_axis_value120_opcode                      =  9; // event
global U32 wl_pointer_axis_relative_direction_opcode            = 10; // event
global U32 wl_keyboard_keymap_opcode                            =  0; // event
global U32 wl_keyboard_enter_opcode                             =  1; // event
global U32 wl_keyboard_leave_opcode                             =  2; // event
global U32 wl_keyboard_key_opcode                               =  3; // event
global U32 wl_keyboard_modifiers_opcode                         =  4; // event
global U32 wl_keyboard_release_opcode                           =  0; // request
global U32 wl_keyboard_repeat_info_opcode                       =  5; // event
global U32 wl_touch_down_opcode                                 =  0; // event
global U32 wl_touch_up_opcode                                   =  1; // event
global U32 wl_touch_motion_opcode                               =  2; // event
global U32 wl_touch_frame_opcode                                =  3; // event
global U32 wl_touch_cancel_opcode                               =  4; // event
global U32 wl_touch_release_opcode                              =  0; // request
global U32 wl_touch_shape_opcode                                =  5; // event
global U32 wl_touch_orientation_opcode                          =  6; // event
global U32 wl_output_geometry_opcode                            =  0; // event
global U32 wl_output_mode_opcode                                =  1; // event
global U32 wl_output_done_opcode                                =  2; // event
global U32 wl_output_scale_opcode                               =  3; // event
global U32 wl_output_release_opcode                             =  0; // request
global U32 wl_output_name_opcode                                =  4; // event
global U32 wl_output_description_opcode                         =  5; // event
global U32 wl_region_destroy_opcode                             =  0; // request
global U32 wl_region_add_opcode                                 =  1; // request
global U32 wl_region_subtract_opcode                            =  2; // request
global U32 wl_subcompositor_destroy_opcode                      =  0; // request
global U32 wl_subcompositor_get_subsurface_opcode               =  1; // request
global U32 wl_subsurface_destroy_opcode                         =  0; // request
global U32 wl_subsurface_set_position_opcode                    =  1; // request
global U32 wl_subsurface_place_above_opcode                     =  2; // request
global U32 wl_subsurface_place_below_opcode                     =  3; // request
global U32 wl_subsurface_set_sync_opcode                        =  4; // request
global U32 wl_subsurface_set_desync_opcode                      =  5; // request
global U32 xdg_wm_base_destroy_opcode                           =  0; // request
global U32 xdg_wm_base_create_positioner_opcode                 =  1; // request
global U32 xdg_wm_base_get_xdg_surface_opcode                   =  2; // request
global U32 xdg_wm_base_pong_opcode                              =  3; // request
global U32 xdg_wm_base_ping_opcode                              =  0; // event
global U32 xdg_positioner_destroy_opcode                        =  0; // request
global U32 xdg_positioner_set_size_opcode                       =  1; // request
global U32 xdg_positioner_set_anchor_rect_opcode                =  2; // request
global U32 xdg_positioner_set_anchor_opcode                     =  3; // request
global U32 xdg_positioner_set_gravity_opcode                    =  4; // request
global U32 xdg_positioner_set_constraint_adjustment_opcode      =  5; // request
global U32 xdg_positioner_set_offset_opcode                     =  6; // request
global U32 xdg_positioner_set_reactive_opcode                   =  7; // request
global U32 xdg_positioner_set_parent_size_opcode                =  8; // request
global U32 xdg_positioner_set_parent_configure_opcode           =  9; // request
global U32 xdg_surface_destroy_opcode                           =  0; // request
global U32 xdg_surface_get_toplevel_opcode                      =  1; // request
global U32 xdg_surface_get_popup_opcode                         =  2; // request
global U32 xdg_surface_set_window_geometry_opcode               =  3; // request
global U32 xdg_surface_ack_configure_opcode                     =  4; // request
global U32 xdg_surface_configure_opcode                         =  0; // event
global U32 xdg_toplevel_destroy_opcode                          =  0; // request
global U32 xdg_toplevel_set_parent_opcode                       =  1; // request
global U32 xdg_toplevel_set_title_opcode                        =  2; // request
global U32 xdg_toplevel_set_app_id_opcode                       =  3; // request
global U32 xdg_toplevel_show_window_menu_opcode                 =  4; // request
global U32 xdg_toplevel_move_opcode                             =  5; // request
global U32 xdg_toplevel_resize_opcode                           =  6; // request
global U32 xdg_toplevel_set_max_size_opcode                     =  7; // request
global U32 xdg_toplevel_set_min_size_opcode                     =  8; // request
global U32 xdg_toplevel_set_maximized_opcode                    =  9; // request
global U32 xdg_toplevel_unset_maximized_opcode                  = 10; // request
global U32 xdg_toplevel_set_fullscreen_opcode                   = 11; // request
global U32 xdg_toplevel_unset_fullscreen_opcode                 = 12; // request
global U32 xdg_toplevel_set_minimized_opcode                    = 13; // request
global U32 xdg_toplevel_configure_opcode                        =  0; // event
global U32 xdg_toplevel_close_opcode                            =  1; // event
global U32 xdg_toplevel_configure_bounds_opcode                 =  2; // event
global U32 xdg_toplevel_wm_capabilities_opcode                  =  3; // event
global U32 xdg_popup_destroy_opcode                             =  0; // request
global U32 xdg_popup_grab_opcode                                =  1; // request
global U32 xdg_popup_configure_opcode                           =  0; // event
global U32 xdg_popup_popup_done_opcode                          =  1; // event
global U32 xdg_popup_reposition_opcode                          =  2; // request
global U32 xdg_popup_repositioned_opcode                        =  2; // event

// NOTE: message request functions
proc B32
wl_display_sync(U32 callback)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_display_id;
  message_header->opcode = wl_display_sync_opcode;

  *arena_push_struct(scratch.arena, U32) = callback;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_display_get_registry(U32 registry)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_display_id;
  message_header->opcode = wl_display_get_registry_opcode;

  *arena_push_struct(scratch.arena, U32) = registry;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_registry_bind(U32 name, String8 interface, U32 version, U32 id)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_registry_id;
  message_header->opcode = wl_registry_bind_opcode;

  *arena_push_struct(scratch.arena, U32) = name;
  *arena_push_struct(scratch.arena, U32) = interface.count + 1;
  arena_push_str8_copy(scratch.arena, interface);
  *arena_push_struct(scratch.arena, U32) = version;
  *arena_push_struct(scratch.arena, U32) = id;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_compositor_create_surface(U32 id)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_compositor_id;
  message_header->opcode = wl_compositor_create_surface_opcode;

  *arena_push_struct(scratch.arena, U32) = id;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_compositor_create_region(U32 id)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_compositor_id;
  message_header->opcode = wl_compositor_create_region_opcode;

  *arena_push_struct(scratch.arena, U32) = id;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_shm_pool_create_buffer(U32 id, S32 offset, S32 width, S32 height, S32 stride, U32 format)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_shm_pool_id;
  message_header->opcode = wl_shm_pool_create_buffer_opcode;

  *arena_push_struct(scratch.arena, U32) = id;
  *arena_push_struct(scratch.arena, S32) = offset;
  *arena_push_struct(scratch.arena, S32) = width;
  *arena_push_struct(scratch.arena, S32) = height;
  *arena_push_struct(scratch.arena, S32) = stride;
  *arena_push_struct(scratch.arena, U32) = format;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_shm_pool_destroy()
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_shm_pool_id;
  message_header->opcode = wl_shm_pool_destroy_opcode;


  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_shm_pool_resize(S32 size)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_shm_pool_id;
  message_header->opcode = wl_shm_pool_resize_opcode;

  *arena_push_struct(scratch.arena, S32) = size;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_shm_create_pool(U32 id, S32 size)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_shm_id;
  message_header->opcode = wl_shm_create_pool_opcode;

  *arena_push_struct(scratch.arena, U32) = id;
  *arena_push_struct(scratch.arena, S32) = size;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  U64 buffer_len = CMSG_SPACE(sizeof(wayland_state.shared_memory_handle));
  U8 *buffer = arena_push_array(scratch.arena, U8, buffer_len);

  struct iovec io = {.iov_base = message_header, .iov_len = message_header->message_size};
  struct msghdr socket_msg = {.msg_iov = &io, .msg_iovlen = 1, .msg_control = buffer, .msg_controllen = buffer_len};

  struct cmsghdr *cmsg = CMSG_FIRSTHDR(&socket_msg);
  cmsg->cmsg_level = SOL_SOCKET;
  cmsg->cmsg_type = SCM_RIGHTS;
  cmsg->cmsg_len = buffer_len;

  *((int *)CMSG_DATA(cmsg)) = wayland_state.shared_memory_handle;
  socket_msg.msg_controllen = CMSG_SPACE(sizeof(wayland_state.shared_memory_handle));

  int send_size = sendmsg(wayland_state.display_socket_handle, &socket_msg, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_shm_release()
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_shm_id;
  message_header->opcode = wl_shm_release_opcode;


  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_buffer_destroy()
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_buffer_id;
  message_header->opcode = wl_buffer_destroy_opcode;


  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_data_offer_accept(U32 serial, String8 mime_type)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_data_offer_id;
  message_header->opcode = wl_data_offer_accept_opcode;

  *arena_push_struct(scratch.arena, U32) = serial;
  *arena_push_struct(scratch.arena, U32) = mime_type.count + 1;
  arena_push_str8_copy(scratch.arena, mime_type);

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_data_offer_receive(String8 mime_type)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_data_offer_id;
  message_header->opcode = wl_data_offer_receive_opcode;

  *arena_push_struct(scratch.arena, U32) = mime_type.count + 1;
  arena_push_str8_copy(scratch.arena, mime_type);

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  U64 buffer_len = CMSG_SPACE(sizeof(wayland_state.shared_memory_handle));
  U8 *buffer = arena_push_array(scratch.arena, U8, buffer_len);

  struct iovec io = {.iov_base = message_header, .iov_len = message_header->message_size};
  struct msghdr socket_msg = {.msg_iov = &io, .msg_iovlen = 1, .msg_control = buffer, .msg_controllen = buffer_len};

  struct cmsghdr *cmsg = CMSG_FIRSTHDR(&socket_msg);
  cmsg->cmsg_level = SOL_SOCKET;
  cmsg->cmsg_type = SCM_RIGHTS;
  cmsg->cmsg_len = buffer_len;

  *((int *)CMSG_DATA(cmsg)) = wayland_state.shared_memory_handle;
  socket_msg.msg_controllen = CMSG_SPACE(sizeof(wayland_state.shared_memory_handle));

  int send_size = sendmsg(wayland_state.display_socket_handle, &socket_msg, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_data_offer_destroy()
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_data_offer_id;
  message_header->opcode = wl_data_offer_destroy_opcode;


  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_data_offer_finish()
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_data_offer_id;
  message_header->opcode = wl_data_offer_finish_opcode;


  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_data_offer_set_actions(U32 dnd_actions, U32 preferred_action)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_data_offer_id;
  message_header->opcode = wl_data_offer_set_actions_opcode;

  *arena_push_struct(scratch.arena, U32) = dnd_actions;
  *arena_push_struct(scratch.arena, U32) = preferred_action;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_data_source_offer(String8 mime_type)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_data_source_id;
  message_header->opcode = wl_data_source_offer_opcode;

  *arena_push_struct(scratch.arena, U32) = mime_type.count + 1;
  arena_push_str8_copy(scratch.arena, mime_type);

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_data_source_destroy()
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_data_source_id;
  message_header->opcode = wl_data_source_destroy_opcode;


  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_data_source_set_actions(U32 dnd_actions)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_data_source_id;
  message_header->opcode = wl_data_source_set_actions_opcode;

  *arena_push_struct(scratch.arena, U32) = dnd_actions;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_data_device_start_drag(U32 source, U32 origin, U32 icon, U32 serial)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_data_device_id;
  message_header->opcode = wl_data_device_start_drag_opcode;

  *arena_push_struct(scratch.arena, U32) = source;
  *arena_push_struct(scratch.arena, U32) = origin;
  *arena_push_struct(scratch.arena, U32) = icon;
  *arena_push_struct(scratch.arena, U32) = serial;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_data_device_set_selection(U32 source, U32 serial)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_data_device_id;
  message_header->opcode = wl_data_device_set_selection_opcode;

  *arena_push_struct(scratch.arena, U32) = source;
  *arena_push_struct(scratch.arena, U32) = serial;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_data_device_release()
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_data_device_id;
  message_header->opcode = wl_data_device_release_opcode;


  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_data_device_manager_create_data_source(U32 id)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_data_device_manager_id;
  message_header->opcode = wl_data_device_manager_create_data_source_opcode;

  *arena_push_struct(scratch.arena, U32) = id;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_data_device_manager_get_data_device(U32 id, U32 seat)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_data_device_manager_id;
  message_header->opcode = wl_data_device_manager_get_data_device_opcode;

  *arena_push_struct(scratch.arena, U32) = id;
  *arena_push_struct(scratch.arena, U32) = seat;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_shell_get_shell_surface(U32 id, U32 surface)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_shell_id;
  message_header->opcode = wl_shell_get_shell_surface_opcode;

  *arena_push_struct(scratch.arena, U32) = id;
  *arena_push_struct(scratch.arena, U32) = surface;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_shell_surface_pong(U32 serial)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_shell_surface_id;
  message_header->opcode = wl_shell_surface_pong_opcode;

  *arena_push_struct(scratch.arena, U32) = serial;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_shell_surface_move(U32 seat, U32 serial)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_shell_surface_id;
  message_header->opcode = wl_shell_surface_move_opcode;

  *arena_push_struct(scratch.arena, U32) = seat;
  *arena_push_struct(scratch.arena, U32) = serial;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_shell_surface_resize(U32 seat, U32 serial, U32 edges)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_shell_surface_id;
  message_header->opcode = wl_shell_surface_resize_opcode;

  *arena_push_struct(scratch.arena, U32) = seat;
  *arena_push_struct(scratch.arena, U32) = serial;
  *arena_push_struct(scratch.arena, U32) = edges;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_shell_surface_set_toplevel()
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_shell_surface_id;
  message_header->opcode = wl_shell_surface_set_toplevel_opcode;


  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_shell_surface_set_transient(U32 parent, S32 x, S32 y, U32 flags)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_shell_surface_id;
  message_header->opcode = wl_shell_surface_set_transient_opcode;

  *arena_push_struct(scratch.arena, U32) = parent;
  *arena_push_struct(scratch.arena, S32) = x;
  *arena_push_struct(scratch.arena, S32) = y;
  *arena_push_struct(scratch.arena, U32) = flags;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_shell_surface_set_fullscreen(U32 method, U32 framerate, U32 output)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_shell_surface_id;
  message_header->opcode = wl_shell_surface_set_fullscreen_opcode;

  *arena_push_struct(scratch.arena, U32) = method;
  *arena_push_struct(scratch.arena, U32) = framerate;
  *arena_push_struct(scratch.arena, U32) = output;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_shell_surface_set_popup(U32 seat, U32 serial, U32 parent, S32 x, S32 y, U32 flags)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_shell_surface_id;
  message_header->opcode = wl_shell_surface_set_popup_opcode;

  *arena_push_struct(scratch.arena, U32) = seat;
  *arena_push_struct(scratch.arena, U32) = serial;
  *arena_push_struct(scratch.arena, U32) = parent;
  *arena_push_struct(scratch.arena, S32) = x;
  *arena_push_struct(scratch.arena, S32) = y;
  *arena_push_struct(scratch.arena, U32) = flags;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_shell_surface_set_maximized(U32 output)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_shell_surface_id;
  message_header->opcode = wl_shell_surface_set_maximized_opcode;

  *arena_push_struct(scratch.arena, U32) = output;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_shell_surface_set_title(String8 title)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_shell_surface_id;
  message_header->opcode = wl_shell_surface_set_title_opcode;

  *arena_push_struct(scratch.arena, U32) = title.count + 1;
  arena_push_str8_copy(scratch.arena, title);

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_shell_surface_set_class(String8 class_)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_shell_surface_id;
  message_header->opcode = wl_shell_surface_set_class_opcode;

  *arena_push_struct(scratch.arena, U32) = class_.count + 1;
  arena_push_str8_copy(scratch.arena, class_);

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_surface_destroy()
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_surface_id;
  message_header->opcode = wl_surface_destroy_opcode;


  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_surface_attach(U32 buffer, S32 x, S32 y)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_surface_id;
  message_header->opcode = wl_surface_attach_opcode;

  *arena_push_struct(scratch.arena, U32) = buffer;
  *arena_push_struct(scratch.arena, S32) = x;
  *arena_push_struct(scratch.arena, S32) = y;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_surface_damage(S32 x, S32 y, S32 width, S32 height)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_surface_id;
  message_header->opcode = wl_surface_damage_opcode;

  *arena_push_struct(scratch.arena, S32) = x;
  *arena_push_struct(scratch.arena, S32) = y;
  *arena_push_struct(scratch.arena, S32) = width;
  *arena_push_struct(scratch.arena, S32) = height;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_surface_frame(U32 callback)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_surface_id;
  message_header->opcode = wl_surface_frame_opcode;

  *arena_push_struct(scratch.arena, U32) = callback;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_surface_set_opaque_region(U32 region)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_surface_id;
  message_header->opcode = wl_surface_set_opaque_region_opcode;

  *arena_push_struct(scratch.arena, U32) = region;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_surface_set_input_region(U32 region)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_surface_id;
  message_header->opcode = wl_surface_set_input_region_opcode;

  *arena_push_struct(scratch.arena, U32) = region;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_surface_commit()
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_surface_id;
  message_header->opcode = wl_surface_commit_opcode;


  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_surface_set_buffer_transform(S32 transform)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_surface_id;
  message_header->opcode = wl_surface_set_buffer_transform_opcode;

  *arena_push_struct(scratch.arena, S32) = transform;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_surface_set_buffer_scale(S32 scale)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_surface_id;
  message_header->opcode = wl_surface_set_buffer_scale_opcode;

  *arena_push_struct(scratch.arena, S32) = scale;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_surface_damage_buffer(S32 x, S32 y, S32 width, S32 height)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_surface_id;
  message_header->opcode = wl_surface_damage_buffer_opcode;

  *arena_push_struct(scratch.arena, S32) = x;
  *arena_push_struct(scratch.arena, S32) = y;
  *arena_push_struct(scratch.arena, S32) = width;
  *arena_push_struct(scratch.arena, S32) = height;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_surface_offset(S32 x, S32 y)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_surface_id;
  message_header->opcode = wl_surface_offset_opcode;

  *arena_push_struct(scratch.arena, S32) = x;
  *arena_push_struct(scratch.arena, S32) = y;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_seat_get_pointer(U32 id)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_seat_id;
  message_header->opcode = wl_seat_get_pointer_opcode;

  *arena_push_struct(scratch.arena, U32) = id;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_seat_get_keyboard(U32 id)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_seat_id;
  message_header->opcode = wl_seat_get_keyboard_opcode;

  *arena_push_struct(scratch.arena, U32) = id;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_seat_get_touch(U32 id)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_seat_id;
  message_header->opcode = wl_seat_get_touch_opcode;

  *arena_push_struct(scratch.arena, U32) = id;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_seat_release()
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_seat_id;
  message_header->opcode = wl_seat_release_opcode;


  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_pointer_set_cursor(U32 serial, U32 surface, S32 hotspot_x, S32 hotspot_y)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_pointer_id;
  message_header->opcode = wl_pointer_set_cursor_opcode;

  *arena_push_struct(scratch.arena, U32) = serial;
  *arena_push_struct(scratch.arena, U32) = surface;
  *arena_push_struct(scratch.arena, S32) = hotspot_x;
  *arena_push_struct(scratch.arena, S32) = hotspot_y;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_pointer_release()
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_pointer_id;
  message_header->opcode = wl_pointer_release_opcode;


  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_keyboard_release()
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_keyboard_id;
  message_header->opcode = wl_keyboard_release_opcode;


  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_touch_release()
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_touch_id;
  message_header->opcode = wl_touch_release_opcode;


  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_output_release()
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_output_id;
  message_header->opcode = wl_output_release_opcode;


  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_region_destroy()
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_region_id;
  message_header->opcode = wl_region_destroy_opcode;


  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_region_add(S32 x, S32 y, S32 width, S32 height)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_region_id;
  message_header->opcode = wl_region_add_opcode;

  *arena_push_struct(scratch.arena, S32) = x;
  *arena_push_struct(scratch.arena, S32) = y;
  *arena_push_struct(scratch.arena, S32) = width;
  *arena_push_struct(scratch.arena, S32) = height;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_region_subtract(S32 x, S32 y, S32 width, S32 height)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_region_id;
  message_header->opcode = wl_region_subtract_opcode;

  *arena_push_struct(scratch.arena, S32) = x;
  *arena_push_struct(scratch.arena, S32) = y;
  *arena_push_struct(scratch.arena, S32) = width;
  *arena_push_struct(scratch.arena, S32) = height;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_subcompositor_destroy()
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_subcompositor_id;
  message_header->opcode = wl_subcompositor_destroy_opcode;


  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_subcompositor_get_subsurface(U32 id, U32 surface, U32 parent)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_subcompositor_id;
  message_header->opcode = wl_subcompositor_get_subsurface_opcode;

  *arena_push_struct(scratch.arena, U32) = id;
  *arena_push_struct(scratch.arena, U32) = surface;
  *arena_push_struct(scratch.arena, U32) = parent;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_subsurface_destroy()
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_subsurface_id;
  message_header->opcode = wl_subsurface_destroy_opcode;


  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_subsurface_set_position(S32 x, S32 y)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_subsurface_id;
  message_header->opcode = wl_subsurface_set_position_opcode;

  *arena_push_struct(scratch.arena, S32) = x;
  *arena_push_struct(scratch.arena, S32) = y;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_subsurface_place_above(U32 sibling)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_subsurface_id;
  message_header->opcode = wl_subsurface_place_above_opcode;

  *arena_push_struct(scratch.arena, U32) = sibling;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_subsurface_place_below(U32 sibling)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_subsurface_id;
  message_header->opcode = wl_subsurface_place_below_opcode;

  *arena_push_struct(scratch.arena, U32) = sibling;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_subsurface_set_sync()
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_subsurface_id;
  message_header->opcode = wl_subsurface_set_sync_opcode;


  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wl_subsurface_set_desync()
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.wl_subsurface_id;
  message_header->opcode = wl_subsurface_set_desync_opcode;


  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_wm_base_destroy()
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_wm_base_id;
  message_header->opcode = xdg_wm_base_destroy_opcode;


  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_wm_base_create_positioner(U32 id)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_wm_base_id;
  message_header->opcode = xdg_wm_base_create_positioner_opcode;

  *arena_push_struct(scratch.arena, U32) = id;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_wm_base_get_xdg_surface(U32 id, U32 surface)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_wm_base_id;
  message_header->opcode = xdg_wm_base_get_xdg_surface_opcode;

  *arena_push_struct(scratch.arena, U32) = id;
  *arena_push_struct(scratch.arena, U32) = surface;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_wm_base_pong(U32 serial)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_wm_base_id;
  message_header->opcode = xdg_wm_base_pong_opcode;

  *arena_push_struct(scratch.arena, U32) = serial;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_positioner_destroy()
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_positioner_id;
  message_header->opcode = xdg_positioner_destroy_opcode;


  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_positioner_set_size(S32 width, S32 height)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_positioner_id;
  message_header->opcode = xdg_positioner_set_size_opcode;

  *arena_push_struct(scratch.arena, S32) = width;
  *arena_push_struct(scratch.arena, S32) = height;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_positioner_set_anchor_rect(S32 x, S32 y, S32 width, S32 height)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_positioner_id;
  message_header->opcode = xdg_positioner_set_anchor_rect_opcode;

  *arena_push_struct(scratch.arena, S32) = x;
  *arena_push_struct(scratch.arena, S32) = y;
  *arena_push_struct(scratch.arena, S32) = width;
  *arena_push_struct(scratch.arena, S32) = height;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_positioner_set_anchor(U32 anchor)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_positioner_id;
  message_header->opcode = xdg_positioner_set_anchor_opcode;

  *arena_push_struct(scratch.arena, U32) = anchor;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_positioner_set_gravity(U32 gravity)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_positioner_id;
  message_header->opcode = xdg_positioner_set_gravity_opcode;

  *arena_push_struct(scratch.arena, U32) = gravity;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_positioner_set_constraint_adjustment(U32 constraint_adjustment)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_positioner_id;
  message_header->opcode = xdg_positioner_set_constraint_adjustment_opcode;

  *arena_push_struct(scratch.arena, U32) = constraint_adjustment;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_positioner_set_offset(S32 x, S32 y)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_positioner_id;
  message_header->opcode = xdg_positioner_set_offset_opcode;

  *arena_push_struct(scratch.arena, S32) = x;
  *arena_push_struct(scratch.arena, S32) = y;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_positioner_set_reactive()
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_positioner_id;
  message_header->opcode = xdg_positioner_set_reactive_opcode;


  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_positioner_set_parent_size(S32 parent_width, S32 parent_height)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_positioner_id;
  message_header->opcode = xdg_positioner_set_parent_size_opcode;

  *arena_push_struct(scratch.arena, S32) = parent_width;
  *arena_push_struct(scratch.arena, S32) = parent_height;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_positioner_set_parent_configure(U32 serial)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_positioner_id;
  message_header->opcode = xdg_positioner_set_parent_configure_opcode;

  *arena_push_struct(scratch.arena, U32) = serial;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_surface_destroy()
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_surface_id;
  message_header->opcode = xdg_surface_destroy_opcode;


  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_surface_get_toplevel(U32 id)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_surface_id;
  message_header->opcode = xdg_surface_get_toplevel_opcode;

  *arena_push_struct(scratch.arena, U32) = id;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_surface_get_popup(U32 id, U32 parent, U32 positioner)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_surface_id;
  message_header->opcode = xdg_surface_get_popup_opcode;

  *arena_push_struct(scratch.arena, U32) = id;
  *arena_push_struct(scratch.arena, U32) = parent;
  *arena_push_struct(scratch.arena, U32) = positioner;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_surface_set_window_geometry(S32 x, S32 y, S32 width, S32 height)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_surface_id;
  message_header->opcode = xdg_surface_set_window_geometry_opcode;

  *arena_push_struct(scratch.arena, S32) = x;
  *arena_push_struct(scratch.arena, S32) = y;
  *arena_push_struct(scratch.arena, S32) = width;
  *arena_push_struct(scratch.arena, S32) = height;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_surface_ack_configure(U32 serial)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_surface_id;
  message_header->opcode = xdg_surface_ack_configure_opcode;

  *arena_push_struct(scratch.arena, U32) = serial;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_toplevel_destroy()
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_toplevel_id;
  message_header->opcode = xdg_toplevel_destroy_opcode;


  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_toplevel_set_parent(U32 parent)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_toplevel_id;
  message_header->opcode = xdg_toplevel_set_parent_opcode;

  *arena_push_struct(scratch.arena, U32) = parent;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_toplevel_set_title(String8 title)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_toplevel_id;
  message_header->opcode = xdg_toplevel_set_title_opcode;

  *arena_push_struct(scratch.arena, U32) = title.count + 1;
  arena_push_str8_copy(scratch.arena, title);

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_toplevel_set_app_id(String8 app_id)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_toplevel_id;
  message_header->opcode = xdg_toplevel_set_app_id_opcode;

  *arena_push_struct(scratch.arena, U32) = app_id.count + 1;
  arena_push_str8_copy(scratch.arena, app_id);

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_toplevel_show_window_menu(U32 seat, U32 serial, S32 x, S32 y)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_toplevel_id;
  message_header->opcode = xdg_toplevel_show_window_menu_opcode;

  *arena_push_struct(scratch.arena, U32) = seat;
  *arena_push_struct(scratch.arena, U32) = serial;
  *arena_push_struct(scratch.arena, S32) = x;
  *arena_push_struct(scratch.arena, S32) = y;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_toplevel_move(U32 seat, U32 serial)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_toplevel_id;
  message_header->opcode = xdg_toplevel_move_opcode;

  *arena_push_struct(scratch.arena, U32) = seat;
  *arena_push_struct(scratch.arena, U32) = serial;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_toplevel_resize(U32 seat, U32 serial, U32 edges)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_toplevel_id;
  message_header->opcode = xdg_toplevel_resize_opcode;

  *arena_push_struct(scratch.arena, U32) = seat;
  *arena_push_struct(scratch.arena, U32) = serial;
  *arena_push_struct(scratch.arena, U32) = edges;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_toplevel_set_max_size(S32 width, S32 height)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_toplevel_id;
  message_header->opcode = xdg_toplevel_set_max_size_opcode;

  *arena_push_struct(scratch.arena, S32) = width;
  *arena_push_struct(scratch.arena, S32) = height;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_toplevel_set_min_size(S32 width, S32 height)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_toplevel_id;
  message_header->opcode = xdg_toplevel_set_min_size_opcode;

  *arena_push_struct(scratch.arena, S32) = width;
  *arena_push_struct(scratch.arena, S32) = height;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_toplevel_set_maximized()
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_toplevel_id;
  message_header->opcode = xdg_toplevel_set_maximized_opcode;


  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_toplevel_unset_maximized()
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_toplevel_id;
  message_header->opcode = xdg_toplevel_unset_maximized_opcode;


  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_toplevel_set_fullscreen(U32 output)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_toplevel_id;
  message_header->opcode = xdg_toplevel_set_fullscreen_opcode;

  *arena_push_struct(scratch.arena, U32) = output;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_toplevel_unset_fullscreen()
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_toplevel_id;
  message_header->opcode = xdg_toplevel_unset_fullscreen_opcode;


  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_toplevel_set_minimized()
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_toplevel_id;
  message_header->opcode = xdg_toplevel_set_minimized_opcode;


  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_popup_destroy()
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_popup_id;
  message_header->opcode = xdg_popup_destroy_opcode;


  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_popup_grab(U32 seat, U32 serial)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_popup_id;
  message_header->opcode = xdg_popup_grab_opcode;

  *arena_push_struct(scratch.arena, U32) = seat;
  *arena_push_struct(scratch.arena, U32) = serial;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
xdg_popup_reposition(U32 positioner, U32 token)
{
  B32 result = 1;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U64 message_start_pos = arena_pos(scratch.arena);
  WaylandMessageHeader *message_header = arena_push_struct(scratch.arena, WaylandMessageHeader);
  message_header->object_id = wayland_state.xdg_popup_id;
  message_header->opcode = xdg_popup_reposition_opcode;

  *arena_push_struct(scratch.arena, U32) = positioner;
  *arena_push_struct(scratch.arena, U32) = token;

  U64 message_end_pos = arena_pos(scratch.arena);
  U32 message_size = message_end_pos - message_start_pos;
  message_header->message_size = AlignPow2(message_size, 4);

  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);
  if(send_size == -1) {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

