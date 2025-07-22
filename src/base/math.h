typedef union V2
{
  struct {R32 x, y;};
  R32 v[2];
} V2;

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

typedef struct RangeU32
{
  U32 min, max;
} RangeU32;

typedef struct RangeS32
{
  S32 min, max;
} RangeS32;

#define EachRange_Ex(var, type, range, inc) (type var = range.min; var < range.max; var += inc)
#define EachRange(var, type, range) EachRange_Ex(var, type, range, 1)

proc RangeU32 make_rng_u32_invalid(void);
proc RangeS32 make_rng_s32_invalid(void);

proc U32 rng_u32_len(RangeU32 rng);
proc S32 rng_s32_len(RangeS32 rng);

#define PI32  (3.14159265359f)
#define TAU32 (6.28318530718f)
