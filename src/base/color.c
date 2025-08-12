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
