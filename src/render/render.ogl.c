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

proc R_Handle
render_ogl_backend_init(Arena *arena)
{
  R_Handle result = {0};
  if(!ogl_init()) return(result);

  render_commands->render_targets[R_Backend_opengl] = arena_push_struct(arena, Gfx_OglRenderTarget);

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

  ogl_renderer->transforms_loc = glGetUniformLocation(ogl_renderer->shader_prog.handle, "transforms");

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

    // NOTE: proj_transform_idx
    glEnableVertexAttribArray(6);
    glVertexAttribDivisor(6, 1);
    glVertexAttribPointer(6, 1, GL_UNSIGNED_SHORT, 0, sizeof(R_Quad),
                          PtrFromInt(pattern_size + OffsetOf(R_Quad, proj_transform_idx)));

    // NOTE: view_transform_idx
    glEnableVertexAttribArray(7);
    glVertexAttribDivisor(7, 1);
    glVertexAttribPointer(7, 1, GL_UNSIGNED_SHORT, 0, sizeof(R_Quad),
                          PtrFromInt(pattern_size + OffsetOf(R_Quad, view_transform_idx)));

    glViewport(0, 0, commands->viewport_dim.width, commands->viewport_dim.height);

    V4 clear_color = commands->clear_color;
    glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // NOTE: render all batches
    //ProfileScope(render_batches)
    {
      GLenum glerr;
      glUniformMatrix4fv(ogl_renderer->transforms_loc, commands->used_transform_idx, 1,
			 (GLfloat*)&commands->transforms.forward[0]);
      if((glerr = glGetError()))
      {
	Assert(0);
      }

      R_BatchList *list = &commands->batch_list;

      for(R_Batch *batch = list->first_batch; batch; batch = batch->next)
      {
	//ProfileScope(render_batch)
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
    R_BatchList *list = &commands->batch_list;
    if(list->first_batch)
    {
      list->last_batch->next = commands->batch_freelist;
      commands->batch_freelist = list->first_batch;
      list->first_batch = 0;
      list->last_batch = 0;
      list->batch_count = 0;
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

proc void
render_update_texture(R_Texture *texture, S32 pos_x, S32 pos_y, S32 width, S32 height, R_PixelFormat format, void *pixels)
{
  GLuint handle = ogl__handle_from_render_handle(texture->handle);
  GLint pixel_fmt = ogl_fmts[format];
  ogl_update_texture(handle, v2s32(pos_x, pos_y), v2s32(width, height), pixel_fmt, pixels);
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
