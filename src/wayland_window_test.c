#include "base/base.h"
#include "os/os.h"

#include "base/base.c"
#include "os/os.c"

#include <sys/socket.h>
#include <sys/un.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "wayland.h"
#include "wayland.c"

typedef struct AppState
{
  S32 window_width;
  S32 window_height;
  S32 window_stride;

  U32 box_color;
  U32 background_color;

  R32 box_posx;
  R32 box_posy;
  R32 box_width;
  R32 box_height;

  R32 box_vx;
  R32 box_vy;
} AppState;

proc U32
colorU32_from_rgba(U8 r, U8 g, U8 b, U8 a)
{
  U32 result = ((((U32)r) << 0*8) |
		(((U32)g) << 1*8) |
		(((U32)b) << 2*8) |
		(((U32)a) << 3*8));

  return(result);
}

proc void
update_and_render(U32 *pixels, AppState *app_state)
{  
  // NOTE: handle colisions
  if(app_state->box_vx < 0) {
    if(app_state->box_posx + app_state->box_vx < 0) {
      app_state->box_vx = -app_state->box_vx;
    }
  }
  else {
    if(app_state->box_posx + app_state->box_width + app_state->box_vx > app_state->window_width) {
      app_state->box_vx = -app_state->box_vx;
    }
  }

  if(app_state->box_vy < 0) {
    if(app_state->box_posy + app_state->box_vy < 0) {
      app_state->box_vy = -app_state->box_vy;
    }
  }
  else {
    if(app_state->box_posy + app_state->box_height + app_state->box_vy > app_state->window_height) {
      app_state->box_vy = -app_state->box_vy;
    }
  }

  // NOTE: render state
  U8 *row = (U8 *)pixels;
  for(S32 j = 0; j < app_state->window_height; ++j) {
    U32 *pixel = (U32 *)row;
    for(S32 i = 0; i < app_state->window_width; ++i, ++pixel) {
      if(i >= app_state->box_posx && i <= app_state->box_posx + app_state->box_width &&
	 j >= app_state->box_posy && j <= app_state->box_posy + app_state->box_height) {
	*pixel = app_state->box_color;
      }
      else {
	*pixel = app_state->background_color;
      }
    }

    row += app_state->window_stride;
  }

  // NOTE: update state
  app_state->box_posx += app_state->box_vx;
  app_state->box_posy += app_state->box_vy;
}

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  AppState app_state = {0};
  app_state.box_color = colorU32_from_rgba(0xFF, 0, 0, 0xFF);
  app_state.background_color = colorU32_from_rgba(0, 0, 0, 0xFF);
  app_state.box_width = 50;
  app_state.box_height = 50;
  app_state.box_vx = 10;
  app_state.box_vy = 10;

#if 1
  if(wayland_init()) {
    WaylandWindow *window = wayland_open_window(Str8Lit("hello wayland"), 640, 480);
    if(window) {
      app_state.window_width = window->width;
      app_state.window_height = window->height;
      app_state.window_stride = app_state.window_width*sizeof(U32);
      B32 running = 1;
      while(running) {
	// NOTE: poll for events
	WaylandEvent event = {0};
	while(wayland_get_event(window, &event)) {
	  // TODO: cleanup
	  // TODO: handle input
	  // NOTE: acknowledge ping
	  if(event.object_id == window->xdg_wm_base_id &&
	     event.opcode == xdg_wm_base_ping_opcode) {
	    U32 serial = *(U32 *)event.body.mem;
	    if(xdg_wm_base_pong(window->xdg_wm_base_id, serial)) {
	      fprintf(stderr, "**acked ping**\n");
	    }
	  }
	  else if(event.object_id == window->xdg_surface_id &&
	     event.opcode == xdg_surface_configure_opcode) {
	    U32 serial = *(U32 *)event.body.mem;
	    if(xdg_surface_ack_configure(window->xdg_surface_id, serial)) {
	      fprintf(stderr, "**acked ping**\n");
	    }
	  }
	  // NOTE: errors
	  else if(event.object_id == window->wl_display_id &&
		  event.opcode == wl_display_error_opcode) {
	    U32 *event_body = (U32 *)event.body.mem;
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
	    fprintf(stderr, "unhandled message: object=%u, opcode=%u, length=%lu\n",
		    event.object_id, event.opcode, event.body.size);
	  }
	}

	update_and_render((U32 *)window->shared_memory, &app_state);

	if(!wayland_swap_buffers(window)) {
	  Assert(!"FATAL: swap buffers failed");
	}
	// TODO: better frame-rate wait
	usleep(33333);
      }
    }
  }
#else
  //wayland_state_initialize();

  B32 connected = wayland_display_connect();
  if(connected) {
    if(wayland_display_get_registry()) {
      //U32 registry_id = *wayland_interface_object_id_from_name(Str8Lit("wl_registry"));
      printf("registry id: %u\n", wayland_state.wl_registry_id);
      
      U64 shared_memory_size = MB(32);
      if(wayland_allocate_shared_memory(shared_memory_size)) {
	/* Arena *framebuffer_arena = arena_alloc_ex(shared_memory_size, shared_memory_size, */
	/* 					  wayland_state.shared_memory, 0); */
	printf("framebuffer success!\n");

	Arena *frame_arena = arena_alloc();
	B32 running = 1;
	while(running) {
	  U64 message_buffer_size = KB(4);
	  U8 *message_buffer = arena_push_array_z(frame_arena, U8, message_buffer_size);
	  S64 recv_size = recv(wayland_state.display_socket_handle, message_buffer, message_buffer_size, 0);
	  if(recv_size == -1) {
	    running = 0;
	    // TODO: log error: message receive failure
	  }

	  U8 *message = message_buffer;
	  S64 message_len = recv_size;
	  printf("message len: %ld\n", message_len);
	  while(message_len > 0) {
	    wayland_handle_message(&message, &message_len);
	  }
	  
	  if(wayland_state.wl_shm_id && wayland_state.wl_compositor_id && wayland_state.xdg_wm_base_id &&
	     !wayland_state.wl_surface_id) {
	    printf("shm id: %u\n", wayland_state.wl_shm_id);
	    printf("compositor id: %u\n", wayland_state.wl_compositor_id);
	    printf("xdg wm base id: %u\n", wayland_state.xdg_wm_base_id);

	    if(wl_compositor_create_surface(++wayland_current_id)) {
	      wayland_state.wl_surface_id = wayland_current_id;
	      printf("surface id: %u\n", wayland_state.wl_surface_id);
	      
	      if(xdg_wm_base_get_xdg_surface(++wayland_current_id, wayland_state.wl_surface_id)) {
		wayland_state.xdg_surface_id = wayland_current_id;
		printf("xdg surface id: %u\n", wayland_state.xdg_surface_id);

		if(xdg_surface_get_toplevel(++wayland_current_id)) {
		  wayland_state.xdg_toplevel_id = wayland_current_id;
		  printf("xdg toplevel id: %u\n", wayland_state.xdg_toplevel_id);

		  if(wl_surface_commit()) {
		    printf("surface committed\n");
		  }
		}
	      }
	    }	    
	  }

	  if(wayland_state.init_state == WaylandInit_acked) {
	    Assert(wayland_state.wl_surface_id);
	    Assert(wayland_state.xdg_surface_id);
	    Assert(wayland_state.xdg_toplevel_id);

	    if(!wayland_state.wl_shm_pool_id) {
	      if(wl_shm_create_pool(++wayland_current_id,
				    shared_memory_size)) {
		wayland_state.wl_shm_pool_id = wayland_current_id;
		printf("shm pool id: %u\n", wayland_state.wl_shm_pool_id);

		if(!wayland_state.wl_buffer_id) {
		  S32 offset = 0;
		  S32 width = 640;
		  S32 height = 480;
		  S32 stride = width*sizeof(U32);
		  U32 format = 0x34324241; // NOTE: ABGR 32-bit little-endian // TODO: generate enums
		  if(wl_shm_pool_create_buffer(++wayland_current_id,
					       offset, width, height, stride, format)) {
		    wayland_state.wl_buffer_id = wayland_current_id;
		    printf("buffer id: %u\n", wayland_state.wl_buffer_id);

		    /* // NOTE: clear window to black */
		    /* U8 *row = (U8 *)wayland_state.shared_memory; */
		    /* for(S32 j = 0; j < height; ++j, row += stride) { */
		    /*   U32 *pixel = (U32 *)row; */
		    /*   for(S32 i = 0; i < width; ++i, ++pixel) { */
		    /* 	*pixel =  */
		    /*   } */
		    /* } */

		    app_state.window_width = width;
		    app_state.window_height = height;
		    app_state.window_stride = stride;

		    if(wl_surface_attach(wayland_state.wl_buffer_id, 0, 0)) {
		      printf("surface attached\n");
		      if(wl_surface_commit()) {
			printf("surface commited. initialization complete\n");
			wayland_state.init_state = WaylandInit_attached;
		      }
		    }		    		    
		  }
		}
	      }
	    }	    
	  }

	  if(wayland_state.init_state == WaylandInit_attached) {	    

	    // NOTE: handle colisions
	    if(app_state.box_vx < 0) {
	      if(app_state.box_posx + app_state.box_vx < 0) {
		app_state.box_vx = -app_state.box_vx;
	      }
	    }
	    else {
	      if(app_state.box_posx + app_state.box_width + app_state.box_vx > app_state.window_width) {
		app_state.box_vx = -app_state.box_vx;
	      }
	    }

	    if(app_state.box_vy < 0) {
	      if(app_state.box_posy + app_state.box_vy < 0) {
		app_state.box_vy = -app_state.box_vy;
	      }
	    }
	    else {
	      if(app_state.box_posy + app_state.box_height + app_state.box_vy > app_state.window_height) {
		app_state.box_vy = -app_state.box_vy;
	      }
	    }

	    // NOTE: render state
	    U8 *row = (U8 *)wayland_state.shared_memory;
	    for(S32 j = 0; j < app_state.window_height; ++j) {
	      U32 *pixel = (U32 *)row;
	      for(S32 i = 0; i < app_state.window_width; ++i, ++pixel) {
		if(i >= app_state.box_posx && i <= app_state.box_posx + app_state.box_width &&
		   j >= app_state.box_posy && j <= app_state.box_posy + app_state.box_height) {
		  *pixel = app_state.box_color;
		}
		else {
		  *pixel = app_state.background_color;
		}
	      }

	      row += app_state.window_stride;
	    }

	    // NOTE: update state
	    app_state.box_posx += app_state.box_vx;
	    app_state.box_posy += app_state.box_vy;

	    // NOTE: commit frame to be rendered
	    wl_surface_attach(wayland_state.wl_buffer_id, 0, 0);
	    wl_surface_damage(0, 0, app_state.window_width, app_state.window_height);
	    if(wl_surface_commit()) {
	      printf("frame committed\n");
	    }

	    usleep(33333);
	  }
	  
	  arena_clear(frame_arena);
	}
      }
    }
  }
#endif

  return(0);
}
