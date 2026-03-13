#include "base/base.h"
#include "gfx/gfx.h"
/* #include "render/render.h" */
/* #include "font/font.h" */
/* #include "OpenGL/ogl.h" */

// TODO: sort out dependencies. It should be possible to open a window without
// providing a renderer or graphics api. It should be possible to use a renderer
// without providing a font loader.

#include "base/base.c"
#include "gfx/gfx.c"
/* #include "render/render.c" */
/* #include "font/font.c" */
/* #include "OpenGL/ogl.c" */

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  if(!gfx_init()) return 1;
  //if(!render_init()) return 1;

  Os_Handle window = gfx_window_open(Str8Lit("3d filter"), 640, 480);
  //render_equip_window(window);

  Arena *frame_arena = arena_alloc();
  B32 running = 1;
  while(running)
  {
    Os_EventList events = gfx_events(frame_arena);
    for(Os_Event *event = events.first; event; event = event->next)
    {
      if(event->kind == Os_EventKind_close)
      {
	running = 0;
	break;
      }
    }

    //render_begin_frame();
    gfx_window_begin_frame(window);

    //render_end_frame();
    gfx_window_end_frame(window);
    arena_clear(frame_arena);
  }

  gfx_window_close(window);
  gfx_uninit();

  return(0);
}
