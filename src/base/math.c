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

// TODO: stop depending on crt

proc R32
rygc_sqrt(R32 num)
{
  return(sqrtf(num));
}

proc R32
rygc_cos(R32 num)
{
  return(cosf(num));
}

proc R32
rygc_sin(R32 num)
{
  return(sinf(num));
}

proc R32
rygc_atan2(R32 x, R32 y)
{
  return(atan2f(y, x));
}

proc R32
lerp(R32 val0, R32 val1, R32 t)
{
  R32 result = (1 - t)*val0 + t*val1;
  return(result);
}

// -----------------------------------------------------------------------------
// vectors

// -----------------------------------------------------------------------------
// constructors

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

proc V4
v4_from_v3_xyz(V3 xyz, R32 w)
{
  V4 result = v4(xyz.x, xyz.y, xyz.z, w);
  return(result);
}

proc V2
v2_polar(R32 mag, R32 angle)
{
  V2 result = v2_lmul(mag, v2(rygc_cos(angle), rygc_sin(angle)));
  return(result);
}

// -----------------------------------------------------------------------------
// proj/embed

proc V4
v4_embed_xy(V2 xy)
{
  V4 result = v4(xy.x, xy.y, 0, 0);
  return(result);
}

proc V2
v4_proj_xy(V4 v)
{
  V2 result = v2(v.x, v.y);
  return(result);
}

// -----------------------------------------------------------------------------
// casts

proc V2S32
v2s32(S32 x, S32 y)
{
  V2S32 result = {.x = x, .y = y};
  return(result);
}

proc V2
v2_from_v2s32(V2S32 v)
{
  V2 result = {.x = (R32)v.x, .y = (R32)v.y};
  return(result);
}

// -----------------------------------------------------------------------------
// utils

proc V2
v2_normalized(V2 v)
{
  R32 length = v2_length(v);
  V2 result = v2_lmul(1.f/length, v);
  return(result);
}

proc V3
v3_normalized(V3 v)
{
  R32 length = v3_length(v);
  V3 result = v3_lmul(1.f/length, v);
  return(result);
}

proc V4
v4_normalized(V4 v)
{
  R32 length = v4_length(v);
  V4 result = v4_lmul(1.f/length, v);
  return(result);
}

// -----------------------------------------------------------------------------
// binops

//
// v2
//

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

proc R32
v2_dot(V2 v, V2 w)
{
  R32 result = v.x*w.x + v.y*w.y;
  return(result);
}

//
// v3
//

proc V3
v3_add(V3 v, V3 w)
{
  V3 result = v3(v.x + w.x, v.y + w.y, v.z + w.z);
  return(result);
}

proc V3
v3_sub(V3 v, V3 w)
{
  V3 result = v3(v.x - w.x, v.y - w.y, v.z - w.z);
  return(result);
}

proc void
v3_inc(V3 *v, V3 w)
{
  v->x += w.x;
  v->y += w.y;
  v->z += w.z;
}

proc void
v3_dec(V3 *v, V3 w)
{
  v->x -= w.x;
  v->y -= w.y;
  v->z -= w.z;
}

proc V3
v3_lmul(R32 a, V3 v)
{
  V3 result = v3(a*v.x, a*v.y, a*v.z);
  return(result);
}

proc V3
v3_rmul(V3 v, R32 a)
{
  V3 result = v3_lmul(a, v);
  return(result);
}

proc void
v3_scale(V3 *v, R32 a)
{
  v->x *= a;
  v->y *= a;
  v->z *= a;
}

proc V3
v3_hadamard(V3 v, V3 w)
{
  V3 result = v3(v.x * w.x, v.y * w.y, v.z * w.z);
  return(result);
}

proc R32
v3_dot(V3 v, V3 w)
{
  R32 result = v.x*w.x + v.y*w.y + v.z*w.z;
  return(result);
}

proc V3
v3_cross(V3 v, V3 w)
{
  V3 result = v3(v.y*w.z - v.z*w.y, -v.x*w.z + v.z*w.x, v.x*w.y - v.y*w.x);
  return(result);
}

//
// v4
//

proc V4
v4_add(V4 v, V4 w)
{
  V4 result = v4(v.x + w.x, v.y + w.y, v.z + w.z, v.w + w.w);
  return(result);
}

proc V4
v4_sub(V4 v, V4 w)
{
  V4 result = v4(v.x - w.x, v.y - w.y, v.z - w.z, v.w - w.w);
  return(result);
}

proc void
v4_inc(V4 *v, V4 w)
{
  v->x += w.x;
  v->y += w.y;
  v->z += w.z;
  v->w += w.w;
}

proc void
v4_dec(V4 *v, V4 w)
{
  v->x -= w.x;
  v->y -= w.y;
  v->z -= w.z;
  v->w -= w.w;
}

proc V4
v4_lmul(R32 a, V4 v)
{
  V4 result = v4(a * v.x, a * v.y, a * v.z, a * v.w);
  return(result);
}

proc V4
v4_rmul(V4 v, R32 a)
{
  V4 result = v4_lmul(a, v);
  return(result);
}

proc void
v4_scale(V4 *v, R32 a)
{
  v->x *= a;
  v->y *= a;
  v->z *= a;
  v->w *= a;
}

proc V4
v4_hadamard(V4 v, V4 w)
{
  V4 result = v4(v.x * w.x, v.y * w.y, v.z * w.z, v.w * w.w);
  return(result);
}

proc R32
v4_dot(V4 v, V4 w)
{
  R32 result = v.x*w.x + v.y*w.y + v.z*w.z + v.w*w.w;
  return(result);
}

// -----------------------------------------------------------------------------
// scalar from vector

proc R32
v2_length(V2 v)
{
  R32 result = rygc_sqrt(v2_dot(v, v));
  return(result);
}

proc R32
v2_angle(V2 v)
{
  R32 result = rygc_atan2(v.x, v.y);
  return(result);
}

proc R32
v3_length(V3 v)
{
  R32 result = rygc_sqrt(v3_dot(v, v));
  return(result);
}

proc R32
v4_length(V4 v)
{
  R32 result = rygc_sqrt(v4_dot(v, v));
  return(result);
}

// -----------------------------------------------------------------------------
// interpolation

proc V2
lerp_v2(V2 v, V2 w, R32 t)
{
  // r = (1 - t) * v + t * w;
  V2 result = v2_add(v2_lmul(1.f - t, v),
                     v2_lmul(t, w));
  return(result);
}

proc V3
lerp_v3(V3 v, V3 w, R32 t)
{
  V3 result = v3_add(v3_lmul(1.f - t, v),
                     v3_lmul(t, w));
  return(result);
}

proc V4
lerp_v4(V4 v, V4 w, R32 t)
{
  V4 result = v4_add(v4_lmul(1.f - t, v),
                     v4_lmul(t, w));
  return(result);
}

// -----------------------------------------------------------------------------
// ranges

// -----------------------------------------------------------------------------
// construction

proc RangeU32
range_u32(U32 min, U32 max)
{
  RangeU32 result = {.min = min, .max = max};
  return(result);
}

proc RangeS32
range_s32(S32 min, S32 max)
{
  RangeS32 result = {.min = min, .max = max};
  return(result);
}

proc RangeR32
range_r32(R32 min, R32 max)
{
  RangeR32 result = {.min = min, .max = max};
  return(result);
}

proc RangeU32
range_u32_invalid(void)
{
  RangeU32 result = {.min = U32_MAX, .max = 0};
  return(result);
}

proc RangeS32
range_s32_invalid(void)
{
  RangeS32 result = {.min = S32_MAX, .max = S32_MIN};
  return(result);
}

proc RangeR32
range_r32_invalid(void)
{
  RangeR32 result = {.min = R32_MAX, .min = R32_MIN};
  return(result);
}

// -----------------------------------------------------------------------------
// scalar from range

proc U32
range_u32_len(RangeU32 rng)
{
  U32 result = rng.max - rng.min;
  return(result);
}

proc S32
range_s32_len(RangeS32 rng)
{
  S32 result = rng.max - rng.min;
  return(result);
}

proc R32
range_r32_len(RangeR32 rng)
{
  R32 result = rng.max - rng.min;
  return(result);
}

proc R32
range_r32_map_01(R32 val, RangeR32 rng)
{
  R32 val_clamped = ClampToRange(val, rng.min, rng.max);
  R32 result = (val_clamped - rng.min)/(rng.max - rng.min);
  return(result);
}

// -----------------------------------------------------------------------------
// rects

// -----------------------------------------------------------------------------
// construction

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

// -----------------------------------------------------------------------------
// modification

proc Rect2
rect2_offset(Rect2 rect, V2 offset)
{
  Rect2 result = {.min = v2_add(rect.min, offset), .max = v2_add(rect.max, offset)};
  return(result);
}

// -----------------------------------------------------------------------------
// vector from rect

proc V2
rect2_center(Rect2 rect)
{
  V2 result = v2_lmul(0.5f, v2_sub(rect.max, rect.min));
  return(result);
}

proc V2
rect_dim(Rect2 rect)
{
  V2 result = v2_sub(rect.max, rect.min);
  return(result);
}

// -----------------------------------------------------------------------------
// complex

// -----------------------------------------------------------------------------
// construction

proc C64
c64(R32 re, R32 im)
{
  C64 result = { .re = re, .im = im};
  return(result);
}

proc C64
c64_from_v2(V2 v)
{
  C64 result = c64(v.x, v.y);
  return(result);
}

proc V2
v2_from_c64(C64 z)
{
  V2 result = v2(z.re, z.im);
  return(result);
}

proc R32
c64_mag_sq(C64 z)
{
  R32 result = z.re*z.re + z.im*z.im;
  return(result);
}

// -----------------------------------------------------------------------------
// binops

proc C64
c64_add(C64 z, C64 w)
{
  C64 result = c64(z.re + w.re, z.im + w.im);
  return(result);
}

proc C64
c64_sub(C64 z, C64 w)
{
  C64 result = c64(z.re - w.re, z.im - w.im);
  return(result);
}

proc C64
c64_mul(C64 z, C64 w)
{
  C64 result = c64(z.re*w.re - z.im*w.im, z.re*w.im + z.im*w.re);
  return(result);
}

proc C64
c64_div(C64 z, C64 w)
{
  R32 w_mag_sq_inv = 1.f/c64_mag_sq(w);
  C64 result = c64((z.re*w.re + z.im*w.im)*w_mag_sq_inv, (z.im*w.re - z.re*w.im)*w_mag_sq_inv);
  return(result);
}

proc C64
c64_lscale(R32 r, C64 z)
{
  C64 result = c64(r*z.re, r*z.im);
  return(result);
}

proc C64
c64_rscale(C64 z, R32 r)
{
  return(c64_lscale(r, z));
}

// -----------------------------------------------------------------------------
// matrices

// -----------------------------------------------------------------------------
// construction

proc Mat3
mat3(V3 r0, V3 r1, V3 r2)
{
  Mat3 result = {.r0 = r0, .r1 = r1, .r2 = r2};
  return(result);
}

proc Mat3
mat3_id(void)
{
  Mat3 result = mat3(v3(1, 0, 0), v3(0, 1, 0), v3(0, 0, 1));
  return(result);
}

proc Mat4
mat4(V4 r0, V4 r1, V4 r2, V4 r3)
{
  Mat4 result = {.r0 = r0, .r1 = r1, .r2 = r2, .r3 = r3};
  return(result);
}

proc Mat4
mat4_id(void)
{
  Mat4 result = mat4(v4(1, 0, 0, 0),
                     v4(0, 1, 0, 0),
                     v4(0, 0, 1, 0),
                     v4(0, 0, 0, 1));
  return(result);
}

proc Mat4
mat4_yflip(void)
{
  Mat4 result = mat4(v4(1,  0,  0,  0),
                     v4(0, -1,  0,  0),
                     v4(0,  0,  1,  0),
                     v4(0,  0,  0,  1));
  return(result);
}

proc Mat4
mat4_screen_transform_ndc(V2S32 dim)
{
  R32 a = 2.f/(R32)dim.width;
  R32 b = 2.f/(R32)dim.height;
  Mat4 result = mat4(v4(a, 0,  0, -1),
                     v4(0, b,  0, -1),
                     v4(0, 0,  1,  0),
                     v4(0, 0, -1,  0));
  return(result);
}

proc Mat4
mat4_screen_from_world(V2 world_origin_in_screen_space, R32 pixels_from_world_units)
{
  R32 a = pixels_from_world_units;
  R32 off_x = world_origin_in_screen_space.x;
  R32 off_y = world_origin_in_screen_space.y;
  Mat4 result = mat4(v4(a, 0, 0, off_x),
		     v4(0, a, 0, off_y),
		     v4(0, 0, 1,     0),
		     v4(0, 0, 0,     1));
  return(result);
}

proc Mat4
mat4_world_from_screen(V2 world_origin_in_screen_space, R32 pixels_from_world_units)
{
  R32 world_units_from_pixels = 1.f / pixels_from_world_units;
  Mat4 result = mat4_screen_from_world(v2_lmul(-world_units_from_pixels,
					       world_origin_in_screen_space),
				       world_units_from_pixels);
  return(result);
}

proc Mat4
mat4_camera_transform(V3 cx, V3 cy, V3 cz, V3 cp)
{
  Mat4 result = mat4(v4_from_v3_xyz(cx, -v3_dot(cx, cp)),
		     v4_from_v3_xyz(cy, -v3_dot(cy, cp)),
		     v4_from_v3_xyz(cz, -v3_dot(cz, cp)),
		     v4(0, 0, 0, 1));
  return(result);
}

proc Mat4
mat4_camera_transform_inverse(V3 cx, V3 cy, V3 cz, V3 cp)
{
  Mat4 result = mat4(v4(cx.x, cy.x, cz.x, cp.x),
		     v4(cx.y, cy.y, cz.y, cp.y),
		     v4(cx.z, cy.z, cz.z, cp.z),
		     v4(   0,    0,    0,    1));
  return(result);
}

proc Mat3
mat3_mul(Mat3 m0, Mat3 m1)
{
  Mat3 result = {0};
  for(U32 i = 0; i < 3; ++i)
  {
    for(U32 j = 0; j < 3; ++j)
    {
      for(U32 k = 0; k < 3; ++k)
      {
	result.v[i][j] += m0.v[i][k] * m1.v[k][j];
      }
    }
  }
  return(result);
}

proc Mat4
mat4_mul(Mat4 m0, Mat4 m1)
{
  Mat4 result = {0};
  for(U32 i = 0; i < 4; ++i)
  {
    for(U32 j = 0; j < 4; ++j)
    {
      for(U32 k = 0; k < 4; ++k)
      {
	result.v[i][j] += m0.v[i][k] * m1.v[k][j];
      }
    }
  }
  return(result);
}

proc V4
mat4_transform(Mat4 m, V4 v)
{
  V4 result = v4(v4_dot(m.r0, v),
		 v4_dot(m.r1, v),
		 v4_dot(m.r2, v),
		 v4_dot(m.r3, v));
  return(result);
}
