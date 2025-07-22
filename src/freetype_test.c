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

typedef struct GlTexture
{
  U32 texture;
  LoadedBitmap *bitmap;
} GlTexture;

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
    printf("packed font\n");

    if(wayland_init()) {

      String8 window_name = Str8Lit("hello freetype");
      WaylandWindow *window = wayland_open_window(window_name, 640, 480, RenderTarget_hardware);
      if(window) {

	glEnable(GL_TEXTURE_2D);
	U32 gl_textures_count = window_name.count;
	GlTexture *gl_textures = arena_push_array(scratch.arena, GlTexture, gl_textures_count);
	for(U32 texture_idx = 0; texture_idx < gl_textures_count; ++texture_idx) {
	  
	  GlTexture *gl_texture = gl_textures + texture_idx;	  
	  glGenTextures(1, &gl_texture->texture);

	  U8 *c = window_name.string + texture_idx;
	  LoadedBitmap *glyph = font_get_glyph_from_codepoint(&font, *c);
	  gl_texture->bitmap = glyph;

	  glBindTexture(GL_TEXTURE_2D, gl_texture->texture);
	  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		       gl_texture->bitmap->width, gl_texture->bitmap->height,
		       0, GL_RGBA, GL_UNSIGNED_BYTE, gl_texture->bitmap->pixels);
	  GlTextureDefaultParams();
	  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}

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

	  V2 bottom_left = v2(-0.5f, 0);
	  for(U32 texture_idx = 0; texture_idx < gl_textures_count; ++texture_idx) {
	    
	    GlTexture *gl_texture = gl_textures + texture_idx;
	    R32 glyph_width_gl = 0.5f*(gl_texture->bitmap->width/(R32)window_width);
	    R32 glyph_height_gl = 0.5f*(gl_texture->bitmap->height/(R32)window_height);

	    // TODO: glyph colors all weird
	    glBindTexture(GL_TEXTURE_2D, gl_texture->texture);
	    glBegin(GL_TRIANGLES);
	    {
	      glTexCoord2f(0, 0);
	      glVertex2f(bottom_left.x, bottom_left.y);
	      glTexCoord2f(1, 0);
	      glVertex2f(bottom_left.x + glyph_width_gl, bottom_left.y);
	      glTexCoord2f(1, 1);
	      glVertex2f(bottom_left.x + glyph_width_gl, bottom_left.y + glyph_height_gl);

	      glTexCoord2f(0, 0);
	      glVertex2f(bottom_left.x, bottom_left.y);
	      glTexCoord2f(1, 1);
	      glVertex2f(bottom_left.x + glyph_width_gl, bottom_left.y + glyph_height_gl);
	      glTexCoord2f(0, 1);
	      glVertex2f(bottom_left.x, bottom_left.y + glyph_height_gl);
	    }
	    glEnd();

	    bottom_left.x += glyph_width_gl + 0.02f;
	  }

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
