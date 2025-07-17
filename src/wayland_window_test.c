#include "base/base.h"
#include "os/os.h"

#include "base/base.c"
#include "os/os.c"

#include <stdlib.h>
#include <stdio.h>

#include "wayland.h"
#include "wayland.c"

/**
 * TODO:
 * there seems to be bug where we render from the wrong gl texture first
 * don't store the last mouse position inside the window when the mouse goes
   outside the window (get mouse screen position?)
 * scale up example to multiple rectangles
 * pull out the renderer
 * image and text rendering 
 * handle geometry better
 * play sound when boxes collide
 */

typedef struct AppState
{ 
  S32 window_width;
  S32 window_height;
  S32 window_stride;

  V2 mouse_pos;

  U32 box_color_index;
  V4 box_colors[2];
  V4 background_color;

  V2 box_pos; // NOTE: percent of window dim
  V2 box_dim; // NOTE: percent of window dim

  V2 box_v; // NOTE: percent of window dim

  B32 running;

  RenderTarget render_target;
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

proc U32
colorU32_from_v4(V4 c)
{
  U32 result = colorU32_from_rgba(c.r*255, c.g*255, c.b*255, c.a*255);
  return(result);
}

proc V4
colorV4_from_rgba(U8 r, U8 g, U8 b, U8 a)
{
  V4 result = v4(r/255.f, g/255.f, b/255.f, a/255.f);

  return(result);
}

proc void
update_and_render(U32 *pixels, GlFramebuffer gl_framebuffer, AppState *app_state)
{
  V2 box_pos_pixels = v2(app_state->box_pos.x* app_state->window_width,
			 app_state->box_pos.y * app_state->window_height);
  V2 box_dim_pixels = v2(app_state->box_dim.x * app_state->window_width,
			 app_state->box_dim.y * app_state->window_height);

  if(app_state->running) {    
    // NOTE: handle colisions
    if(app_state->box_v.x < 0) {
      if(app_state->box_pos.x + app_state->box_v.x < 0) {
	app_state->box_v.x = -app_state->box_v.x;
      }
    }
    else {
      if(app_state->box_pos.x + app_state->box_dim.x + app_state->box_v.x > 1) {
	app_state->box_v.x = -app_state->box_v.x;
      }
    }

    if(app_state->box_v.y < 0) {
      if(app_state->box_pos.y + app_state->box_v.y < 0) {
	app_state->box_v.y = -app_state->box_v.y;
      }
    }
    else {
      if(app_state->box_pos.y + app_state->box_dim.y + app_state->box_v.y > 1) {
	app_state->box_v.y = -app_state->box_v.y;
      }
    }
    
    // NOTE: if mouse is in box, change its color
    if(app_state->mouse_pos.x >= box_pos_pixels.x &&
       app_state->mouse_pos.x <= box_pos_pixels.x + box_dim_pixels.x &&
       app_state->mouse_pos.y >= box_pos_pixels.y &&
       app_state->mouse_pos.y <= box_pos_pixels.y + box_dim_pixels.y) {
      app_state->box_color_index = !app_state->box_color_index;
    }  

    // NOTE: update state
    app_state->box_pos.x += app_state->box_v.x;
    app_state->box_pos.y += app_state->box_v.y;
  }
  
  // NOTE: render state  
  if(app_state->render_target == RenderTarget_software) {
    U32 box_color = colorU32_from_v4(app_state->box_colors[app_state->box_color_index]);
    U32 background_color = colorU32_from_v4(app_state->background_color);
    
    U8 *row = (U8 *)pixels;
    for(S32 j = 0; j < app_state->window_height; ++j) {
      U32 *pixel = (U32 *)row;
      for(S32 i = 0; i < app_state->window_width; ++i, ++pixel) {
	if(i >= box_pos_pixels.x &&
	   i <= box_pos_pixels.x + box_dim_pixels.x &&
	   j >= box_pos_pixels.y &&
	   j <= box_pos_pixels.y + box_dim_pixels.y) {
	  *pixel = box_color;
	}
	else {
	  *pixel = background_color;
	}
      }

      row += app_state->window_stride;
    }
  } else if(app_state->render_target == RenderTarget_hardware) {
    glBindFramebuffer(GL_FRAMEBUFFER, gl_framebuffer.fbo);
    glViewport(0, 0, app_state->window_width, app_state->window_height);

    V4 bgrnd_color = app_state->background_color;
    glClearColor(bgrnd_color.r, bgrnd_color.g, bgrnd_color.b, bgrnd_color.a);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    V4 box_color = app_state->box_colors[app_state->box_color_index];
    V2 box_pos_gl = v2(2.f*app_state->box_pos.x - 1.f,
		       2.f*app_state->box_pos.y - 1.f);
    V2 box_max_gl = v2(2.f*(app_state->box_pos.x + app_state->box_dim.x) - 1.f,
		       2.f*(app_state->box_pos.y + app_state->box_dim.y) - 1.f);
    glBegin(GL_TRIANGLES);
    {
      glColor4fv(box_color.v);
      glVertex2f(box_pos_gl.x, box_pos_gl.y);
      glColor4fv(box_color.v);
      glVertex2f(box_max_gl.x, box_pos_gl.y);
      glColor4fv(box_color.v);
      glVertex2f(box_pos_gl.x, box_max_gl.y);

      glColor4fv(box_color.v);
      glVertex2f(box_max_gl.x, box_pos_gl.y);
      glColor4fv(box_color.v);
      glVertex2f(box_pos_gl.x, box_max_gl.y);
      glColor4fv(box_color.v);
      glVertex2f(box_max_gl.x, box_max_gl.y);
    }
    glEnd();
  }
}

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  AppState app_state = {0};
  app_state.box_colors[0] = v4(1, 0, 0, 1);
  app_state.box_colors[1] = v4(0, 1, 0, 1);
  app_state.background_color = v4(0, 0, 0, 1);  
  app_state.box_dim = v2(0.05, 0.05);
  app_state.box_v = v2(0.01, 0.01);
  app_state.mouse_pos = v2(-1, -1);

  if(wayland_init()) {
    WaylandWindow *window = wayland_open_window(Str8Lit("hello wayland"), 640, 480, RenderTarget_hardware);
    if(window) {
      app_state.render_target = window->render_target;
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

	R32 aspect_ratio = (R32)window->width/(R32)window->height;
	app_state.box_dim.y = aspect_ratio*app_state.box_dim.x;
	U32 *pixels = wayland_get_render_pixels(window);
	GlFramebuffer gl_framebuffer = wayland_get_framebuffer(window);
	update_and_render(pixels, gl_framebuffer, &app_state);

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
