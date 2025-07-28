#include "base/base.h"
#include "os/os.h"

#include "wayland.h"

#include "font.h"

#include "freetype/freetype.h"
#include "font_freetype.h"

#include "base/base.c"
#include "os/os.c"

#include "wayland.c"

#include "font.c"
#include "font_freetype.c"

#if 0
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
#endif

proc void
render_texture(Rect2 rect, U32 texture_id)
{
  V2 p = rect.min;
  V2 dim = v2_sub(rect.max, rect.min);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glBegin(GL_TRIANGLES);
  {
    glTexCoord2f(0, 0);
    glVertex2f(p.x, p.y);
    glTexCoord2f(0, 1);
    glVertex2f(p.x, p.y + dim.y);
    glTexCoord2f(1, 1);
    glVertex2f(p.x + dim.x, p.y + dim.y);

    glTexCoord2f(0, 0);
    glVertex2f(p.x, p.y);
    glTexCoord2f(1, 1);
    glVertex2f(p.x + dim.x, p.y + dim.y);
    glTexCoord2f(1, 0);
    glVertex2f(p.x + dim.x, p.y);
  }
  glEnd();
}

proc V2
render_string(PackedFont *font, String8 string, V2 pos, V2 ndc_scale)
{
  for(U32 char_idx = 0; char_idx < string.count; ++char_idx) {

    U8 c = string.string[char_idx];
    U32 glyph_index = font_glyph_index_from_codepoint(font, c);
    PackedGlyph *glyph = font->glyphs + glyph_index;

    V2 uv_min = glyph->uv.min;
    V2 uv_max = glyph->uv.max;
    V2 rect_min = v2_add(pos, v2_hadamard(ndc_scale, glyph->rect.min));
    V2 rect_max = v2_add(pos, v2_hadamard(ndc_scale, glyph->rect.max));
    glBindTexture(GL_TEXTURE_2D, font->atlas_texture_id);
    glBegin(GL_TRIANGLES);
    {
      // NOTE: bottom left
      glTexCoord2f(uv_min.x, uv_min.y);
      glVertex2f(rect_min.x, rect_min.y);
      // NOTE: top left
      glTexCoord2f(uv_min.x, uv_max.y);
      glVertex2f(rect_min.x, rect_max.y);
      // NOTE: top right
      glTexCoord2f(uv_max.x, uv_max.y);
      glVertex2f(rect_max.x, rect_max.y);

      // NOTE: bottom left
      glTexCoord2f(uv_min.x, uv_min.y);
      glVertex2f(rect_min.x, rect_min.y);
      // NOTE: top right
      glTexCoord2f(uv_max.x, uv_max.y);
      glVertex2f(rect_max.x, rect_max.y);
      // NOTE: bottom right
      glTexCoord2f(uv_max.x, uv_min.y);
      glVertex2f(rect_max.x, rect_min.y);
    }
    glEnd();

    pos.x += ndc_scale.x * glyph->advance;
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
    
    if(wayland_init()) {

      String8 window_name = Str8Lit("hello freetype");
      WaylandWindow *window = wayland_open_window(window_name, 640, 480, RenderTarget_hardware);
      if(window) {

	glEnable(GL_TEXTURE_2D);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Arena *arena = arena_alloc();
	PackedFont *font = font_pack(arena, &loose_font);

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

	  /* Rect2 atlas_rect = make_rect2_center_dim(v2(0, 0), v2(2.f, 1.f)); */
	  /* render_texture(atlas_rect, font->atlas_texture_id); */

	  V2 start = v2(-0.5f, 0);
	  V2 pos = start;
	  pos = render_string(font, window_name, pos, ndc_scale);	  

	  start.y -= ndc_scale.y * font->line_height;

	  String8 second_string = Str8Lit("Second String");
	  pos = start;
	  pos = render_string(font, second_string, pos, ndc_scale);

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
