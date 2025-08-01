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

typedef struct GlVertex
{
  V2 pos;
  V2 uv;
} GlVertex;

proc void
render_rect(PushBuffer *vertex_buffer, Rect2 rect)
{
  V2 p = rect.min;
  V2 dim = v2_sub(rect.max, rect.min);

#if 1
  GlVertex v00 = {0};
  v00.pos = p;
  v00.uv = v2(0, 0);
  GlVertex v01 = {0};
  v01.pos = v2(p.x, p.y + dim.y);
  v01.uv = v2(0, 1);
  GlVertex v10 = {0};
  v10.pos = v2(p.x + dim.x, p.y);
  v10.uv = v2(1, 0);
  GlVertex v11 = {0};
  v11.pos = v2_add(p, dim);
  v11.uv = v2(1, 1);

  GlVertex *verts = buf_push_array(vertex_buffer, GlVertex, 6);
  verts[0] = v00;
  verts[1] = v10;
  verts[2] = v11;
  verts[3] = v00;
  verts[4] = v11;
  verts[5] = v01;
#else
  //glBindTexture(GL_TEXTURE_2D, texture_id);
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
#endif
}

proc V2
render_string(PushBuffer *vertex_buffer, PackedFont *font, String8 string, V2 pos, V2 ndc_scale)
{
  for(U32 char_idx = 0; char_idx < string.count; ++char_idx) {

    U8 c = string.string[char_idx];
    U32 glyph_index = font_glyph_index_from_codepoint(font, c);
    PackedGlyph *glyph = font->glyphs + glyph_index;

    V2 uv_min = glyph->uv.min;
    V2 uv_max = glyph->uv.max;
    V2 rect_min = v2_add(pos, v2_hadamard(ndc_scale, glyph->rect.min));
    V2 rect_max = v2_add(pos, v2_hadamard(ndc_scale, glyph->rect.max));
    
#if 1
    GlVertex v00 = {0};
    v00.pos = rect_min;
    v00.uv = uv_min;
    GlVertex v01 = {0};
    v01.pos = v2(rect_min.x, rect_max.y);
    v01.uv = v2(uv_min.x, uv_max.y);
    GlVertex v10 = {0};
    v10.pos = v2(rect_max.x, rect_min.y);
    v10.uv = v2(uv_max.x, uv_min.y);
    GlVertex v11 = {0};
    v11.pos = rect_max;
    v11.uv = uv_max;

    GlVertex *verts = buf_push_array(vertex_buffer, GlVertex, 6);
    verts[0] = v00;
    verts[1] = v10;
    verts[2] = v11;
    verts[3] = v00;
    verts[4] = v11;
    verts[5] = v01;    
#else
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
#endif

    pos.x += ndc_scale.x * glyph->advance;
  }

  return(pos);
}

global char vert_shader_src[] =
  "#version 330\n"
  "layout (location = 0) in vec2 v_p;\n"
  "layout (location = 1) in vec2 v_uv;\n"  
  "out vec4 f_c;\n"
  "out vec2 f_uv;\n"
  "void main() {\n"
  "gl_Position = vec4(v_p, 0, 1);\n"
  "f_c = vec4(1, 1, 1, 1);\n"
  "f_uv = v_uv;\n"
  "}\n";

global char frag_shader_src[] =
  "#version 330\n"
  "in vec4 f_c;\n"
  "in vec2 f_uv;\n"
  "uniform sampler2D atlas;\n"
  "out vec4 out_color;\n"
  "void main() {\n"
  "vec4 sampled = texture(atlas, f_uv);\n"
  "out_color = vec4(sampled.r, sampled.r, sampled.r, 1);\n"
  /* "out_color = f_c;\n" */
  "}\n";

typedef struct GlShader
{
  GLuint handle;
  String8 log;
} GlShader;

proc GlShader
gl_make_shader(Arena *arena, char *src, GLenum kind)
{  
  GLuint shader_id = glCreateShader(kind);
  glShaderSource(shader_id, 1, (const GLchar*const*)&src, 0);
  glCompileShader(shader_id);

  String8 log = {0};
  GLint info_log_length;
  glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
  if(info_log_length) {
    
    char *buffer = arena_push_array(arena, char, info_log_length + 1);
    GLint len = 0;
    glGetShaderInfoLog(shader_id, info_log_length + 1, &len, buffer);
    log.count = len;
    log.string = (U8*)buffer;
  }

  GLint compile_status;
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_status);
  if(compile_status == 0) {
    glDeleteShader(shader_id);
    shader_id = 0;
  }

  GlShader result = {0};
  result.handle = shader_id;
  result.log = log;
  return(result);
}

proc GlShader
gl_make_program(Arena *arena, GLuint *shaders, U32 shader_count)
{
  GLuint program_id = glCreateProgram();
  for(U32 shader_idx = 0; shader_idx < shader_count; ++shader_idx)
    { glAttachShader(program_id, shaders[shader_idx]); }
  glLinkProgram(program_id);

  String8 log = {0};
  GLint info_log_length;
  glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
  if(info_log_length) {
    
    char *buffer = arena_push_array(arena, char, info_log_length + 1);
    GLint len = 0;
    glGetProgramInfoLog(program_id, info_log_length + 1, &len, buffer);
    log.count = len;
    log.string = (U8*)buffer;
  }

  GLint link_status;
  glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);
  if(link_status == 0) {
    glDeleteProgram(program_id);
    program_id = 0;
  }  

  GlShader result = {0};
  result.handle = program_id;
  result.log = log;
  return(result);
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

	// NOTE: OpenGL setup
	glEnable(GL_TEXTURE_2D);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	GlShader vert_shader = gl_make_shader(scratch.arena, vert_shader_src, GL_VERTEX_SHADER);
	if(vert_shader.log.count)
	  { fprintf(stderr, "ERROR: vert_shader: %.*s\n", (int)vert_shader.log.count, vert_shader.log.string); }

	GlShader frag_shader = gl_make_shader(scratch.arena, frag_shader_src, GL_FRAGMENT_SHADER);
	if(frag_shader.log.count)
	  { fprintf(stderr, "ERROR: frag_shader: %.*s\n", (int)frag_shader.log.count, frag_shader.log.string); }

	GLuint shaders[] = {
	  vert_shader.handle,
	  frag_shader.handle,
	};
	GlShader shader_prog = gl_make_program(scratch.arena, shaders, ArrayCount(shaders));
	if(shader_prog.log.count)
	  { fprintf(stderr, "ERROR: shader_program: %.*s\n", (int)shader_prog.log.count, shader_prog.log.string); }

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	GLuint sampler = 0;
	glGenSamplers(1, &sampler);
	//glBindSampler(0, sampler);

	glUseProgram(shader_prog.handle);	
	GLint sampler_loc = glGetUniformLocation(shader_prog.handle, "atlas");
	glUniform1i(sampler_loc, 0);

	// NOTE: pack font
	Arena *arena = arena_alloc();
	PackedFont *font = font_pack(arena, &loose_font);

	// NOTE: main loop
	Arena *frame_arena = arena_alloc();
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

	  PushBuffer vertex_buffer = push_buffer_alloc(frame_arena, KB(32));

	  /* Rect2 atlas_rect = make_rect2_center_dim(v2(0, 0), v2(2.f, 1.f)); */
	  /* render_rect(&vertex_buffer, atlas_rect); */

	  V2 start = v2(-0.5f, 0);
	  V2 pos = start;
	  pos = render_string(&vertex_buffer, font, window_name, pos, ndc_scale);	  

	  start.y -= ndc_scale.y * font->line_height;

	  String8 second_string = Str8Lit("Second String");
	  pos = start;
	  pos = render_string(&vertex_buffer, font, second_string, pos, ndc_scale);

	  // NOTE: render
	  glBufferData(GL_ARRAY_BUFFER, vertex_buffer.pos, vertex_buffer.mem, GL_STREAM_DRAW);

	  glActiveTexture(GL_TEXTURE0);
	  glBindTexture(GL_TEXTURE_2D, font->atlas_texture_id);

	  glUseProgram(shader_prog.handle);
	  glUniform1i(sampler_loc, 0);

	  glEnableVertexAttribArray(0);
	  glVertexAttribPointer(0, 2, GL_FLOAT, 0, sizeof(GlVertex),
				PtrFromInt(OffsetOf(GlVertex, pos)));
	  
	  glEnableVertexAttribArray(1);
	  glVertexAttribPointer(1, 2, GL_FLOAT, 0, sizeof(GlVertex),
				PtrFromInt(OffsetOf(GlVertex, uv)));

	  glDrawArrays(GL_TRIANGLES, 0, vertex_buffer.pos/sizeof(GlVertex));

	  arena_clear(frame_arena);
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
