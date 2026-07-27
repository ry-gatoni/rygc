proc U32 color_u32_from_rgba(U8 r, U8 g, U8 b, U8 a);
proc U32 color_u32_from_v3(V3 color, R32 a);
proc U32 color_u32_from_v4(V4 color);

proc V3 color_v3_from_rgb(U8 r, U8 g, U8 b);
proc V3 color_blend_v3(V3 lo, V3 hi, R32 val);

proc V4 color_v4_from_rgba(U8 r, U8 g, U8 b, U8 a);
proc V4 color_v4_from_rgba_u32(U32 rgba);
proc V4 color_blend_v4(V4 lo, V4 hi, R32 val);
