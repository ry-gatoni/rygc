#include "base/base.h"
#include "os/os.h"

#include "font.h"
#include "wayland.h"

#include "freetype/freetype.h"
#include "font_freetype.h"

#include "base/base.c"
#include "os/os.c"

#include "font.c"
#include "font_freetype.c"
#include "wayland.c"

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  ArenaTemp scratch = arena_get_scratch(0, 0);
  String8 font_file_path = Str8Lit("/usr/share/fonts/liberation-mono-fonts/LiberationMono-Regular.ttf");
  String8 font_file = os_read_entire_file(scratch.arena, font_file_path);  
  if(font_file.count) {

    LooseFont loose_font = font_parse(scratch.arena, font_file);

    Arena *arena = arena_alloc();
    LoadedFont font = font_pack(arena, &loose_font);
    printf("packed font\n");

    if(wayland_init()) {
      
      WaylandWindow *window = wayland_open_window(Str8Lit("hello freetype"), 640, 480, RenderTarget_hardware);
      if(window) {

	B32 running = 1;
	while(running) {
	  
	  EventList events = wayland_get_events(window);
	  Event event = {0};
	  while(next_event(&events, &event)) {
	    
	    switch(event.kind) {
	    case EventKind_Close: {
	      running = 0;
	    } break;
	    default: {} break;
	    }
	  }	  
	  
	  GlFramebuffer gl_framebuffer = wayland_get_framebuffer(window);
	  glBindFramebuffer(GL_FRAMEBUFFER, gl_framebuffer.fbo);

	  S32 window_width = window->width;
	  S32 window_height = window->height;
	  glViewport(0, 0, window_width, window_height);
	  
	  glClearColor(0, 0, 0, 1);
	  glClearDepth(1);
	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	  // TODO: render a string!

	  if(!wayland_end_frame(window)) {
	    Assert(!"FATAL: wayland end frame failed");
	  }
	}

	wayland_close_window(window);
      }
    }
  }

  arena_release_scratch(scratch);
  return(0);
}
