#if OS_WINDOWS
#  include "OpenGl/windows/ogl_win32.c"
#endif

proc Ogl_Shader
ogl_make_shader(Arena *arena, char *src, GLenum kind)
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

  Ogl_Shader result = {0};
  result.handle = shader_id;
  result.log = log;
  return(result);
}

proc Ogl_Shader
ogl_make_program(Arena *arena, GLuint *shaders, U32 shader_count)
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

  Ogl_Shader result = {0};
  result.handle = program_id;
  result.log = log;
  return(result);
}

// NOTE: render implementations
proc R_Texture
render_create_texture(S32 width, S32 height, R_PixelFormat internal_fmt, R_PixelFormat pixel_fmt, void *pixels)
{
  U32 handle;
  glGenTextures(1, &handle);
  glBindTexture(GL_TEXTURE_2D, handle);

  glTexImage2D(GL_TEXTURE_2D, 0, ogl_fmts[internal_fmt], width, height, 0, ogl_fmts[pixel_fmt], GL_UNSIGNED_BYTE, pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  R_Texture result = {0};
  result.handle.handle = PtrFromInt(handle);
  result.dim = v2s32(width, height);
  return(result);
}

proc void
render_update_texture(R_Texture *texture, S32 pos_x, S32 pos_y, S32 width, S32 height, R_PixelFormat format, void *pixels)
{
  U32 handle = (U32)IntFromPtr(texture->handle.handle);
  glBindTexture(GL_TEXTURE_2D, handle);

  glTexSubImage2D(GL_TEXTURE_2D, 0, pos_x, pos_y, width, height, ogl_fmts[format], GL_UNSIGNED_BYTE, pixels);
}

proc R_Handle
render_backend_init(Arena *arena)
{
  ogl_renderer = arena_push_struct(arena, Ogl_Renderer);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glDepthFunc(GL_LESS);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);

  ArenaTemp scratch = arena_get_scratch(&arena, 1);
  ogl_renderer->vert_shader = ogl_make_shader(scratch.arena, vert_shader_src, GL_VERTEX_SHADER);
  if(ogl_renderer->vert_shader.log.count) {
    fprintf(stderr, "ERROR: vert_shader: %.*s\n",
            (int)ogl_renderer->vert_shader.log.count, ogl_renderer->vert_shader.log.string);
  }

  ogl_renderer->frag_shader = ogl_make_shader(scratch.arena, frag_shader_src, GL_FRAGMENT_SHADER);
  if(ogl_renderer->frag_shader.log.count) {
    fprintf(stderr, "ERROR: frag_shader: %.*s\n",
            (int)ogl_renderer->frag_shader.log.count, ogl_renderer->frag_shader.log.string);
  }

  GLuint shaders[] = {
    ogl_renderer->vert_shader.handle,
    ogl_renderer->frag_shader.handle,
  };
  ogl_renderer->shader_prog = ogl_make_program(scratch.arena, shaders, ArrayCount(shaders));
  if(ogl_renderer->shader_prog.log.count) {
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

  R_Handle result = {0};
  result.handle = ogl_renderer;
  return(result);
}

proc void
render_end_frame(void)
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

    glUniformMatrix4fv(ogl_renderer->transform_loc, 1, 0, (GLfloat*)commands->transform.v);

    glViewport(0, 0, commands->window_dim.width, commands->window_dim.height);
    glClearColor(0.2, 0.2, 0.2, 1);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // NOTE: render all batches
    ProfileScope(render_batches)
    {
      for(R_Batch *batch = commands->first_batch; batch; batch = batch->next)
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

    // NOTE: move all batches onto the freelist
    commands->last_batch->next = commands->batch_freelist;
    commands->batch_freelist = commands->first_batch;
    commands->first_batch = 0;
    commands->last_batch = 0;
    commands->batch_count = 0;

    os_window_end_frame(commands->window);
  }
}
