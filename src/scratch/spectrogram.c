/**
 * TODO:
 * test fft implementation is working properly
 * compute and render the spectrogram of something
 */

#include "base/base.h"
#include "os/os.h"

#include "wayland.h"

#include "font/font.h"
#include "fourier/fourier.h"

#include "base/base.c"
#include "os/os.c"
#include "font/font.c"
#include "fourier/fourier.c"

#include "wayland.c"

#include <math.h>

// TODO: should there be two of these, for each endianness?
proc U32
color_u32_from_v4(V4 color)
{
  U32 result = ((((U32)(color.r*255.f)) << 0*8) |
		(((U32)(color.g*255.f)) << 1*8) |
		(((U32)(color.b*255.f)) << 2*8) |
		(((U32)(color.a*255.f)) << 3*8));
  return(result);
}

global char vert_shader_src[] =
  "#version 330\n"
  "layout (location = 0) in vec3 v_p;\n"
  "layout (location = 1) in vec2 v_uv;\n"
  "layout (location = 2) in vec4 v_c;\n"
  "out vec2 f_uv;\n"
  "out vec4 f_c;\n"
  "void main() {\n"
  "gl_Position = vec4(v_p, 1);\n"
  "f_uv = v_uv;\n"
  "f_c = v_c;\n"  
  "}\n";

global char frag_shader_src[] =
  "#version 330\n"
  "in vec2 f_uv;\n"
  "in vec4 f_c;\n"
  "uniform sampler2D atlas;\n"
  "out vec4 out_color;\n"
  "void main() {\n"
  "vec4 sampled = texture(atlas, f_uv);\n"
  "out_color = f_c * vec4(sampled.r, sampled.r, sampled.r, 1);\n"
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

typedef struct R_Vertex
{
  V3 pos;
  V2 uv;
  U32 color;
} R_Vertex;

typedef struct R_Texture
{
  GLuint handle;
  V2S32 dim;
} R_Texture;

typedef struct R_Font
{
  PackedFont *font;
  R_Texture *atlas;
} R_Font;

typedef struct R_Batch R_Batch;
struct R_Batch
{
  R_Batch *next;

  U32 vertex_cap;
  U32 vertex_count;
  R_Vertex *vertex_buffer;
  
  R_Texture *texture;
};

typedef struct R_Commands
{
  Arena *arena;

  V2S32 window_dim;

  R_Batch *first_batch;
  R_Batch *last_batch;
  U32 batch_count;

  R_Texture *white_texture;

  R_Batch *batch_freelist;

  GLuint vao;
  GLuint vbo;
  GLuint sampler;
  GLint sampler_loc;
  
  GlShader vert_shader;
  GlShader frag_shader;
  GlShader shader_prog;
} R_Commands;

typedef enum RenderLevel
{
  RenderLevel_top,
  RenderLevel_background,
  RenderLevel_count,
} RenderLevel;

proc R_Font*
render_alloc_font(Arena *arena, PackedFont *font)
{
  R_Texture *atlas = arena_push_struct(arena, R_Texture);
  atlas->handle = font->atlas_texture_id;
  atlas->dim = v2s32(font->atlas_width, font->atlas_height);
      
  R_Font *result = arena_push_struct(arena, R_Font);      
  result->font = font;
  result->atlas = atlas;
  return(result);
}

proc R_Commands*
render_alloc_commands(Arena *arena)
{    
  R_Commands *result = arena_push_struct_z(arena, R_Commands);
  result->arena = arena;

  // NOTE: generate white texture
  result->white_texture = arena_push_struct(result->arena, R_Texture);  
  glGenTextures(1, &result->white_texture->handle);
  result->white_texture->dim = v2s32(16, 16);
  glBindTexture(GL_TEXTURE_2D, result->white_texture->handle);
  GlTextureDefaultParams();
  U8 white[256];
  SetSize(white, U8_MAX, ArrayCount(white));
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 16, 16, 0, GL_RED, GL_UNSIGNED_BYTE, white);

  // NOTE: compile and link shaders
  ArenaTemp scratch = arena_get_scratch(&arena, 1);
  result->vert_shader = gl_make_shader(scratch.arena, vert_shader_src, GL_VERTEX_SHADER);
  if(result->vert_shader.log.count) {
    fprintf(stderr, "ERROR: vert_shader: %.*s\n",
	    (int)result->vert_shader.log.count, result->vert_shader.log.string);
  }
  
  result->frag_shader = gl_make_shader(scratch.arena, frag_shader_src, GL_FRAGMENT_SHADER);
  if(result->frag_shader.log.count) {
    fprintf(stderr, "ERROR: frag_shader: %.*s\n",
	    (int)result->frag_shader.log.count, result->frag_shader.log.string);
  }

  GLuint shaders[] = {
    result->vert_shader.handle,
    result->frag_shader.handle,
  };
  result->shader_prog = gl_make_program(scratch.arena, shaders, ArrayCount(shaders));
  if(result->shader_prog.log.count) {
    fprintf(stderr, "ERROR: shader_program: %.*s\n",
	    (int)result->shader_prog.log.count, result->shader_prog.log.string);
  }

  glUseProgram(result->shader_prog.handle);

  // NOTE: generate arrays, samplers, uniforms
  glGenVertexArrays(1, &result->vao);
  glBindVertexArray(result->vao);

  glGenBuffers(1, &result->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, result->vbo);

  glGenSamplers(1, &result->sampler);
  result->sampler_loc = glGetUniformLocation(result->shader_prog.handle, "atlas");
  glUniform1i(result->sampler_loc, 0);

  arena_release_scratch(scratch);
  return(result);
}

proc void
render_begin_frame(R_Commands *commands, S32 window_width, S32 window_height)
{
  commands->window_dim = v2s32(window_width, window_height);
}

proc void
render_rect(R_Commands *commands, R_Texture *texture, Rect2 rect, Rect2 uv, RenderLevel render_level, V4 color)
{
  // NOTE: if a texture was not supplied, use the white texture
  if(!texture) {
    texture = commands->white_texture;
    uv = rect2(v2(0, 0), v2(1, 1));
  }

  // NOTE: check if texture is already used in a batch
  R_Batch *batch = 0;
  for(R_Batch *current_batch = commands->first_batch; current_batch; current_batch = current_batch->next) {

    if(texture->handle == current_batch->texture->handle) {

      batch = current_batch;
      break;
    }
  }

  // NOTE: if there is not already a batch with this texture, allocate a new one and add it to the list
  if(!batch) {
    if(commands->batch_freelist) {
      // NOTE: pull a batch off of the freelist
      batch = commands->batch_freelist;
      batch->vertex_count = 0;
      SLLStackPop(commands->batch_freelist);
    }else {
      // NOTE: push a new batch onto the arena
      batch = arena_push_struct_z(commands->arena, R_Batch);
      batch->vertex_cap = KB(32);
      batch->vertex_buffer = arena_push_array_z(commands->arena, R_Vertex, batch->vertex_cap);      
    }
    batch->texture = texture;
    SLLQueuePush(commands->first_batch, commands->last_batch, batch);
    ++commands->batch_count;
  }

  // TODO: allocate a new batch if we go over capacity
  Assert(batch->vertex_count + 6 <= batch->vertex_cap);

  R32 level = (R32)render_level/(R32)RenderLevel_count;
  U32 color_u32 = color_u32_from_v4(color);
  R_Vertex v00 = {0};
  v00.pos = v3(rect.min.x, rect.min.y, level);
  v00.uv = uv.min;
  v00.color = color_u32;
  R_Vertex v01 = {0};
  v01.pos = v3(rect.min.x, rect.max.y, level);
  v01.uv = v2(uv.min.x, uv.max.y);
  v01.color = color_u32;
  R_Vertex v10 = {0};
  v10.pos = v3(rect.max.x, rect.min.y, level);
  v10.uv = v2(uv.max.x, uv.min.y);
  v10.color = color_u32;
  R_Vertex v11 = {0};
  v11.pos = v3(rect.max.x, rect.max.y, level);
  v11.uv = uv.max;
  v11.color = color_u32;

  R_Vertex *verts = batch->vertex_buffer + batch->vertex_count;
  verts[0] = v00;
  verts[1] = v10;
  verts[2] = v11;
  verts[3] = v00;
  verts[4] = v11;
  verts[5] = v01;
  batch->vertex_count += 6;
}

proc void
render_string(R_Commands *commands, R_Font *font, String8 string, V2 pos, V4 color)
{
  V2 ndc_scale = v2(2.f/(R32)commands->window_dim.x,
		    2.f/(R32)commands->window_dim.y);

  for(U32 char_idx = 0; char_idx < string.count; ++char_idx) {

    U8 c = string.string[char_idx];
    PackedGlyph *glyph = font_glyph_from_codepoint(font->font, c);

    Rect2 rect = rect2(v2_add(pos, v2_hadamard(ndc_scale, glyph->rect.min)),
		       v2_add(pos, v2_hadamard(ndc_scale, glyph->rect.max)));
    render_rect(commands, font->atlas, rect, glyph->uv, 0, color);
    
    pos.x += ndc_scale.x * glyph->advance;
  }
}

proc void
render_from_commands(R_Commands *commands)
{
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, 0, sizeof(R_Vertex),
			PtrFromInt(OffsetOf(R_Vertex, pos)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, 0, sizeof(R_Vertex),
			PtrFromInt(OffsetOf(R_Vertex, uv)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, 1, sizeof(R_Vertex),
			PtrFromInt(OffsetOf(R_Vertex, color)));

  // NOTE: render all batches
  for(R_Batch *batch = commands->first_batch; batch; batch = batch->next) {
    
    glBufferData(GL_ARRAY_BUFFER, batch->vertex_count*sizeof(R_Vertex), batch->vertex_buffer, GL_STREAM_DRAW);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, batch->texture->handle);

    glDrawArrays(GL_TRIANGLES, 0, batch->vertex_count);
  }

  // NOTE: push all batches onto the freelist
  commands->last_batch->next = commands->batch_freelist;
  commands->batch_freelist = commands->first_batch;
  commands->first_batch = 0;
  commands->last_batch = 0;
  commands->batch_count = 0;
}

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  int result = 0;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  LooseFont loose_font = {0};  
  
  String8 font_file_path = Str8Lit("/usr/share/fonts/liberation-mono-fonts/LiberationMono-Regular.ttf");
  String8 font_file = os_read_entire_file(scratch.arena, font_file_path);
  if(font_file.count) {

    U32 pt_size = 32;
    loose_font = font_parse(scratch.arena, font_file, pt_size);
  }

  if(wayland_init()) {

    WaylandWindow *window = wayland_open_window(Str8Lit("spectrogram"), 640, 480, RenderTarget_hardware);
    if(window) {

      glEnable(GL_TEXTURE_2D);
      
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glDepthFunc(GL_LESS);

      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      glPixelStorei(GL_PACK_ALIGNMENT, 1);

      Arena *arena = arena_alloc();
      R_Commands *commands = render_alloc_commands(arena);
      
      PackedFont *packed_font = font_pack(arena, &loose_font);
      R_Font *font = render_alloc_font(arena, packed_font);

      ArenaTemp test_arena = arena_begin_temp(arena);
      FloatBuffer test_signal = {0};
      test_signal.count = 1024;
      test_signal.mem = arena_push_array_z(test_arena.arena, R32, test_signal.count);

      struct {
	R32 freq, amp;
      } fa[] = {
	{ 32.f, 8.f},
	{105.f, 5.f},
	{330.f, 3.f},
      };      
      for(U32 i = 0; i < test_signal.count; ++i) {

	R32 sample = 0;
	for(U32 j = 0; j < ArrayCount(fa); ++j) {

	  R32 freq = fa[j].freq;
	  R32 amp = fa[j].amp;
	  sample += amp*sin(2.f*PI32*freq*(R32)i/(R32)test_signal.count);
	}
	test_signal.mem[i] = sample;
      }

      ComplexBuffer test_fft_result = fft_re(test_arena.arena, test_signal);
      FloatBuffer test_result = ifft_re(test_arena.arena, test_fft_result);
      Unused(test_result);

      arena_end_temp(test_arena);

      B32 running = 1;
      while(running) {
	
	Event event = {0};
	EventList events = wayland_get_events(window);
	while(next_event(&events, &event)) {	  
	  switch(event.kind) {	    
	  case EventKind_Close:
	    { running = 0; } break;
	  default:
	    {} break;
	  }
	}

	GlFramebuffer framebuffer = wayland_get_framebuffer(window);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);

	S32 window_width  = window->width;
	S32 window_height = window->height;
	V2 ndc_scale = v2(2.f/(R32)window_width, 2.f/(R32)window_height);	
	glViewport(0, 0, window_width, window_height);
	glClearColor(0, 0, 0, 1);
	glClearDepth(1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	render_begin_frame(commands, window_width, window_height);

	// NOTE: draw grid
	U32 major_freqs[] = {2, 20, 200, 2000, 20000};
	U32 freq_line_count = ArrayCount(major_freqs);
	R32 freq_line_major_advance = 2.f/(R32)(freq_line_count - 1);
	
	S32 major_amps[] = {-60, -54, -48, -42, -36, -30, -24, -18, -12, -6, 0};
	U32 amp_line_count = ArrayCount(major_amps);
	R32 amp_line_major_advance = 2.f/(R32)amp_line_count;
	
	V4 background_color = v4(0.09411f, 0.10196f, 0.14902f, 1);
	/* render_rect(commands, font_atlas, */
	/* 	    rect2_center_dim(v2(0, 0), v2(2, 1)), */
	/* 	    rect2(v2(0, 0), v2(1, 1)), */
	/* 	    0, v4(1, 1, 1, 1)); */
	Rect2 screen_rect = rect2_center_dim(v2(0, 0), v2(2, 2));
	render_rect(commands, 0, screen_rect, rect2_invalid(), RenderLevel_background, background_color);

	V2 freq_label_pos = v2(-0.99, -0.95f);
	char *freq_labels[] = {"2", "20", "200", "2000", "20000"};
	StaticAssert(ArrayCount(freq_labels) == ArrayCount(major_freqs), freq_check);
	for(U32 freq_idx = 0; freq_idx < ArrayCount(freq_labels); ++freq_idx) {

	  render_string(commands, font, Str8Cstr(freq_labels[freq_idx]), freq_label_pos, v4(1, 1, 1, 1));
	  freq_label_pos.x += freq_line_major_advance;
	}

	V2 amp_label_pos = v2(-0.95, -0.99f);
	char *amp_labels[] = {"-60", "-54", "-48", "-42", "-36", "-30", "-24", "-18", "-12", "-6", "0"};
	StaticAssert(ArrayCount(amp_labels) == ArrayCount(major_amps), amp_check);
	for(U32 amp_idx = 0; amp_idx < ArrayCount(amp_labels); ++amp_idx) {

	  render_string(commands, font, Str8Cstr(amp_labels[amp_idx]), amp_label_pos, v4(1, 1, 1, 1));
	  amp_label_pos.y += amp_line_major_advance;
	}

	U32 freq_line_thickness_px = 5;
	U32 freq_line_minor_thickness_px = 2;
	R32 freq_line_thickness = freq_line_thickness_px * ndc_scale.x;
	R32 freq_line_minor_thickness = freq_line_minor_thickness_px * ndc_scale.x;
	V2 freq_line_pos = v2(-1, 0);
	V2 freq_line_dim = v2(freq_line_thickness, 2);
	V2 freq_line_minor_dim = v2(freq_line_minor_thickness, 2);
	V4 freq_line_color = v4(1, 1, 1, 1);
	for(U32 freq_decade_idx = 0; freq_decade_idx < freq_line_count - 1; ++freq_decade_idx) {

	  Rect2 freq_line_rect = rect2_center_dim(freq_line_pos, freq_line_dim);
	  render_rect(commands, 0, freq_line_rect, rect2_invalid(), RenderLevel_top, freq_line_color);

	  R32 sep = ((R32)major_freqs[freq_decade_idx + 1] - (R32)major_freqs[freq_decade_idx])/10.f;
	  for(U32 line_idx = 1; line_idx < 10; ++line_idx) {

	    R32 offset = (log10f(((R32)major_freqs[freq_decade_idx] + sep*line_idx)/(R32)major_freqs[freq_decade_idx]))*freq_line_major_advance;
	    V2 pos = v2(freq_line_pos.x + offset, freq_line_pos.y);
	    freq_line_rect = rect2_center_dim(pos, freq_line_minor_dim);
	    render_rect(commands, 0, freq_line_rect, rect2_invalid(), RenderLevel_top, freq_line_color);
	  }

	  freq_line_pos.x += freq_line_major_advance;
	}

	U32 amp_line_thickness_px = 5;
	R32 amp_line_thickness = amp_line_thickness_px * ndc_scale.y;
	V2 amp_line_pos = v2(0, -1);
	V2 amp_line_dim = v2(2, amp_line_thickness);
	V4 amp_line_color = v4(1, 1, 1, 1);
	for(U32 amp_line_idx = 0; amp_line_idx < amp_line_count; ++amp_line_idx) {

	  Rect2 amp_line_rect = rect2_center_dim(amp_line_pos, amp_line_dim);
	  render_rect(commands, 0, amp_line_rect, rect2_invalid(), 0, amp_line_color);
	  amp_line_pos.y += amp_line_major_advance;
	}

	render_from_commands(commands);

	if(!wayland_end_frame(window)) {
	  Assert(!"FATAL: wayland end frame failed");
	}
      }
      
      wayland_close_window(window);
    }
  }

  arena_release_scratch(scratch);
  return(result);
}
