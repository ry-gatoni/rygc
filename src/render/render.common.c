proc B32
render_init(void)
{
  B32 result = 1;
  Arena *arena = arena_alloc();
  render_commands = arena_push_struct_z(arena, R_Commands);
  render_commands->arena = arena;

  render_commands->clear_color = color_v4_from_rgba(0x40, 0x40, 0x40, 0xFF);

  // NOTE: initialize transforms
  {
    // NOTE: default identity projection transforms
    {
      Mat4 *forward = &render_forward_projection(identity);
      Mat4 *inverse = &render_inverse_projection(identity);
      transform4_id(forward, inverse);
    }

    // NOTE: default orthographic projection transforms
    {
      Mat4 *forward = &render_forward_projection(orthographic);
      Mat4 *inverse = &render_inverse_projection(orthographic);
      transform4_id(forward, inverse);
    }

    // NOTE: default perspective projection transforms
    {
      Mat4 *forward = &render_forward_projection(perspective);
      Mat4 *inverse = &render_inverse_projection(perspective);
      transform4_id(forward, inverse);
    }

    // NOTE: default view transforms
    {
      // one past last projection
      Mat4 *forward = &render_forward_projection(Count);
      Mat4 *inverse = &render_inverse_projection(Count);
      transform4_id(forward, inverse);
    }
  }
  render_commands->used_transform_idx = R_ProjTransformKind_Count + 1;
  render_make_and_use_transform(render_default_view(), render_projection(R_ProjTransformKind_orthographic));

  render_commands->renderer = render_backend_init(arena);

  // NOTE: generate white texture
  U32 white[] = { 0xFFFFFFFF };
  render_commands->white_texture = render_create_texture(1, 1, white);

  return(result);
}

proc void
render_set_backend(R_Backend backend)
{
  render_commands->backend = backend;
}

#if defined(FONT_LAYER)
// -----------------------------------------------------------------------------
// font

proc R_Font
render_alloc_font(PackedFont *font)
{
  R_Texture texture = render_create_texture(font->atlas_width, font->atlas_height, font->atlas_pixels);

  R_Font result = {0};
  result.font = font;
  result.atlas = texture;
  return(result);
}

proc void
render_string(R_Font *font, String8 string, V2 pos, R32 level, V4 color)
{
  for(U32 char_idx = 0; char_idx < string.count; ++char_idx)
  {
    U8 c = string.string[char_idx];
    PackedGlyph *glyph = font_glyph_from_codepoint(font->font, c);

    render_texture(&font->atlas, rect2_offset(glyph->rect, pos), glyph->uv, 0, level, color);

    pos.x += glyph->advance;
  }
}

#endif
#if defined(GFX_LAYER)
// -----------------------------------------------------------------------------
// windowing

global Gfx_Backend gfx_backend_from_r_backend[R_Backend_Count] = {
  [R_Backend_software] = Gfx_Backend_software,
  [R_Backend_opengl] = Gfx_Backend_opengl,
};

proc void
render_equip_window(Gfx_Handle window)
{
  render_commands->window = window;
  gfx_set_backend(window, gfx_backend_from_r_backend[render_commands->backend]);
  gfx_render_target_from_window(render_commands->render_targets[render_commands->backend], render_commands->window);
}

proc void
render_begin_frame(void)
{
  R_Commands *commands = render_commands;

  render_set_viewport_dim(gfx_window_dim(commands->window));

  commands->used_transform_idx = R_ProjTransformKind_Count + 1;
}

proc void
render_end_frame(void)
{
  render_flush_commands();
  gfx_window_submit_frame(render_commands->window);
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

  R_BatchList *list = &render_commands->batch_list;
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
  R_BatchList *list = &commands->batch_list;
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
render_set_viewport_dim(V2S32 dim)
{
  render_commands->viewport_dim = dim;

  // NOTE: transforms
  {
    V2 dimf = v2_from_v2s32(dim);
    R32 near = 0.1f;
    R32 far = 999.f;
    R32 fov = PI32;
    R32 aspect_ratio = dimf.x / dimf.y;

    // NOTE: orhtographic projection transforms
    {
      Mat4 *forward = &render_forward_projection(orthographic);
      Mat4 *inverse = &render_inverse_projection(orthographic);
      transform4_ortho(forward, inverse, dimf, near, far);
    }

    // NOTE: perspective projection transforms
    {
      Mat4 *forward = &render_forward_projection(perspective);
      Mat4 *inverse = &render_inverse_projection(perspective);
      transform4_perspective(forward, inverse, fov, aspect_ratio, near, far);
    }
  }
}

proc R_ProjTransformId
render_projection(R_ProjTransformKind kind)
{
  Assert(kind != R_ProjTransformKind_Count);
  R_ProjTransformId result = { .idx = kind, };
  return result;
}

proc R_ViewTransformId
render_default_view(void)
{
  R_ViewTransformId result = { .idx = R_ProjTransformKind_Count, }; // one past last projection
  return result;
}

proc R_ViewTransformId
render_set_view(V3 camera_x, V3 camera_y, V3 camera_z, V3 camera_pos)
{
  U32 free_idx = render_commands->used_transform_idx;
  Assert(free_idx >= R_ProjTransformKind_Count);
  Assert(free_idx < R_MAX_TRANSFORM_COUNT);
  Mat4 *forward = &render_commands->transforms.forward[free_idx];
  Mat4 *inverse = &render_commands->transforms.inverse[free_idx];
  transform4_camera(forward, inverse, camera_x, camera_y, camera_z, camera_pos);

  R_ViewTransformId result = { .idx = free_idx, };
  ++render_commands->used_transform_idx;
  return result;
}

proc R_TransformId
render_make_and_use_transform(R_ViewTransformId view_id, R_ProjTransformId proj_id)
{
  R_TransformId result = render__make_transform_id(view_id, proj_id);
  render_use_transform(result);
  return result;
}

proc void
render_use_transform(R_TransformId id)
{
  render_commands->active_transform = id;
}

proc V3
render_unproject(V2 screen_point, R_TransformId transform)
{
  Mat4 view_inverse = render_commands->transforms.inverse[transform.view_id.idx];
  Mat4 proj_inverse = render_commands->transforms.inverse[transform.proj_id.idx];
  V2 viewport_dim = v2_from_v2s32(render_commands->viewport_dim);
  V4 near_clip_projected = v4(2.f*screen_point.x/viewport_dim.x, 2.f*screen_point.y/viewport_dim.y, -1, 1);
  V4 near_world_projected = mat4_transform(view_inverse,
					   mat4_transform(proj_inverse,
							  near_clip_projected));
  V4 res_v4 = v4_lmul(1.f/near_world_projected.w, near_world_projected);
  V3 result = res_v4.xyz;
  return result;
}

proc inline R_TransformId
render__make_transform_id(R_ViewTransformId view_id, R_ProjTransformId proj_id)
{
  Assert(proj_id.idx < R_ProjTransformKind_Count);
  Assert(view_id.idx < R_MAX_TRANSFORM_COUNT);
  Assert(proj_id.idx != view_id.idx);
  R_TransformId result = {
    .view_id = view_id,
    .proj_id = proj_id,
  };
  return result;
}

// -----------------------------------------------------------------------------
// drawing

proc void
render_set_clear_color(V4 color)
{
  render_commands->clear_color = color;
}

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
  quad->proj_transform_idx = render_commands->active_transform.proj_id.idx;
  quad->view_transform_idx = render_commands->active_transform.view_id.idx;

  R_BatchList *list = &render_commands->batch_list;
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
