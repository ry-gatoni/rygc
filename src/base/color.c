proc U32
color_u32_from_rgba(U8 r, U8 g, U8 b, U8 a)
{
  U32 result = (((U32)(r) << 0*8) |
                ((U32)(g) << 1*8) |
                ((U32)(b) << 2*8) |
                ((U32)(a) << 3*8));
  return(result);
}

proc U32
color_u32_from_v3(V3 color, R32 a)
{
  U32 result = ((((U32)(color.r*255.f)) << 0*8) |
                (((U32)(color.g*255.f)) << 1*8) |
                (((U32)(color.b*255.f)) << 2*8) |
                (((U32)(a*255.f)) << 3*8));
  return(result);
}

// TODO: should there be two of these, for each endianness?
proc U32
color_u32_from_v4(V4 color)
{
  U32 result = ((((U32)(color.r*255.f)) << 0*8) |
                (((U32)(color.g*255.f)) << 1*8) |
                (((U32)(color.b*255.f)) << 2*8) |
                (((U32)(color.a*255.f)) << 3*8));
  return(result);
}

proc V4
color_v4_from_rgba(U8 r, U8 g, U8 b, U8 a)
{
  R32 inv_255 = 1.f / 255.f;
  V4 result = v4((R32)r * inv_255, (R32)g * inv_255, (R32)b * inv_255, (R32)a * inv_255);
  return(result);
}

proc V4
color_v4_from_rgba_u32(U32 rgba)
{
  U8 r = (rgba >> (0*8)) & 0xFF;
  U8 g = (rgba >> (1*8)) & 0xFF;
  U8 b = (rgba >> (2*8)) & 0xFF;
  U8 a = (rgba >> (3*8)) & 0xFF;
  V4 result = color_v4_from_rgba(r, g, b, a);
  return(result);
}
