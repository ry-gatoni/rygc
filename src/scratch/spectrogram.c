#include "base/base.h"
#include "os/os.h"

#include "wayland.h"

#include "base/base.c"
#include "os/os.c"

#include "wayland.c"

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
  "vec4 unpack_uint_color(uint color) {\n"
  "float r = float((color >> 24) & uint(0xFF)) / 255.0;\n"
  "float g = float((color >> 16) & uint(0xFF)) / 255.0;\n"
  "float b = float((color >>  8) & uint(0xFF)) / 255.0;\n"
  "float a = float( color        & uint(0xFF)) / 255.0;\n"
  "return vec4(b, b, b, 1);\n"
  "}\n"
  "void main() {\n"
  "gl_Position = vec4(v_p, 1);\n"
  "f_uv = v_uv;\n"
  /* "f_c = unpack_uint_color(v_c);\n" */
  "f_c = v_c;\n"
  /* "f_c = vec4(float((v_c & uint(0xFF000000)) >> 24), float((v_c & uint(0xFF000000)) >> 24), float((v_c & uint(0xFF000000)) >> 24), 255) / 255.0;\n" */
  "}\n";

global char frag_shader_src[] =
  "#version 330\n"
  "in vec2 f_uv;\n"
  "in vec4 f_c;\n"
  "uniform sampler2D atlas;\n"
  "out vec4 out_color;\n"
  "void main() {\n"
  "vec4 sampled = texture(atlas, f_uv);\n"
  /* "out_color = vec4(1, 0, 1, 1) * vec4(sampled.r, sampled.r, sampled.r, 1);\n" */
  "out_color = f_c;\n"
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
  Rect2 uv;
} R_Texture;

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

proc R_Commands*
render_begin_commands(Arena *arena)
{    
  R_Commands *result = arena_push_struct_z(arena, R_Commands);
  result->arena = arena;

  // NOTE: generate white texture
  result->white_texture = arena_push_struct(result->arena, R_Texture);  
  glGenTextures(1, &result->white_texture->handle);
  result->white_texture->uv = rect2(v2(0, 0), v2(1, 1));
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
render_rect(R_Commands *commands, R_Texture *texture, Rect2 rect, R32 level, V4 color)
{
  if(!texture)
    { texture = commands->white_texture; }

  // NOTE: check if texture is already used in a batch
  R_Batch *batch = 0;
  for(R_Batch *current_batch = commands->first_batch; current_batch; current_batch = current_batch->next) {

    if(texture->handle == current_batch->texture->handle) {

      batch = current_batch;
      break;
    }
  }

  if(!batch) {
    if(commands->batch_freelist) {
      batch = commands->batch_freelist;
      batch->vertex_count = 0;
      SLLStackPop(commands->batch_freelist);
    }else {
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

  U32 color_u32 = color_u32_from_v4(color);
  R_Vertex v00 = {0};
  v00.pos = v3(rect.min.x, rect.min.y, level);
  v00.uv = texture->uv.min;
  v00.color = color_u32;
  R_Vertex v01 = {0};
  v01.pos = v3(rect.min.x, rect.max.y, level);
  v01.uv = v2(texture->uv.min.x, texture->uv.max.y);
  v01.color = color_u32;
  R_Vertex v10 = {0};
  v10.pos = v3(rect.max.x, rect.min.y, level);
  v10.uv = v2(texture->uv.max.x, texture->uv.min.y);
  v10.color = color_u32;
  R_Vertex v11 = {0};
  v11.pos = v3(rect.max.x, rect.max.y, level);
  v11.uv = texture->uv.max;
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

  for(R_Batch *batch = commands->first_batch, *last_batch = 0;
      batch;
      last_batch = batch, batch = batch->next, SLLStackPush(commands->batch_freelist, last_batch), SLLQueuePop(commands->first_batch, commands->last_batch)) {
    
    glBufferData(GL_ARRAY_BUFFER, batch->vertex_count*sizeof(R_Vertex), batch->vertex_buffer, GL_STREAM_DRAW);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, batch->texture->handle);

    glDrawArrays(GL_TRIANGLES, 0, batch->vertex_count);
  }
  commands->batch_count = 0;
}

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  int result = 0;

  if(wayland_init()) {

    WaylandWindow *window = wayland_open_window(Str8Lit("spectrogram"), 640, 480, RenderTarget_hardware);
    if(window) {

      glEnable(GL_TEXTURE_2D);
      
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      glPixelStorei(GL_PACK_ALIGNMENT, 1);

      Arena *arena = arena_alloc();
      R_Commands *commands = render_begin_commands(arena);

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
	glViewport(0, 0, window_width, window_height);
	glClearColor(0, 0, 0, 1);
	glClearDepth(1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	render_rect(commands, 0, rect2_center_dim(v2(0, 0), v2(1, 1)), 0, v4(1, 0, 1, 1));

	render_from_commands(commands);

	if(!wayland_end_frame(window)) {
	  Assert(!"FATAL: wayland end frame failed");
	}
      }
      
      wayland_close_window(window);
    }
  }

  return(result);
}
