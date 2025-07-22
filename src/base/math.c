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
