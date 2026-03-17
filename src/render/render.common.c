proc B32
render_init(void)
{
  Arena *arena = arena_alloc();
  R_Commands *commands = arena_push_struct_z(arena, R_Commands);
  commands->arena = arena;

  commands->renderer = render_backend_init(arena);

  // NOTE: generate white texture
  U32 white[] = { 0xFFFFFFFF };
  commands->white_texture = render_create_texture(1, 1, white);

  render_commands = commands;
  B32 result = render_commands != 0;
  return(result);
}

// -----------------------------------------------------------------------------
// font

/* proc R_Font* */
/* render_alloc_font(LooseFont *loose_font) */
/* { */
/*   R_Commands *commands = render_commands; */
/*   R_Font *result = font_pack(commands->arena, loose_font); */
/*   return(result); */
/* } */

#if defined(GFX_LAYER)
// -----------------------------------------------------------------------------
// windowing

proc void
render_equip_window(Os_Handle window)
{
  render_commands->window = window;
}

proc void
render_begin_frame(void)
{
  R_Commands *commands = render_commands;
  gfx_window_begin_frame(commands->window);

  commands->window_dim = gfx_window_get_dim(commands->window);
  // TODO: different transforms for different rendering backends??
  commands->transform_device_from_screen = mat4_screen_transform_ndc(commands->window_dim);
}

proc void
render_end_frame(void)
{
  render_flush_commands();
  gfx_window_end_frame();
}

#endif
// -----------------------------------------------------------------------------
// batch helpers

proc R_Batch*
render_batch_alloc(B32 push_front)
{
  R_Batch *result = render_commands->batch_freelist;
  if(result)
  {
    result->quad_count = 0;
    SLLStackPop(render_commands->batch_freelist);
  }
  else
  {
    result = arena_push_struct_z(render_commands->arena, R_Batch);
    result->quad_cap = RENDER_BATCH_QUAD_CAP;
    result->quads = arena_push_array_z(render_commands->arena, R_Quad, result->quad_cap);
  }

  R_BatchList *list = render_commands->batch_lists + render_commands->active_transform;
  if(push_front)
  {
    SLLQueuePushFront(list->first_batch, list->last_batch, result);
  }
  else
  {
    SLLQueuePush(list->first_batch, list->last_batch, result);
  }
  ++list->batch_count;

  return(result);
}

proc R_Batch*
render_batch_for_texture(R_Texture *texture)
{
  R_Commands *commands = render_commands;

  R_Batch *batch = 0;
  R_BatchList *list = commands->batch_lists + commands->active_transform;
  // NOTE: check if texture is already used in a batch
  for(R_Batch *current_batch = list->first_batch;
      current_batch;
      current_batch = current_batch->next)
  {
    if(texture->handle.handle == current_batch->texture->handle.handle)
    {
      batch = current_batch;
      break;
    }
  }

  // NOTE: if there is not already a batch with this texture, allocate a new one and add it to the list
  if(batch == 0)
  {
    batch = render_batch_alloc(0);
    batch->texture = texture;
  }

  return(batch);
}

// -----------------------------------------------------------------------------
// transforms

proc void
render_set_world_transform(Mat4 mat)
{
  render_commands->transform_screen_from_world = mat;
}

proc void
render_equip_push_transform(R_TransformKind transform)
{
  render_commands->active_transform = transform;
}

// -----------------------------------------------------------------------------
// drawing

proc void
render_texture(R_Texture *texture, Rect2 rect, Rect2 uv, R32 angle, R32 level, V4 color)
{
  R_Batch *batch = render_batch_for_texture(texture);

  if(batch->quad_count >= batch->quad_cap)
  {
    batch = render_batch_alloc(1);
    batch->texture = texture;
  }

  R_Quad *quad = batch->quads + batch->quad_count++;
  quad->p_min = rect.min;
  quad->p_max = rect.max;
  quad->uv_min = uv.min;
  quad->uv_max = uv.max;
  quad->color = color_u32_from_v4(color);
  quad->angle = angle;
  quad->level = level;

  R_BatchList *list = render_commands->batch_lists + render_commands->active_transform;
  ++list->total_quad_count;
}

proc void
render_rect(Rect2 rect, R32 angle, R32 level, V4 color)
{
  R_Texture *texture = &render_commands->white_texture;
  Rect2 uv = rect2(v2(0, 0), v2(1, 1));
  render_texture(texture, rect, uv, angle, level, color);
}

proc void
render_line_segment(V2 p0, V2 p1, R32 thickness, R32 level, V4 color)
{
  V2 center = v2_lmul(0.5f, v2_add(p0, p1));
  V2 direction = v2_sub(p1, p0);

  R32 angle = v2_angle(direction);
  R32 length = v2_length(direction);

  V2 dim = v2(length, thickness);
  Rect2 rect = rect2_center_dim(center, dim);

  render_rect(rect, angle, level, color);
}
