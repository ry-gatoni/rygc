#include "base/base.h"
#include "file_formats/bmp.h"
#include "render/render.h"

#include "base/base.c"
#include "file_formats/bmp.c"
#include "render/render.c"

#define IMAGE_WIDTH  640
#define IMAGE_HEIGHT 480

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  if(!render_init()) return(1);

  Arena *arena = arena_alloc();
  LoadedBmp bmp = bmp_alloc(arena, IMAGE_WIDTH, IMAGE_HEIGHT);

  R_Handle framebuffer = render_create_framebuffer(IMAGE_WIDTH, IMAGE_HEIGHT);

  V3 camera_pos = v3(0, -10, 10);
  V3 camera_target = v3(0, 0, 0);
  V3 camera_dir = v3_normalized(v3_sub(camera_target, camera_pos));
  //R32 camera_fov = PI32 * 0.5f;
  V3 camera_z = v3_lmul(-1, camera_dir);
  V3 camera_x = v3_cross(v3(0, 1, 0), camera_z);
  V3 camera_y = v3_cross(camera_z, camera_x);
  Mat4 camera_transform = mat4_camera_transform(camera_x, camera_y, camera_z, camera_pos);



  return(0);
}
