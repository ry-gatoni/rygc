#define PI32  (3.14159265359f)
#define TAU32 (6.28318530718f)

// NOTE: vectors
typedef union V2
{
  struct {R32 x, y;};
  R32 v[2];
} V2;

typedef union V2S32
{
  struct {S32 x, y;};
  S32 v[2];
} V2S32;

typedef union V3
{
  struct {R32 x, y, z;};
  R32 v[3];
} V3;

typedef union V4
{
  struct {R32 x, y, z, w;};
  struct {R32 r, g, b, a;};
  R32 v[4];
} V4;

proc V2 v2(R32 x, R32 y);
proc V3 v3(R32 x, R32 y, R32 z);
proc V4 v4(R32 x, R32 y, R32 z, R32 w);

proc V2S32 v2s32(S32 x, S32 y);
proc V2 v2_from_v2s32(V2S32 v);

proc V2 v2_add(V2 v, V2 w);
proc V2 v2_sub(V2 v, V2 w);
proc void v2_inc(V2 *v, V2 w);
proc void v2_dec(V2 *v, V2 w);
proc V2 v2_lmul(R32 a, V2 v);
proc V2 v2_rmul(V2 v, R32 a);
proc void v2_scale(V2 *v, R32 a);
proc V2 v2_hadamard(V2 v, V2 w);

// NOTE: ranges & rects
typedef struct RangeU32
{
  U32 min, max;
} RangeU32;

typedef struct RangeS32
{
  S32 min, max;
} RangeS32;

typedef struct Rect2
{
  V2 min, max;
} Rect2;

#define EachRange_Ex(var, type, range, inc) (type var = range.min; var < range.max; var += inc)
#define EachRange(var, type, range) EachRange_Ex(var, type, range, 1)

proc RangeU32 rng_u32(U32 min, U32 max);
proc RangeS32 rng_s32(S32 min, S32 max);
proc RangeU32 rng_u32_invalid(void);
proc RangeS32 rng_s32_invalid(void);

proc U32 rng_u32_len(RangeU32 rng);
proc S32 rng_s32_len(RangeS32 rng);

proc Rect2 rect2(V2 min, V2 max);
proc Rect2 rect2_min_dim(V2 min, V2 dim);
proc Rect2 rect2_center_dim(V2 center, V2 dim);
proc Rect2 rect2_center_halfdim(V2 center, V2 halfdim);
