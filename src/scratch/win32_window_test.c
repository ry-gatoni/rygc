#define OS_FEATURE_GFX
#include "base/base.h"
#include "os/os.h"
#include "render/render.h"
#include "font/font.h"
#include "OpenGL/ogl.h"

#include "base/base.c"
#include "os/os.c"
#include "render/render.c"
#include "font/font.c"
#include "OpenGL/ogl.c"

int
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
  int result = 0;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  LooseFont loose_font =
    font_parse(scratch.arena, Str8Lit("C:\\Windows\\Fonts\\LiberationMono-Regular.ttf"), 32);

  if(os_gfx_init()) {

    // TODO: I think the window title unicode coversion isn't working correctly
    Os_Handle window = os_open_window(Str8Lit("w32-window-test"), 640, 480);
    if(window.handle) {
      
      B32 running = 1;
      Arena *frame_arena = arena_alloc();
      while(running) {

	Os_EventList events = os_events(frame_arena);
	for(Os_Event *event = events.first; event; event = event->next) {
	  if(event->window.handle == window.handle) {
	    switch(event->kind)
	      {
	      case Os_EventKind_close:
		{
		  running = 0;
		}break;
	      }
	  }
	}

	os_window_begin_frame(window);
	{
	  V2S32 window_dim = os_window_get_dim(window);
	  glViewport(0, 0, window_dim.width, window_dim.height);
	  glClearColor(1.f, 0.f, 1.f, 1.f);
	  glClear(GL_COLOR_BUFFER_BIT);
	}
	os_window_end_frame(window);
	
	arena_clear(frame_arena);
      }
      
      os_close_window(window);
    }  
  }
  
  arena_release_scratch(scratch);
  return(result);
}
