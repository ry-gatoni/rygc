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
