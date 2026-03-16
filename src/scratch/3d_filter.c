#include "base/base.h"
#include "gfx/gfx.h"
#include "render/render.h"
/* #include "font/font.h" */
/* #include "OpenGL/ogl.h" */

// TODO: sort out dependencies. It should be possible to open a window without
// providing a renderer or graphics api. It should be possible to use a renderer
// without providing a font loader.

#include "base/base.c"
#include "gfx/gfx.c"
#include "render/render.c"
/* #include "font/font.c" */
/* #include "OpenGL/ogl.c" */

#if 0
// NOTE: shitty float from string parser
proc R64
r64_from_str8(String8 str)
{
  R64 result = 0.0;
  B32 parsing_num = 1;
  U32 dec_places = 0;

  U8 *at = str.string;
  U8 *opl = at + str.count;
  for(; at < opl; ++at)
  {
    if(*at == '-' && at == str.string)
    {
      result *= -1;
    }

    if(*at == '.')
    {
      parsing_num = 0;
      continue;
    }

    if(!is_number(*at))
    {
      break;
    }

    if(parsing_num)
    {
      result *= 10.0;
      result += (R64)(*at - '0');
    }
    else
    {
      ++dec_places;
      R64 dec_inc = (R64)(*at - '0');
      for(U32 dec_idx = 0; dec_idx < dec_places; ++dec_idx)
      {
	dec_inc *= 0.1;
      }
      result += dec_inc;
    }
  }

  return(result);
}
#endif

// -----------------------------------------------------------------------------
// filter stuff

typedef struct Filter
{
  U32 zero_order;
  U32 pole_order;
  R64 *zero_coeffs;
  R64 *pole_coeffs;
} Filter;

typedef struct FilterTermLoose FilterTermLoose;
struct FilterTermLoose
{
  FilterTermLoose *next;
  R64 coeff;
  S64 power;
};

typedef struct LooseFilterTermList
{
  FilterTermLoose *first;
  FilterTermLoose *last;
  U64 term_count;
  S64 max_power;
  S64 min_power;
} LooseFilterTermList;

proc LooseFilterTermList
filter_parse_string(Arena *arena, String8 poly)
{
  LooseFilterTermList list = {0};

  ArenaTemp scratch = arena_get_scratch(&arena, 1);

  Buffer pm_idxs = str8_indices_of_chars_in_string(scratch.arena, poly, Str8Lit("+-"));
  U64 *inc_ptr = buf_consume_struct(&pm_idxs, U64);
  U8 *start = poly.string;
  U8 *end = start + poly.count;
  U8 *opl = inc_ptr ? start + *inc_ptr : end;
  for(; start < end; )
  {
    if(start == opl)
    {
      // NOTE: should only happen if we start with a plus or minus sign. expand the range
      Assert(*start == '+' || *start == '-');
      opl += *buf_consume_struct(&pm_idxs, U64);
      Assert(start != opl);
    }

    if(*(opl - 1) == '^')
    {
      // NOTE: hit a sign in an exponent. expand the range
      opl += *buf_consume_struct(&pm_idxs, U64);
    }

    // NOTE: parse the term of the form ((\+|\-)?%d)?(z(^(\+|\-)?%d)?)? (i suck at regexp btw)
    Buffer term = buffer_from_range(start, opl);
    R64 coeff = 0.0;
    if(buf_peek_char(term, '+') || buf_peek_char(term, '-'))
    {
      B32 coeff_is_neg = '-' == *buf_consume_struct(&term, U8);
      if(buf_peek_char(term, 'z'))
      {
	coeff = coeff_is_neg ? -1.0 : 1.0;
      }
      else
      {
	String8 term_str = str8_from_buf(term);
	term_str = str8_parse_r64(term_str, &coeff);
	if(coeff_is_neg) coeff *= -1.0;
	term = buf_from_str8(term_str);
      }
    }
    else
    {
      String8 term_str = str8_from_buf(term);
      term_str = str8_parse_r64(term_str, &coeff);
      term = buf_from_str8(term_str);
    }

    // NOTE: parse the power
    S64 power = 0;
    if(buf_peek_char(term, 'z'))
    {
      buf_consume_struct(&term, U8);
      if(buf_peek_char(term, '^'))
      {
	buf_consume_struct(&term, U8);
	String8 term_str = str8_from_buf(term);
	term_str = str8_parse_s64(term_str, &power);
	term = buf_from_str8(term_str);
      }
      else
      {
	power = 1;
      }
    }

    // NOTE: new term
    FilterTermLoose *loose_term = arena_push_struct(arena, FilterTermLoose);
    loose_term->coeff = coeff;
    loose_term->power = power;
    SLLQueuePush(list.first, list.last, loose_term);
    ++list.term_count;
    list.max_power = Max(list.max_power, power);
    list.min_power = Min(list.min_power, power);

    // NOTE: increment
    inc_ptr = buf_consume_struct(&pm_idxs, U64);
    start = opl;
    opl = inc_ptr ? start + *inc_ptr : end;
  }

  arena_release_scratch(scratch);

  return(list);
}

proc Filter
filter_from_strings(Arena *arena, String8 zeros_poly, String8 poles_poly)
{
  ArenaTemp scratch = arena_get_scratch(&arena, 1);

  // NOTE: parse zeros
  zeros_poly = str8_strip_whitespace(scratch.arena, zeros_poly);
  LooseFilterTermList loose_zero_terms = filter_parse_string(scratch.arena, zeros_poly);

  // NOTE: parse poles
  poles_poly = str8_strip_whitespace(scratch.arena, poles_poly);
  LooseFilterTermList loose_pole_terms = filter_parse_string(scratch.arena, poles_poly);

  // NOTE: bake
  U64 zero_order = 0;
  U64 pole_order = 0;

  S64 min_power = Min(loose_zero_terms.min_power, loose_pole_terms.min_power);
  if(min_power < 0)
  {
    zero_order = loose_zero_terms.max_power - min_power;
    pole_order = loose_pole_terms.max_power - min_power;
  }
  else
  {
    zero_order = loose_zero_terms.max_power;
    pole_order = loose_pole_terms.max_power;
  }
  S64 zero_power_inc = zero_order - loose_zero_terms.max_power;
  S64 pole_power_inc = pole_order - loose_pole_terms.max_power;

  R64 *zero_coeffs = arena_push_array(arena, R64, zero_order + 1);
  for(FilterTermLoose *term = loose_zero_terms.first; term; term = term->next)
  {
    R64 coeff = term->coeff;
    U64 power = term->power + zero_power_inc;
    zero_coeffs[zero_order - power] = coeff;
  }

  R64 *pole_coeffs = arena_push_array(arena, R64, pole_order + 1);
  for(FilterTermLoose *term = loose_pole_terms.first; term; term = term->next)
  {
    R64 coeff = term->coeff;
    U64 power = term->power + pole_power_inc;
    pole_coeffs[pole_order - power] = coeff;
  }

  arena_release_scratch(scratch);

  Filter result = {0};
  result.zero_order = zero_order;
  result.pole_order = pole_order;
  result.zero_coeffs = zero_coeffs;
  result.pole_coeffs = pole_coeffs;
  return(result);
}

// -----------------------------------------------------------------------------
// rendering

typedef enum RenderLevel
{
  RenderLevel_top,
  RenderLevel_camera,
  RenderLevel_grid_line,
  RenderLevel_background,
  RenderLevel_Count,
} RenderLevel;
#define RenderLevel(level) ((R32)(RenderLevel_##level)/(R32)(RenderLevel_Count))

#define GRID_COLS 20
#define GRID_ROWS 10

// -----------------------------------------------------------------------------
// "game" state

typedef enum Direction2D
{
  Direction2D_up,
  Direction2D_left,
  Direction2D_down,
  Direction2D_right,
  Direction2D_Count,
} Direction2D;

typedef struct Camera
{
  V2 pos; // 2d position for now
  R32 rot; // orientation. [-pi, pi)
  R32 zoom;
  R32 fov;
} Camera;

proc void
move_camera(Camera *c, Direction2D dir, R32 adv, R32 rot_adv)
{
  if(dir == Direction2D_left || dir == Direction2D_right)
  {
    // NOTE: rotate left and right
    c->rot += dir == Direction2D_right ? rot_adv : -rot_adv;
    if(c->rot >= PI32) c->rot -= TAU32;
    if(c->rot < -PI32) c->rot += TAU32;
  }
  else if(dir == Direction2D_up || dir == Direction2D_down)
  {
    // NOTE: move forwards and backwards
    R32 ang = dir == Direction2D_up ? c->rot : c->rot + PI32;
    R32 adv_x = adv * cosf(ang);
    R32 adv_y = adv * sinf(ang);
    c->pos.x += adv_x;
    c->pos.y += adv_y;
  }
}

#define SQUARES_PER_ROW 20

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  if(!gfx_init()) return 1;
  if(!render_init()) return 1;

  Os_Handle window = gfx_window_open(Str8Lit("3d filter"), 640, 480);
  render_equip_window(window);

  Arena *permanent_arena = arena_alloc();
  String8 zeros_string = Str8Lit("1 - z^2");
  String8 poles_string = Str8Lit("z^2");
  Filter filter = filter_from_strings(permanent_arena, zeros_string, poles_string);
  Unused(filter);

  Camera camera = {0};
  camera.pos = v2(0, 0);
  camera.rot = 0.7 * PI32;
  camera.zoom = 1;
  camera.fov = 0.5 * PI32;

  Arena *frame_arena = arena_alloc();
  B32 running = 1;
  while(running)
  {
    Os_EventList events = gfx_events(frame_arena);
    for(Os_Event *event = events.first; event; event = event->next)
    {
      if(event->kind == Os_EventKind_close)
      {
	running = 0;
	break;
      }

      if(event->kind == Os_EventKind_press)
      {
	R32 const lin_speed = 0.1f;
	R32 const ang_speed = lin_speed * TAU32 * 0.1f;
	switch(event->key)
	{
	  case Os_Key_left:
	  {
	    camera.rot += ang_speed;
	  }break;
	  case Os_Key_right:
	  {
	    camera.rot -= ang_speed;
	  }break;
	  case Os_Key_up:
	  {
	    V2 direction = v2_lmul(lin_speed, v2(cosf(camera.rot), sinf(camera.rot)));
	    v2_inc(&camera.pos, direction);
	  }break;
	  case Os_Key_down:
	  {
	    V2 direction = v2_lmul(-lin_speed, v2(cosf(camera.rot), sinf(camera.rot)));
	    v2_inc(&camera.pos, direction);
	  }break;
	  default: {}break;
	}
      }
    }

    V2S32 screen_dim = gfx_window_get_dim(window);
    Rect2 screen_rect = rect2(v2(0, 0), v2_from_v2s32(screen_dim));
    V2 screen_center = rect2_center(screen_rect);

    // NOTE: cells are 1 world unit by 1 world unit.
    R32 pixels_from_world_units = (R32)screen_dim.width / (R32)SQUARES_PER_ROW;
    R32 world_units_from_pixels = 1.f / pixels_from_world_units;
    Mat4 screen_from_world = mat4_screen_from_world(screen_center, pixels_from_world_units);
    V2 const bottom_left_world = v2_lmul(-world_units_from_pixels, screen_center);
    V2 const top_right_world = v2_lmul(world_units_from_pixels, screen_center);

    render_begin_frame();
    //gfx_window_begin_frame(window);
    render_set_world_transform(screen_from_world);

    render_equip_push_transform(R_Transform_device_from_screen);

    V4 const background_color = color_v4_from_rgba(0x08, 0x0C, 0x1C, 0xFF);
    render_rect(screen_rect, 0, RenderLevel(background), background_color);

    R32 const line_thickness_px = 4;
    R32 const line_thickness = world_units_from_pixels * line_thickness_px;

    render_equip_push_transform(R_Transform_screen_from_world);

    // NOTE: grid
    V4 const grid_line_color = color_v4_from_rgba(0x8b, 0x89, 0x89, 0xFF);
    R32 const screen_height_world_units = world_units_from_pixels * screen_dim.height;
    R32 const screen_width_world_units = world_units_from_pixels * screen_dim.width;
    for(R32 col = floorf(bottom_left_world.x); col < top_right_world.x; col += 1.f)
    {
      Rect2 vert_line = rect2_min_dim(v2(col, bottom_left_world.y),
				      v2(line_thickness, screen_height_world_units));
      render_rect(vert_line, 0, RenderLevel(grid_line), grid_line_color);
    }
    for(R32 row = floorf(bottom_left_world.y); row < top_right_world.y; row += 1.f)
    {
      Rect2 horz_line = rect2_min_dim(v2(bottom_left_world.x, row),
				      v2(screen_width_world_units, line_thickness));
      render_rect(horz_line, 0, RenderLevel(grid_line), grid_line_color);
    }

    // NOTE: camera
    //render_equip_push_transform(R_Transform_screen_from_world);
    V4 const camera_rect_color = color_v4_from_rgba(0xFF, 0xFF, 0x00, 0xFF);
    V2 const camera_rect_dim_px = v2(10, 10);
    V2 const camera_rect_dim = v2_lmul(world_units_from_pixels, camera_rect_dim_px);
    Rect2 camera_rect = rect2_center_dim(camera.pos, camera_rect_dim);
    render_rect(camera_rect, 0, RenderLevel(camera), camera_rect_color);

    V4 const direction_vector_color = color_v4_from_rgba(0x00, 0x00, 0xFF, 0xFF);
    R32 direction_vector_length = camera.zoom;
    V2 camera_direction = v2_lmul(direction_vector_length, v2(cosf(camera.rot), sinf(camera.rot)));
    V2 plane_center = v2_add(camera.pos, camera_direction);
    render_line_segment(camera.pos, plane_center, line_thickness, RenderLevel(camera), direction_vector_color);

    V4 const plane_color = color_v4_from_rgba(0xFF, 0x00, 0xFF, 0xFF);
    V2 plane_v =  v2_lmul(direction_vector_length / tanf(0.5f * camera.fov), v2(-camera_direction.y, camera_direction.x));
    V2 plane_0 = v2_sub(plane_center, plane_v);
    V2 plane_1 = v2_add(plane_center, plane_v);
    render_line_segment(plane_0, plane_1, line_thickness, RenderLevel(camera), plane_color);

    V4 const cone_color = color_v4_from_rgba(0xFF, 0x00, 0x00, 0xFF);
    render_line_segment(camera.pos, plane_0, line_thickness, RenderLevel(camera), cone_color);
    render_line_segment(camera.pos, plane_1, line_thickness, RenderLevel(camera), cone_color);

    // NOTE: cast rays
    V4 const ray_pos_color = color_v4_from_rgba(0xFF, 0x7F, 0x00, 0xFF);
    V2 const ray_pos_rect_dim = camera_rect_dim;

    R32 const step_x = plane_center.x > camera.pos.x ? 1.f : -1.f;
    R32 const step_y = plane_center.y > camera.pos.y ? 1.f : -1.f;
    V2 ray_pos = camera.pos;
    for(;;)
    {
      R32 ray_pos_next_x = (S32)(ray_pos.x + step_x);
      R32 ray_pos_next_y = (S32)(ray_pos.y + step_y);

      R32 ray_pos_next_x_derived_y =
	(camera_direction.y / camera_direction.x) * (ray_pos_next_x - plane_center.x) + plane_center.y;
      R32 ray_pos_next_y_derived_x =
	(camera_direction.x / camera_direction.y) * (ray_pos_next_y - plane_center.y) + plane_center.x;

      V2 ray_pos_next_x_v = v2(ray_pos_next_x, ray_pos_next_x_derived_y);
      V2 ray_pos_next_y_v = v2(ray_pos_next_y_derived_x, ray_pos_next_y);

      V2 ray_pos_next_x_diff = v2_sub(ray_pos_next_x_v, ray_pos);
      V2 ray_pos_next_y_diff = v2_sub(ray_pos_next_y_v, ray_pos);
      R32 ray_pos_next_x_diff_mag_sq =
	ray_pos_next_x_diff.x*ray_pos_next_x_diff.x + ray_pos_next_x_diff.y*ray_pos_next_x_diff.y;
      R32 ray_pos_next_y_diff_mag_sq =
	ray_pos_next_y_diff.x*ray_pos_next_y_diff.x + ray_pos_next_y_diff.y*ray_pos_next_y_diff.y;

      if(ray_pos_next_x_diff_mag_sq < ray_pos_next_y_diff_mag_sq)
      {
	ray_pos = ray_pos_next_x_v;
      }
      else
      {
	ray_pos = ray_pos_next_y_v;
      }

      Rect2 ray_pos_rect = rect2_center_dim(ray_pos, ray_pos_rect_dim);
      render_rect(ray_pos_rect, 0, RenderLevel(camera), ray_pos_color);

      if(ray_pos.x < bottom_left_world.x || ray_pos.x > top_right_world.x ||
	 ray_pos.y < bottom_left_world.y || ray_pos.y > top_right_world.y)
      {
	break;
      }
    }
    render_line_segment(camera.pos, ray_pos, line_thickness, RenderLevel(camera), ray_pos_color);

    render_end_frame();
    //gfx_window_end_frame(window);
    arena_clear(frame_arena);
  }

  gfx_window_close(window);
  gfx_uninit();

  return(0);
}
