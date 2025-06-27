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

  R32 mouse_x;
  R32 mouse_y;

  //U32 box_color;
  //U32 alt_color;
  U32 box_color_index;
  U32 box_colors[2];
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

  // NOTE: if mouse is in box, change its color
  if(app_state->mouse_x >= app_state->box_posx &&
     app_state->mouse_x <= app_state->box_posx + app_state->box_width &&
     app_state->mouse_y >= app_state->box_posy &&
     app_state->mouse_y <= app_state->box_posy + app_state->box_height) {
    app_state->box_color_index = !app_state->box_color_index;
  }

  // NOTE: render state
  U32 box_color = app_state->box_colors[app_state->box_color_index];
  U32 background_color = app_state->background_color;
  U8 *row = (U8 *)pixels;
  for(S32 j = 0; j < app_state->window_height; ++j) {
    U32 *pixel = (U32 *)row;
    for(S32 i = 0; i < app_state->window_width; ++i, ++pixel) {
      if(i >= app_state->box_posx && i <= app_state->box_posx + app_state->box_width &&
	 j >= app_state->box_posy && j <= app_state->box_posy + app_state->box_height) {
	*pixel = box_color;
      }
      else {
	*pixel = background_color;
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
  app_state.box_colors[0] = colorU32_from_rgba(0xFF, 0, 0, 0xFF);
  app_state.box_colors[1] = colorU32_from_rgba(0, 0xFF, 0, 0xFF);
  app_state.background_color = colorU32_from_rgba(0, 0, 0, 0xFF);
  app_state.box_width = 50;
  app_state.box_height = 50;
  app_state.box_vx = 10;
  app_state.box_vy = 10;

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
	  // NOTE: mouse events
	  else if(event.object_id == window->wl_pointer_id &&
		  event.opcode == wl_pointer_motion_opcode) {
	    U32 time = *(U32 *)event.body.mem;
	    U32 surface_x__fixed = *((U32 *)event.body.mem + 1);
	    U32 surface_y__fixed = *((U32 *)event.body.mem + 2);
	    R32 surface_x = (R32)surface_x__fixed / 256.f;
	    R32 surface_y = (R32)surface_y__fixed / 256.f;
	    app_state.mouse_x = surface_x;
	    app_state.mouse_y = surface_y;
	    /* fprintf(stderr, "mouse motion: time=%u, pos=(%08.4f, %08.4f)\n", */
	    /* 	    time, surface_x, surface_y); */
	  }
	  else if(event.object_id == window->wl_pointer_id &&
		  event.opcode == wl_pointer_button_opcode) {
	    U32 *event_body = (U32 *)event.body.mem;
	    U32 serial = event_body[0];
	    U32 time = event_body[1];
	    U32 button = event_body[2];
	    U32 state = event_body[3];
	    fprintf(stderr, "mouse click: time=%u, serial=%u, button=%u, state=%s\n",
		    time, serial, button, state ? "pressed" : "released");
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

  return(0);
}
