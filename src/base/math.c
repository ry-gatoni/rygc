// NOTE: vectors
proc V2
v2(R32 x, R32 y)
{
  V2 result = {.x = x, .y = y};
  return(result);
}

proc V3
v3(R32 x, R32 y, R32 z)
{
  V3 result = {.x = x, .y = y, .z = z};
  return(result);
}

proc V4
v4(R32 x, R32 y, R32 z, R32 w)
{
  V4 result = {.x = x, .y = y, .z = z, .w = w};
  return(result);
}

proc V2S32
v2s32(S32 x, S32 y)
{
  V2S32 result = {.x = x, .y = y};
  return(result);
}

proc V2
v2_from_v2s32(V2S32 v)
{
  V2 result = {.x = v.x, .y = v.y};
  return(result);
}
     
proc V2
v2_add(V2 v, V2 w)
{
  V2 result = {.x = v.x + w.x, .y = v.y + w.y};
  return(result);
}

proc V2
v2_sub(V2 v, V2 w)
{
  V2 result = {.x = v.x - w.x, .y = v.y - w.y};
  return(result);
}

proc void
v2_inc(V2 *v, V2 w)
{
  v->x += w.x;
  v->y += w.y;
}

proc void
v2_dec(V2 *v, V2 w)
{
  v->x -= w.x;
  v->y -= w.x;
}

proc V2
v2_lmul(R32 a, V2 v)
{
  V2 result = {.x = a*v.x, .y = a*v.y};
  return(result);
}

proc V2
v2_rmul(V2 v, R32 a)
{
  V2 result = {.x = a*v.x, .y = a*v.y};
  return(result);
}

proc void
v2_scale(V2 *v, R32 a)
{
  v->x *= a;
  v->y *= a;
}

proc V2
v2_hadamard(V2 v, V2 w)
{
  V2 result = {.x = v.x * w.x, .y = v.y *w .y};
  return(result);
}

// NOTE: ranges
proc RangeU32
make_rng_u32_invalid(void)
{
  RangeU32 result = {.min = U32_MAX, .max = 0};
  return(result);
}

proc RangeS32
make_rng_s32_invalid(void)
{
  RangeS32 result = {.min = S32_MAX, .max = S32_MIN};
  return(result);
}

proc U32
rng_u32_len(RangeU32 rng)
{
  U32 result = rng.max - rng.min;
  return(result);
}

proc S32
rng_s32_len(RangeS32 rng)
{
  S32 result = rng.max - rng.min;
  return(result);
}

// NOTE: rects
proc Rect2
make_rect2(V2 min, V2 max)
{
  Rect2 result = {.min = min, .max = max};
  return(result);
}

proc Rect2
make_rect2_min_dim(V2 min, V2 dim)
{
  Rect2 result = {.min = min, .max = v2_add(min, dim)};
  return(result);
}

proc Rect2
make_rect2_center_dim(V2 center, V2 dim)
{
  Rect2 result = make_rect2_center_halfdim(center, v2_rmul(dim, 0.5f));
  return(result);
}

proc Rect2
make_rect2_center_halfdim(V2 center, V2 halfdim)
{
  Rect2 result = {.min = v2_sub(center, halfdim), .max = v2_add(center, halfdim)};
  return(result);
}
