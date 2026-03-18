#include "base/base.h"
#include "file_formats/bmp.h"
#include "render/render.h"

#include "base/base.c"
#include "file_formats/bmp.c"
#include "render/render.c"

#define IMAGE_WIDTH  640
#define IMAGE_HEIGHT 480

typedef enum RenderLevel
{
  RenderLevel_front,
  RenderLevel_grid,
  RenderLevel_background,
  RenderLevel_Count,
} RenderLevel;
#define RenderLevel(level) ((R32)RenderLevel_##level/(R32)RenderLevel_Count)

proc V3
unproject_pixel_to_world_z0(V2 pixel_pos, V2 image_center, R32 world_units_from_pixels,
			    V3 cx, V3 cy, V3 cz, V3 cp)
{
  R32 view_x = (pixel_pos.x - image_center.x) * world_units_from_pixels;
  R32 view_y = (pixel_pos.y - image_center.y) * world_units_from_pixels;

  V3 ray_d_view = v3(view_x, view_y, -1);
  V3 ray_d_world = v3_add(v3_add(v3_lmul(ray_d_view.x, cx),
				 v3_lmul(ray_d_view.y, cy)),
			  v3_lmul(ray_d_view.z, cz));
  if(fabsf(ray_d_world.z) < 1e-3) return v3(0, 0, 0);

  R32 t = -cp.z / ray_d_world.z;
  V3 result = v3_add(cp, v3_lmul(t, ray_d_world));
  return(result);
}

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  if(!render_init()) return(1);

  Arena *arena = arena_alloc();
  LoadedBmp bmp = bmp_alloc(arena, IMAGE_WIDTH, IMAGE_HEIGHT);

  V2S32 image_dim = v2s32(IMAGE_WIDTH, IMAGE_HEIGHT);
  R_Handle framebuffer = render_create_framebuffer(IMAGE_WIDTH, IMAGE_HEIGHT);

  Rect2 image_rect = rect2_min_dim(v2(0, 0), v2_from_v2s32(image_dim));
  V2 image_center = rect2_center(image_rect);

  R32 const pixels_from_world_units = 50.f;
  R32 const world_units_from_pixels = 1.f / pixels_from_world_units;
  Mat4 screen_from_world = mat4_screen_from_world(image_center, pixels_from_world_units);
  /* V2 const bottom_left_world = v2_lmul(-world_units_from_pixels, image_center); */
  /* V2 const top_right_world = v2_lmul(world_units_from_pixels, image_center); */
  /* V2 const image_dim_world = v2_sub(top_right_world, bottom_left_world); */

  V3 camera_pos = v3(0, -0.1, 1);
  V3 camera_target = v3(0, 0, 0);
  V3 camera_dir = v3_normalized(v3_sub(camera_target, camera_pos));
  //R32 camera_fov = PI32 * 0.5f;
  V3 camera_z = v3_lmul(-1, camera_dir);
  V3 camera_x = v3_normalized(v3_cross(v3(0, 1, 0), camera_z));
  V3 camera_y = v3_normalized(v3_cross(camera_z, camera_x));
  Mat4 camera_transform = mat4_camera_transform(camera_x, camera_y, camera_z, camera_pos);
  screen_from_world = mat4_mul(screen_from_world, camera_transform); // TODO: this is a weird order to compose these transforms in

  /* Mat4 world_from_screen = */
  /*   mat4_mul(mat4_camera_transform_inverse(camera_x, camera_y, camera_z, camera_pos), */
  /* 	     mat4_world_from_screen(image_center, pixels_from_world_units)); */
  /* Mat4 inverse_test = mat4_mul(world_from_screen, screen_from_world); */
  /* V2 bottom_left_world = v4_proj_xy(mat4_transform(world_from_screen, */
  /* 						   v4(0, 0, 0, 1))); */
  /* V2 bottom_right_world = v4_proj_xy(mat4_transform(world_from_screen, */
  /* 						    v4(image_dim.x, 0, 0, 1))); */
  /* V2 top_left_world = v4_proj_xy(mat4_transform(world_from_screen, */
  /* 						v4(0, image_dim.y, 0, 1))); */
  /* V2 top_right_world = v4_proj_xy(mat4_transform(world_from_screen, */
  /* 						 v4(image_dim.x, image_dim.y, 0, 1))); */

  V3 bottom_left_world =
    unproject_pixel_to_world_z0(v2(0, 0), image_center, world_units_from_pixels,
				camera_x, camera_y, camera_z, camera_pos);
  V3 bottom_right_world =
    unproject_pixel_to_world_z0(v2(IMAGE_WIDTH, 0), image_center, world_units_from_pixels,
				camera_x, camera_y, camera_z, camera_pos);
  V3 top_left_world =
    unproject_pixel_to_world_z0(v2(0, IMAGE_HEIGHT), image_center, world_units_from_pixels,
				camera_x, camera_y, camera_z, camera_pos);
  V3 top_right_world =
    unproject_pixel_to_world_z0(v2(IMAGE_WIDTH, IMAGE_HEIGHT), image_center, world_units_from_pixels,
				camera_x, camera_y, camera_z, camera_pos);
  V2 image_dim_world = v2(Max(bottom_right_world.x - bottom_left_world.x,
			      top_right_world.x - top_left_world.x),
			  Max(top_left_world.y - bottom_left_world.y,
			      top_right_world.y - bottom_right_world.y));
  R32 image_left_world = Min(bottom_left_world.x, top_left_world.x);
  R32 image_right_world = Max(bottom_right_world.x, top_right_world.x);
  R32 image_bottom_world = Min(bottom_left_world.y, bottom_right_world.y);
  R32 image_top_world = Min(top_left_world.y, top_right_world.y);

  //Mat4 screen_transform = mat4_screen_transform_ndc(v2s32(IMAGE_WIDTH, IMAGE_HEIGHT));
  render_set_viewport_dim(image_dim);
  render_set_world_transform(screen_from_world);
  render_equip_push_transform(R_Transform_screen_from_world);

  // NOTE: draw
  V4 const background_color = color_v4_from_rgba(0x08, 0x0C, 0x1C, 0xFF);
  render_set_clear_color(background_color);

  V4 const line_color = color_v4_from_rgba(0x80, 0x89, 0x89, 0xFF);
  V4 const axis_color = color_v4_from_rgba(0xFF, 0x7F, 0x00, 0xFF);
  R32 const line_thickness_px = 4;
  R32 const line_thickness = world_units_from_pixels * line_thickness_px;
  for(R32 x = floorf(image_left_world) + 1; x < image_right_world; x += 1.f)
  {
    Rect2 vert_line = rect2_min_dim(v2(x, image_bottom_world),
				    v2(line_thickness, image_dim_world.y));
    render_rect(vert_line, 0, RenderLevel(grid), fabsf(x) < 1e-3 ? axis_color : line_color);
  }
  for(R32 y = floorf(image_bottom_world) + 1; y < image_top_world; y += 1.f)
  {
    Rect2 horz_line = rect2_min_dim(v2(image_left_world, y),
				    v2(image_dim_world.x, line_thickness));
    render_rect(horz_line, 0, RenderLevel(grid), fabsf(y) < 1e-3 ? axis_color : line_color);
  }

  render_flush_commands();

  // NOTE: send to texture
  render_read_framebuffer_pixels(framebuffer, v2s32(0, 0), v2s32(IMAGE_WIDTH, IMAGE_HEIGHT), bmp.pixels);

  bmp_write(bmp, Str8Lit(DATA_DIR"/test/render_to_texture.bmp"));

  return(0);
}
