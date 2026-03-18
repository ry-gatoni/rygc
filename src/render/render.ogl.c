#include "OpenGL/ogl.c"

proc GLuint
ogl__handle_from_render_handle(R_Handle handle)
{
  GLuint result = IntFromPtr(handle.handle);
  return(result);
}

proc R_Handle
render__handle_from_ogl_handle(GLuint handle)
{
  R_Handle result = {0};
  result.handle = PtrFromInt(handle);
  return(result);
}

/* proc Ogl_Shader */
/* ogl__make_shader(Arena *arena, char *src, GLenum kind) */
/* { */
/*   GLuint shader_id = glCreateShader(kind); */
/*   glShaderSource(shader_id, 1, (const GLchar*const*)&src, 0); */
/*   glCompileShader(shader_id); */

/*   String8 log = {0}; */
/*   GLint info_log_length; */
/*   glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length); */
/*   if(info_log_length) */
/*   { */
/*     char *buffer = arena_push_array(arena, char, info_log_length + 1); */
/*     GLint len = 0; */
/*     glGetShaderInfoLog(shader_id, info_log_length + 1, &len, buffer); */
/*     log.count = len; */
/*     log.string = (U8*)buffer; */
/*   } */

/*   GLint compile_status; */
/*   glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_status); */
/*   if(compile_status == 0) */
/*   { */
/*     glDeleteShader(shader_id); */
/*     shader_id = 0; */
/*   } */

/*   Ogl_Shader result = {0}; */
/*   result.handle = shader_id; */
/*   result.log = log; */
/*   return(result); */
/* } */

/* proc Ogl_Shader */
/* ogl__make_program(Arena *arena, GLuint *shaders, U32 shader_count) */
/* { */
/*   GLuint program_id = glCreateProgram(); */
/*   for(U32 shader_idx = 0; shader_idx < shader_count; ++shader_idx) */
/*   { glAttachShader(program_id, shaders[shader_idx]); } */
/*   glLinkProgram(program_id); */

/*   String8 log = {0}; */
/*   GLint info_log_length; */
/*   glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length); */
/*   if(info_log_length) */
/*   { */
/*     char *buffer = arena_push_array(arena, char, info_log_length + 1); */
/*     GLint len = 0; */
/*     glGetProgramInfoLog(program_id, info_log_length + 1, &len, buffer); */
/*     log.count = len; */
/*     log.string = (U8*)buffer; */
/*   } */

/*   GLint link_status; */
/*   glGetProgramiv(program_id, GL_LINK_STATUS, &link_status); */
/*   if(link_status == 0) */
/*   { */
/*     glDeleteProgram(program_id); */
/*     program_id = 0; */
/*   } */

/*   Ogl_Shader result = {0}; */
/*   result.handle = program_id; */
/*   result.log = log; */
/*   return(result); */
/* } */

/* proc R_Texture */
/* render_ogl_create_texture(V2S32 dim, void *pixels, R_TextureCreateParams *params) */
/* { */
/*   U32 handle; */
/*   glGenTextures(1, &handle); */
/*   glBindTexture(GL_TEXTURE_2D, handle); */

/*   GLint internal_fmt = ogl_fmts[params->internal_fmt]; */
/*   GLint pixel_fmt = ogl_fmts[params->pixel_fmt]; */
/*   S32 width = dim.width; */
/*   S32 height = dim.height; */
/*   glTexImage2D(GL_TEXTURE_2D, 0, internal_fmt, width, height, 0, pixel_fmt, GL_UNSIGNED_BYTE, pixels); */
/*   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); */
/*   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); */
/*   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, params->wrap_x ? GL_REPEAT : GL_CLAMP_TO_EDGE); */
/*   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, params->wrap_y ? GL_REPEAT : GL_CLAMP_TO_EDGE); */

/*   R_Texture result = {0}; */
/*   result.handle = ogl__render_handle_from_handle(handle);//.handle = PtrFromInt(handle); */
/*   result.dim = dim; */
/*   result.pixels = pixels; */
/*   return(result); */
/* } */

/* proc void */
/* render_ogl_update_texture(R_Texture *texture, V2S32 pos, V2S32 dim, R_PixelFormat pixel_format, void *pixels) */
/* { */
/*   //U32 handle = (U32)IntFromPtr(texture->handle.handle); */
/*   GLuint handle = ogl__handle_from_render_handle(texture->handle); */
/*   glBindTexture(GL_TEXTURE_2D, handle); */

/*   S32 x = pos.x; */
/*   S32 y = pos.y; */
/*   S32 w = dim.width; */
/*   S32 h = dim.height; */
/*   glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, ogl_fmts[pixel_format], GL_UNSIGNED_BYTE, pixels); */
/* } */

proc R_Handle
render_ogl_backend_init(Arena *arena)
{
  R_Handle result = {0};
  if(!ogl_init()) return(result);

  ogl_renderer = arena_push_struct(arena, Ogl_Renderer);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDepthFunc(GL_LESS);

  ArenaTemp scratch = arena_get_scratch(&arena, 1);
  ogl_renderer->vert_shader = ogl_make_shader(scratch.arena, vert_shader_src, GL_VERTEX_SHADER);
  if(ogl_renderer->vert_shader.log.count)
  {
    fprintf(stderr, "ERROR: vert_shader: %.*s\n",
            (int)ogl_renderer->vert_shader.log.count, ogl_renderer->vert_shader.log.string);
  }

  ogl_renderer->frag_shader = ogl_make_shader(scratch.arena, frag_shader_src, GL_FRAGMENT_SHADER);
  if(ogl_renderer->frag_shader.log.count)
  {
    fprintf(stderr, "ERROR: frag_shader: %.*s\n",
            (int)ogl_renderer->frag_shader.log.count, ogl_renderer->frag_shader.log.string);
  }

  GLuint shaders[] = {
    ogl_renderer->vert_shader.handle,
    ogl_renderer->frag_shader.handle,
  };
  ogl_renderer->shader_prog = ogl_make_program(scratch.arena, shaders, ArrayCount(shaders));
  if(ogl_renderer->shader_prog.log.count)
  {
    fprintf(stderr, "ERROR: shader_program: %.*s\n",
            (int)ogl_renderer->shader_prog.log.count, ogl_renderer->shader_prog.log.string);
  }

  glUseProgram(ogl_renderer->shader_prog.handle);

  // NOTE: generate arrays, samplers, uniforms
  glGenVertexArrays(1, &ogl_renderer->vao);
  glBindVertexArray(ogl_renderer->vao);

  glGenBuffers(1, &ogl_renderer->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, ogl_renderer->vbo);

  // NOTE: allocate vbo data buffer; set quad pattern data
  U64 pattern_size = sizeof(quad_pattern);
  U64 data_size = pattern_size + RENDER_BATCH_QUAD_CAP*sizeof(R_Quad);
  glBufferData(GL_ARRAY_BUFFER, data_size, 0, GL_STREAM_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, pattern_size, quad_pattern);

  glGenSamplers(1, &ogl_renderer->sampler);
  ogl_renderer->sampler_loc = glGetUniformLocation(ogl_renderer->shader_prog.handle, "atlas");
  glUniform1i(ogl_renderer->sampler_loc, 0);

  ogl_renderer->transform_loc = glGetUniformLocation(ogl_renderer->shader_prog.handle, "transform");

  arena_release_scratch(scratch);

  result.handle = ogl_renderer;
  return(result);
}

proc void
render_ogl_flush_commands(void)
{
  ProfileFunction()
  {
    R_Commands *commands = render_commands;

    glBindVertexArray(ogl_renderer->vao);
    glBindBuffer(GL_ARRAY_BUFFER, ogl_renderer->vbo);
    glUseProgram(ogl_renderer->shader_prog.handle);

    // NOTE: pattern
    glEnableVertexAttribArray(0);
    glVertexAttribDivisor(0, 0);
    glVertexAttribPointer(0, 4, GL_FLOAT, 0, 0, 0);

    U64 pattern_size = sizeof(quad_pattern);

    // TODO: use an xlist for the shader attributes?
    // NOTE: p_min_max
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);
    glVertexAttribPointer(1, 4, GL_FLOAT, 0, sizeof(R_Quad),
                          PtrFromInt(pattern_size + OffsetOf(R_Quad, p_min)));

    // NOTE: uv_min_max
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);
    glVertexAttribPointer(2, 4, GL_FLOAT, 0, sizeof(R_Quad),
                          PtrFromInt(pattern_size + OffsetOf(R_Quad, uv_min)));

    // NOTE: color
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);
    glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, 1, sizeof(R_Quad),
                          PtrFromInt(pattern_size + OffsetOf(R_Quad, color)));

    // NOTE: angle
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);
    glVertexAttribPointer(4, 1, GL_FLOAT, 0, sizeof(R_Quad),
                          PtrFromInt(pattern_size + OffsetOf(R_Quad, angle)));

    // NOTE: level
    glEnableVertexAttribArray(5);
    glVertexAttribDivisor(5, 1);
    glVertexAttribPointer(5, 1, GL_FLOAT, 0, sizeof(R_Quad),
                          PtrFromInt(pattern_size + OffsetOf(R_Quad, level)));

    glViewport(0, 0, commands->viewport_dim.width, commands->viewport_dim.height);

    V4 clear_color = commands->clear_color;
    glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // NOTE: render all batches
    ProfileScope(render_batches)
    {
      Mat4 transform = mat4_id();
      for(R_TransformKind kind = 0; kind < R_Transform_Count; ++kind)
      {
	R_BatchList *list = commands->batch_lists + kind;
	transform = mat4_mul(transform, commands->transforms[kind]);
	glUniformMatrix4fv(ogl_renderer->transform_loc, 1, 1, (GLfloat*)transform.v);

	for(R_Batch *batch = list->first_batch; batch; batch = batch->next)
	{
	  ProfileScope(render_batch)
	  {
	    U64 size = batch->quad_count*sizeof(R_Quad);
	    glBufferSubData(GL_ARRAY_BUFFER, pattern_size, size, batch->quads);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, (U32)IntFromPtr(batch->texture->handle.handle));

	    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, batch->quad_count);
	  }
	}
      }
    }

    // NOTE: move all batches onto the freelist
    for(R_TransformKind kind = 0; kind < R_Transform_Count; ++kind)
    {
      R_BatchList *list = commands->batch_lists + kind;
      if(list->first_batch)
      {
	list->last_batch->next = commands->batch_freelist;
	commands->batch_freelist = list->first_batch;
	list->first_batch = 0;
	list->last_batch = 0;
	list->batch_count = 0;
      }
    }

    //gfx_window_end_frame(commands->window);
  }
}

// -----------------------------------------------------------------------------
// render common api

proc R_Handle
render_backend_init(Arena *arena)
{
  return(render_ogl_backend_init(arena));
}

proc R_Texture
render_create_texture_ex(S32 width, S32 height, void *pixels, R_TextureCreateParams *params)
{
  //return(render_ogl_create_texture(v2s32(width, height), pixels, params));
  GLuint handle = ogl_create_texture(v2s32(width, height), pixels,
				     ogl_fmts[params->internal_fmt], ogl_fmts[params->pixel_fmt],
				     params->wrap_x, params->wrap_y);

  R_Texture result = {0};
  result.handle = render__handle_from_ogl_handle(handle);
  result.dim.width = width;
  result.dim.height = height;
  result.pixels = pixels;
  return(result);
}

proc R_Handle
render_create_framebuffer(S32 width, S32 height)
{
  GLuint fbo = ogl_create_framebuffer(width, height);

  R_Handle result = render__handle_from_ogl_handle(fbo);
  return(result);
}

proc void
render_read_framebuffer_pixels(R_Handle framebuffer, V2S32 pos, V2S32 dim, U32 *pixels)
{
  GLuint fbo = ogl__handle_from_render_handle(framebuffer);
  ogl_read_framebuffer_pixels(fbo, pos, dim, pixels);
}

proc void
render_flush_commands(void)
{
  render_ogl_flush_commands();
}

/* proc void */
/* render_end_frame(void) */
/* { */
/*   render_ogl_end_frame(); */
/* } */
