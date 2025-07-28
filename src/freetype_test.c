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

proc void
gl_texture_from_bitmap(LoadedBitmap *bitmap)
{
  Assert(bitmap);
  
  U32 texture = 0;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
	       bitmap->width, bitmap->height, 0,
	       GL_RGBA, GL_UNSIGNED_BYTE, bitmap->pixels);
  GlTextureDefaultParams();
}

proc V2
render_string(LoadedFont *font, String8 string, V2 pos, V2 ndc_scale)
{
  for(U32 char_idx = 0; char_idx < string.count; ++char_idx) {

    U8 c = string.string[char_idx];
    LoadedGlyph *glyph = font_get_glyph_from_codepoint(font, c);
    gl_texture_from_bitmap(&glyph->bitmap);

    R32 glyph_width_gl  = glyph->bitmap.width  * ndc_scale.x;
    R32 glyph_height_gl = glyph->bitmap.height * ndc_scale.y; 

    V2 glyph_align_gl = v2(glyph->bitmap.align_pos.x * ndc_scale.x,
			   glyph->bitmap.align_pos.y * ndc_scale.y);
	    
    R32 glyph_advance_gl = glyph->advance * ndc_scale.x;

    V2 bottom_left = v2(pos.x + glyph_align_gl.x,
			pos.y + glyph_align_gl.y - glyph_height_gl);
    glBegin(GL_TRIANGLES);
    {
      glTexCoord2f(0, 0);
      glVertex2f(bottom_left.x, bottom_left.y);
      glTexCoord2f(0, 1);
      glVertex2f(bottom_left.x, bottom_left.y + glyph_height_gl);
      glTexCoord2f(1, 1);
      glVertex2f(bottom_left.x + glyph_width_gl, bottom_left.y + glyph_height_gl);

      glTexCoord2f(0, 0);
      glVertex2f(bottom_left.x, bottom_left.y);
      glTexCoord2f(1, 1);
      glVertex2f(bottom_left.x + glyph_width_gl, bottom_left.y + glyph_height_gl);
      glTexCoord2f(1, 0);
      glVertex2f(bottom_left.x + glyph_width_gl, bottom_left.y);
    }
    glEnd();

    pos.x += glyph_advance_gl;
  }

  return(pos);
}

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  ArenaTemp scratch = arena_get_scratch(0, 0);
  String8 font_file_path = Str8Lit("/usr/share/fonts/liberation-mono-fonts/LiberationMono-Regular.ttf");
  String8 font_file = os_read_entire_file(scratch.arena, font_file_path);  
  if(font_file.count) {

    U32 pt_size = 128;
    LooseFont loose_font = font_parse(scratch.arena, font_file, pt_size);

    Arena *arena = arena_alloc();
    LoadedFont font = font_pack(arena, &loose_font);
    
    if(wayland_init()) {

      String8 window_name = Str8Lit("hello freetype");
      WaylandWindow *window = wayland_open_window(window_name, 640, 480, RenderTarget_hardware);
      if(window) {

	glEnable(GL_TEXTURE_2D);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
	  V2 ndc_scale = v2(2.f/(R32)window_width, 2.f/(R32)window_height);
	  glViewport(0, 0, window_width, window_height);
	  
	  glClearColor(0, 0, 0, 1);
	  glClearDepth(1);
	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	  V2 start = v2(-0.5f, 0);
	  V2 pos = start;
	  pos = render_string(&font, window_name, pos, ndc_scale);	  

	  start.y -= ndc_scale.y * font.line_height;

	  String8 second_string = Str8Lit("Second String");
	  pos = start;
	  pos = render_string(&font, second_string, pos, ndc_scale);

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
