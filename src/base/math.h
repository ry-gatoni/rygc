#include <math.h> // TODO: remove

// -----------------------------------------------------------------------------
// scalar

#define PI32  (3.14159265359f)
#define TAU32 (6.28318530718f)

proc B32 is_pow_2(U32 num);
proc U32 bit_reverse_u32(U32 num);
proc U64 bit_reverse_u64(U64 num);
proc U32 rygc_log2(U32 num);
proc R32 rygc_sqrt(R32 num);
proc R32 rygc_cos(R32 num);
proc R32 rygc_sin(R32 num);
proc R32 rygc_atan2(R32 x, R32 y);
proc R32 lerp(R32 val0, R32 val1, R32 t);

// -----------------------------------------------------------------------------
// vectors

typedef union V2
{
  struct {R32 x, y;};
  R32 v[2];
} V2;

typedef union V2S32
{
  struct {S32 x, y;};
  struct {S32 width, height;};
  S32 v[2];
} V2S32;

typedef union V3
{
  struct {R32 x, y, z;};
  struct {R32 r, g, b;};
  R32 v[3];
} V3;

typedef union V4
{
  struct {R32 x, y, z, w;};
  struct {R32 r, g, b, a;};
  R32 v[4];
} V4;

// -----------------------------------------------------------------------------
// constructors

proc V2 v2(R32 x, R32 y);
proc V3 v3(R32 x, R32 y, R32 z);
proc V4 v4(R32 x, R32 y, R32 z, R32 w);

proc V2 v2_polar(R32 mag, R32 angle);

// -----------------------------------------------------------------------------
// casts

proc V2S32 v2s32(S32 x, S32 y);
proc V2 v2_from_v2s32(V2S32 v);

// -----------------------------------------------------------------------------
// binops

proc V2 v2_add(V2 v, V2 w);
proc V2 v2_sub(V2 v, V2 w);
proc void v2_inc(V2 *v, V2 w);
proc void v2_dec(V2 *v, V2 w);
proc V2 v2_lmul(R32 a, V2 v);
proc V2 v2_rmul(V2 v, R32 a);
proc void v2_scale(V2 *v, R32 a);
proc V2 v2_hadamard(V2 v, V2 w);

proc V3 v3_add(V3 v, V3 w);
proc V3 v3_sub(V3 v, V3 w);
proc void v3_inc(V3 *v, V3 w);
proc void v3_dec(V3 *v, V3 w);
proc V3 v3_lmul(R32 a, V3 v);
proc V3 v3_rmul(V3 v, R32 a);
proc void v3_scale(V3 *v, R32 a);
proc V3 v3_hadamard(V3 v, V3 w);

proc V4 v4_add(V4 v, V4 w);
proc V4 v4_sub(V4 v, V4 w);
proc void v4_inc(V4 *v, V4 w);
proc void v4_dec(V4 *v, V4 w);
proc V4 v4_lmul(R32 a, V4 v);
proc V4 v4_rmul(V4 v, R32 a);
proc void v4_scale(V4 *v, R32 a);
proc V4 v4_hadamard(V4 v, V4 w);

// -----------------------------------------------------------------------------
// scalar from vector

proc R32 v2_length(V2 v);
proc R32 v2_angle(V2 v);

// -----------------------------------------------------------------------------
// interpolation

proc V2 lerp_v2(V2 v, V2 w, R32 t);
proc V3 lerp_v3(V3 v, V3 w, R32 t);
proc V4 lerp_v4(V4 v, V4 w, R32 t);

// -----------------------------------------------------------------------------
// ranges

typedef struct RangeU32
{
  U32 min, max;
} RangeU32;

typedef struct RangeS32
{
  S32 min, max;
} RangeS32;

typedef struct RangeR32
{
  R32 min, max;
} RangeR32;

// -----------------------------------------------------------------------------
// iteration

#define EachRange_Ex(var, type, range, inc) (type var = range.min; var < range.max; var += inc)
#define EachRange(var, type, range) EachRange_Ex(var, type, range, 1)

// -----------------------------------------------------------------------------
// construction

proc RangeU32 range_u32(U32 min, U32 max);
proc RangeS32 range_s32(S32 min, S32 max);
proc RangeR32 range_r32(R32 min, R32 max);

proc RangeU32 range_u32_invalid(void);
proc RangeS32 range_s32_invalid(void);
proc RangeR32 range_r32_invalid(void);

// -----------------------------------------------------------------------------
// scalar from range

proc U32 range_u32_len(RangeU32 rng);
proc S32 range_s32_len(RangeS32 rng);
proc R32 range_r32_len(RangeR32 rng);

proc R32 range_r32_map_01(R32 val, RangeR32 rng);

// -----------------------------------------------------------------------------
// rects

typedef struct Rect2
{
  V2 min, max;
} Rect2;

// -----------------------------------------------------------------------------
// construction

proc Rect2 rect2(V2 min, V2 max);
proc Rect2 rect2_invalid(void);
proc Rect2 rect2_min_dim(V2 min, V2 dim);
proc Rect2 rect2_center_dim(V2 center, V2 dim);
proc Rect2 rect2_center_halfdim(V2 center, V2 halfdim);

// -----------------------------------------------------------------------------
// modification

proc Rect2 rect2_offset(Rect2 rect, V2 offset);

// -----------------------------------------------------------------------------
// vector from rect

proc V2 rect2_center(Rect2 rect);
proc V2 rect_dim(Rect2 rect);

// -----------------------------------------------------------------------------
// matrices

typedef union Mat3
{
  struct { V3 c0; V3 c1; V3 c2; };
  R32 v[3][3];
} Mat3;

typedef union Mat4
{
  struct { V4 c0; V4 c1; V4 c2; V4 c3; };
  R32 v[4][4];
} Mat4;

// -----------------------------------------------------------------------------
// construction

proc Mat3 mat3(V3 c0, V3 c1, V3 c2);
proc Mat3 mat3_id(void);

proc Mat4 mat4(V4 c0, V4 c1, V4 c2, V4 c3);
proc Mat4 mat4_id(void);
proc Mat4 mat4_yflip(void);
proc Mat4 mat4_screen_transform_ndc(V2S32 dim);
