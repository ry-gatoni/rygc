proc B32
is_pow_2(U32 num)
{
  B32 result = (num & (num - 1)) == 0;
  return(result);
}

proc U32
bit_reverse_u32(U32 num)
{
  U32 result = num;
  result = ((result >>  1) & 0x55555555) | ((result & 0x55555555) <<  1); // NOTE: swap odd and even bits
  result = ((result >>  2) & 0x33333333) | ((result & 0x33333333) <<  2); // NOTE: swap consecutive pairs
  result = ((result >>  4) & 0x0F0F0F0F) | ((result & 0x0F0F0F0F) <<  4); // NOTE: swap nibbles
  result = ((result >>  8) & 0x00FF00FF) | ((result & 0x00FF00FF) <<  8); // NOTE: swap bytes
  result = ((result >> 16)             ) | ((result             ) << 16); // NOTE: swap words
  return(result);
}

proc U64
bit_reverse_u64(U64 num)
{
  U64 result = num;
  result = ((result >>  1) & 0x5555555555555555) | ((result & 0x5555555555555555) <<  1); // swap odd and even bits
  result = ((result >>  2) & 0x3333333333333333) | ((result & 0x3333333333333333) <<  2); // swap consecutive pairs
  result = ((result >>  4) & 0x0F0F0F0F0F0F0F0F) | ((result & 0x0F0F0F0F0F0F0F0F) <<  4); // swap nibbles
  result = ((result >>  8) & 0x00FF00FF00FF00FF) | ((result & 0x00FF00FF00FF00FF) <<  8); // swap bytes
  result = ((result >> 16) & 0x0000FFFF0000FFFF) | ((result & 0x0000FFFF0000FFFF) << 16); // swap words
  result = ((result >> 32)                     ) | ((result                     ) << 32); // swap dwords
  return(result);
}

proc U32
rygc_log2(U32 num)
{
  struct {
    U32 shift, mask;
  } ms[] = {
    {16, 0xFFFF0000},
    { 8, 0xFF00    },
    { 4, 0xF0      },
    { 2, 0xC       },
    { 1, 0x2       },
  };  

  U32 result = 0;
  for(U32 i = 0; i < ArrayCount(ms); ++i) {

    U32 shift = ms[i].shift;
    U32 mask = ms[i].mask;
    if(num & mask) {

      num >>= shift;
      result |= shift;
    }
  }
  return(result);
}

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
rangeu32(U32 min, U32 max)
{
  RangeU32 result = {.min = min, .max = max};
  return(result);
}

proc RangeS32
ranges32(S32 min, S32 max)
{
  RangeS32 result = {.min = min, .max = max};
  return(result);
}

proc RangeU32
rangeu32_invalid(void)
{
  RangeU32 result = {.min = U32_MAX, .max = 0};
  return(result);
}

proc RangeS32
ranges32_invalid(void)
{
  RangeS32 result = {.min = S32_MAX, .max = S32_MIN};
  return(result);
}

proc U32
rangeu32_len(RangeU32 rng)
{
  U32 result = rng.max - rng.min;
  return(result);
}

proc S32
ranges32_len(RangeS32 rng)
{
  S32 result = rng.max - rng.min;
  return(result);
}

// NOTE: rects
proc Rect2
rect2(V2 min, V2 max)
{
  Rect2 result = {.min = min, .max = max};
  return(result);
}

proc Rect2
rect2_invalid(void)
{
  Rect2 result = {.min = v2(R32_MAX, R32_MAX), .max = v2(R32_MIN, R32_MIN)};
  return(result);
}

proc Rect2
rect2_min_dim(V2 min, V2 dim)
{
  Rect2 result = {.min = min, .max = v2_add(min, dim)};
  return(result);
}

proc Rect2
rect2_center_dim(V2 center, V2 dim)
{
  Rect2 result = rect2_center_halfdim(center, v2_rmul(dim, 0.5f));
  return(result);
}

proc Rect2
rect2_center_halfdim(V2 center, V2 halfdim)
{
  Rect2 result = {.min = v2_sub(center, halfdim), .max = v2_add(center, halfdim)};
  return(result);
}
