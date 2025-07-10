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

  V2 mouse_pos;

  U32 box_color_index;
  U32 box_colors[2];
  U32 background_color;

  V2 box_pos;
  V2 box_dim;

  V2 box_v;

  B32 running;
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
  // TODO: handle the case when the box becomes outside the window dimensions
  //       as a result of the window size changing
  if(app_state->running) {
    // NOTE: handle colisions
    if(app_state->box_v.x < 0) {
      if(app_state->box_pos.x + app_state->box_v.x < 0) {
	app_state->box_v.x = -app_state->box_v.x;
      }
    }
    else {
      if(app_state->box_pos.x + app_state->box_dim.x + app_state->box_v.x > app_state->window_width) {
	app_state->box_v.x = -app_state->box_v.x;
      }
    }

    if(app_state->box_v.y < 0) {
      if(app_state->box_pos.y + app_state->box_v.y < 0) {
	app_state->box_v.y = -app_state->box_v.y;
      }
    }
    else {
      if(app_state->box_pos.y + app_state->box_dim.y + app_state->box_v.y > app_state->window_height) {
	app_state->box_v.y = -app_state->box_v.y;
      }
    }

    // NOTE: if mouse is in box, change its color
    if(app_state->mouse_pos.x >= app_state->box_pos.x &&
       app_state->mouse_pos.x <= app_state->box_pos.x + app_state->box_dim.x &&
       app_state->mouse_pos.y >= app_state->box_pos.y &&
       app_state->mouse_pos.y <= app_state->box_pos.y + app_state->box_dim.y) {
      app_state->box_color_index = !app_state->box_color_index;
    }  

    // NOTE: update state
    app_state->box_pos.x += app_state->box_v.x;
    app_state->box_pos.y += app_state->box_v.y;
  }
  
  // NOTE: render state
  U32 box_color = app_state->box_colors[app_state->box_color_index];
  U32 background_color = app_state->background_color;
  U8 *row = (U8 *)pixels;
  for(S32 j = 0; j < app_state->window_height; ++j) {
    U32 *pixel = (U32 *)row;
    for(S32 i = 0; i < app_state->window_width; ++i, ++pixel) {
      if(i >= app_state->box_pos.x && i <= app_state->box_pos.x + app_state->box_dim.x &&
	 j >= app_state->box_pos.y && j <= app_state->box_pos.y + app_state->box_dim.y) {
	*pixel = box_color;
      }
      else {
	*pixel = background_color;
      }
    }

    row += app_state->window_stride;
  }
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
  app_state.box_dim = (V2){.x = 50, .y = 50};
  app_state.box_v = (V2){.x = 10, .y = 10};
  app_state.mouse_pos = (V2){.x = -1, .y = -1};

  if(wayland_init()) {
    WaylandWindow *window = wayland_open_window(Str8Lit("hello wayland"), 640, 480);
    if(window) {
      app_state.running = 1;

      B32 running = 1;
      Arena *frame_arena = arena_alloc();
      while(running) {
	U64 frame_begin_cycles = cpu_get_cycle_count();

	// NOTE: poll for events
	EventList events = wayland_get_events(window);
	Event event = {0};
	while(next_event(&events, &event)) {
	  switch(event.kind) {
	  case EventKind_Close:
	    {
	      running = 0;
	    } break;
	  case EventKind_Move:
	    {
	      app_state.mouse_pos = event.position;
	    } break;
	  case EventKind_Press:
	    {
	      if(event.button == EventButton_KeySpace) {
		app_state.running = !app_state.running;
	      }
	    } break;
	  default: {} break;
	  }
	}

	app_state.window_width = window->width;
	app_state.window_height = window->height;
	app_state.window_stride = app_state.window_width * sizeof(U32);
	update_and_render((U32 *)window->shared_memory, &app_state);

	if(!wayland_end_frame(window)) {
	  Assert(!"FATAL: swap buffers failed");
	}
	arena_clear(frame_arena);

	U64 frame_end_cycles = cpu_get_cycle_count();
	U64 frame_cycles_elapsed = frame_end_cycles - frame_begin_cycles;
	R32 fps = 1000.f / (R32)window->last_frame_ms_elapsed;
	fprintf(stderr, "frames per second: %.2f (%lu cy)\n", fps, frame_cycles_elapsed);
      }

      wayland_close_window(window);
    }
  }

  return(0);
}
