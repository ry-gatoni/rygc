proc void
render_init(void)
{
  // TODO: pull out OpenGL-specific stuff
  glEnable(GL_TEXTURE_2D);
      
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glDepthFunc(GL_LESS);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);

  Arena *arena = arena_alloc();
  render_commands = render_alloc_commands(arena);
}

proc R_Font*
render_alloc_font(LooseFont *loose_font)
{
  Arena *arena = render_commands->arena;
  PackedFont *font = font_pack(arena, loose_font);
  R_Texture *atlas = arena_push_struct(arena, R_Texture);
  atlas->handle = font->atlas_texture_id;
  atlas->dim = v2s32(font->atlas_width, font->atlas_height);
      
  R_Font *result = arena_push_struct(arena, R_Font);      
  result->font = font;
  result->atlas = atlas;
  return(result);
}

// TODO: still depends on OpenGL
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
render_equip_window(Os_Handle window)
{
  render_commands->window = window;
}

// TODO: depends on OpenGL
proc void
render_begin_frame(void)
{
  R_Commands *commands = render_commands;
  os_window_begin_frame(commands->window);

  commands->window_dim = os_window_get_dim(commands->window);
  commands->ndc_scale = v2(2.f/(R32)commands->window_dim.width,
			   2.f/(R32)commands->window_dim.height);

  glViewport(0, 0, commands->window_dim.width, commands->window_dim.height);
  glClearColor(0, 0, 0, 1);
  glClearDepth(1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// TODO: still depends on OpenGL
proc void
render_end_frame(void)
{
  R_Commands *commands = render_commands;
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

  os_window_end_frame(commands->window);
}

proc void
render_rect(R_Texture *texture, Rect2 rect, Rect2 uv, R32 level, V4 color)
{
  R_Commands *commands = render_commands;
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

  //R32 level = (R32)render_level/(R32)RenderLevel_count;
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
render_string(R_Font *font, String8 string, V2 pos, R32 level, V4 color)
{
  R_Commands *commands = render_commands;
  V2 ndc_scale = v2(2.f/(R32)commands->window_dim.x,
		    2.f/(R32)commands->window_dim.y);

  for(U32 char_idx = 0; char_idx < string.count; ++char_idx) {

    U8 c = string.string[char_idx];
    PackedGlyph *glyph = font_glyph_from_codepoint(font->font, c);

    Rect2 rect = rect2(v2_add(pos, v2_hadamard(ndc_scale, glyph->rect.min)),
		       v2_add(pos, v2_hadamard(ndc_scale, glyph->rect.max)));
    render_rect(font->atlas, rect, glyph->uv, level, color);
    
    pos.x += ndc_scale.x * glyph->advance;
  }
}
