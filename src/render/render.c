proc B32
render_init(void)
{
  Arena *arena = arena_alloc();
  R_Commands *commands = arena_push_struct_z(arena, R_Commands);
  commands->arena = arena;

  commands->renderer = render_backend_init(arena);

  // NOTE: generate white texture
  U8 white[256];
  SetSize(white, U8_MAX, ArrayCount(white));   
  commands->white_texture = render_create_texture(16, 16, R_PixelFormat_red, R_PixelFormat_red, white);
  
  render_commands = commands;
  B32 result = render_commands != 0;
  return(result);
}

proc R_Font*
render_alloc_font(LooseFont *loose_font)
{
  R_Commands *commands = render_commands;
  R_Font *result = font_pack(commands->arena, loose_font);
  return(result);
}

proc void
render_equip_window(Os_Handle window)
{
  render_commands->window = window;
}

proc void
render_begin_frame(void)
{
  R_Commands *commands = render_commands;
  os_window_begin_frame(commands->window);

  commands->window_dim = os_window_get_dim(commands->window);
  commands->ndc_scale = v2(2.f/(R32)commands->window_dim.width,
			   2.f/(R32)commands->window_dim.height);  
}

proc void
render_rect(R_Texture *texture, Rect2 rect, Rect2 uv, R32 level, V4 color)
{
  R_Commands *commands = render_commands;
  // NOTE: if a texture was not supplied, use the white texture
  if(!texture) {
    texture = &commands->white_texture;
    uv = rect2(v2(0, 0), v2(1, 1));
  }

  // NOTE: check if texture is already used in a batch
  R_Batch *batch = 0;
  for(R_Batch *current_batch = commands->first_batch; current_batch; current_batch = current_batch->next) {

    if(texture->handle.handle == current_batch->texture->handle.handle) {

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
    PackedGlyph *glyph = font_glyph_from_codepoint(font, c);

    Rect2 rect = rect2(v2_add(pos, v2_hadamard(ndc_scale, glyph->rect.min)),
		       v2_add(pos, v2_hadamard(ndc_scale, glyph->rect.max)));
    render_rect(&font->atlas, rect, glyph->uv, level, color);
    
    pos.x += ndc_scale.x * glyph->advance;
  }
}
