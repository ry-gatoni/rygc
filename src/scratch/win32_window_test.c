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

    Win32GlLogError();

    if(render_init()) {
      /* glEnable(GL_TEXTURE_2D); */
      
      /* glEnable(GL_BLEND); */
      /* glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); */

      /* glDepthFunc(GL_LESS); */

      /* glPixelStorei(GL_UNPACK_ALIGNMENT, 1); */
      /* glPixelStorei(GL_PACK_ALIGNMENT, 1); */

      Win32GlLogError();

      Arena *arena = arena_alloc();
      PackedFont *packed_font = font_pack(arena, &loose_font);
      U32 font_atlas_texture_handle = (U32)IntFromPtr(packed_font->atlas.handle.handle);
      //glBindTexture(GL_TEXTURE_2D, font_atlas_texture_handle);
      //glDisable(GL_TEXTURE_2D);

      Win32GlLogError();

      // TODO: I think the window title unicode coversion isn't working correctly
      Os_Handle window = os_open_window(Str8Lit("w32-window-test"), 640, 480);
      if(window.handle) {

	render_equip_window(window);

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

	  render_begin_frame();
	  {
	    V2S32 window_dim = os_window_get_dim(window);
	    glViewport(0, 0, window_dim.width, window_dim.height);
	    glClearColor(0.f, 0.f, 0.f, 1.f);
	    glClearDepth(1);
	    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	    //Win32GlLogError();
	  
	    render_rect(&packed_font->atlas,
			rect2_center_dim(v2(0, 0), v2_lmul(0.5f, v2_from_v2s32(window_dim))),
			rect2(v2(0, 0), v2(1, 1)),
			0, v4(1, 1, 1, 1));

	    //Win32GlLogError();
	  }
	  render_end_frame();
	
	  arena_clear(frame_arena);
	}
      
	os_close_window(window);
      }
    }
  }
  
  arena_release_scratch(scratch);
  return(result);
}
