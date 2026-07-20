// TODO:
// - work out intersections with graphics and opengl layers
//   - who selects backend, window or render?
// - sort out the various coordinate systems/projections (world, screen,
//   perspective, orthographic), how the transforms are created and how usage
//   code specifies which (combination of) transform(s) to use for a particular
//   push call.
// - store inverses of all transforms so we can unproject when doing text
//   rendering in a transformed coordinate system
// - clean up font integration: allocation, usage code, how we identify textures
//   when rendering from commands.

// NOTE:
// - coordinate systems:
//   - "world space" is whatever coordinates are used when pushing quads
//   - "view space" is the image of world space under the view transform
//   - "clip space" is the image of view space under the projection transform ([-1, 1]^3)
//   - "screen space" is the image of clip space under the viewport transform (implicit)
//   - "unprojecting" means taking a point in screen space and applying the
//     inverse of the above transforms to produce a point in world space

typedef struct R_Handle
{
  void *handle;
} R_Handle;

typedef enum R_Backend
{
  R_Backend_software,
  R_Backend_opengl,
  R_Backend_metal,
  R_Backend_d3d11,
  R_Backend_vulkan,
  R_Backend_Count,
} R_Backend;

typedef enum R_PixelFormat
{
  R_PixelFormat_rgba,
  R_PixelFormat_abgr,
  R_PixelFormat_red,

  R_PixelFormat_Count,
} R_PixelFormat;

typedef struct R_Quad
{
  V2 p_min;
  V2 p_max;

  V2 uv_min;
  V2 uv_max;

  U32 color;

  R32 angle;
  // TODO: stop doing sorting via the z buffer. it's not a great idea
  R32 level;

  U16 proj_transform_idx;
  U16 view_transform_idx;
} R_Quad;

typedef struct R_TextureCreateParams
{
  R_PixelFormat pixel_fmt;
  R_PixelFormat internal_fmt;
  B32 wrap_x;
  B32 wrap_y;
} R_TextureCreateParams;

typedef struct R_Texture
{
  R_Handle handle;
  V2S32 dim;
  void *pixels;
} R_Texture;

/* typedef enum R_TransformKind */
/* { */
/*   R_Transform_device_from_screen, */
/*   R_Transform_screen_from_world, */
/*   R_Transform_Count, */
/* } R_TransformKind; */

typedef enum R_ProjTransformKind
{
  R_ProjTransformKind_identity,
  R_ProjTransformKind_ndc_from_pixels,
  R_ProjTransformKind_orthographic,
  R_ProjTransformKind_perspective,

  R_ProjTransformKind_Count,
} R_ProjTransformKind;

typedef struct R_ProjTransformId
{
  U16 idx;
} R_ProjTransformId;

typedef struct R_ViewTransformId
{
  U16 idx;
} R_ViewTransformId;

typedef struct R_TransformId
{
  R_ViewTransformId view_id;
  R_ProjTransformId proj_id;
} R_TransformId;

#define R_MAX_TRANSFORM_COUNT 64
typedef struct R_Transforms
{
  Mat4 forward[R_MAX_TRANSFORM_COUNT];
  Mat4 inverse[R_MAX_TRANSFORM_COUNT];
} R_Transforms;

typedef struct R_OrthoParams
{
  V2 dim;
  R32 near_clip_plane;
  R32 far_clip_plane;
} R_OrthoParams;
#define R_ORTHO_PARAMS_DEFAULT\
  .dim = { .x = 2, .y = 2, }, .near_clip_plane = 1, .far_clip_plane = -1,

typedef struct R_PerspectiveParams
{
  R32 fov;
  R32 aspect_ratio;
  R32 near_clip_plane;
  R32 far_clip_plane;
} R_PerspectiveParams;
#define R_PERSPECTIVE_PARAMS_DEFAULT\
  .fov = PI32, .aspect_ratio = 1, .near_clip_plane = 1, .far_clip_plane = -1,

typedef struct R_ViewParams
{
  V3 cx;
  V3 cy;
  V3 cz;
  V3 cp;
} R_ViewParams;

typedef struct R_Batch R_Batch;
struct R_Batch
{
  R_Batch *next;

  U32 quad_cap;
  U32 quad_count;
  R_Quad *quads;

  R_Texture *texture;
};

#define RENDER_BATCH_QUAD_CAP KB(4)

typedef struct R_BatchList
{
  R_Batch *first_batch;
  R_Batch *last_batch;
  U32 batch_count;
  U32 total_quad_count;
} R_BatchList;

typedef struct R_Commands
{
  Arena *arena;

  Gfx_Handle window;

  V2S32 viewport_dim;

  V4 clear_color;

  R_Texture white_texture;

  R_Backend backend;
  void *render_targets[R_Backend_Count];

  R_TransformId active_transform;
  U32 used_transform_idx;
  R_Transforms transforms;

  /* R_TransformKind active_transform; */
  /* union { */
  /*   struct { */
  /*     Mat4 transform_device_from_screen; */
  /*     Mat4 transform_screen_from_world; */
  /*   }; */
  /*   Mat4 transforms[R_Transform_Count]; */
  /* }; */
  /* union { */
  /*   struct { */
  /*     R_BatchList screen_space_batches; */
  /*     R_BatchList world_space_batches; */
  /*   }; */
  /*   R_BatchList batch_lists[R_Transform_Count]; */
  /* }; */

  R_BatchList batch_list;
  R_Batch *batch_freelist;

  R_Handle renderer;
} R_Commands;

global R_Commands *render_commands = 0;

// -----------------------------------------------------------------------------
// initializers

proc B32 render_init(void);
proc R_Handle render_backend_init(Arena *arena);

proc void render_set_backend(R_Backend backend);

#if defined(FONT_LAYER)
// -----------------------------------------------------------------------------
// font

typedef struct R_Font
{
  PackedFont *font;
  R_Texture atlas;
} R_Font;

proc R_Font render_alloc_font(PackedFont *font);
proc void render_string(R_Font *font, String8 string, V2 pos, R32 level, V4 color);

#endif

proc void render_flush_commands(void);

#if defined(GFX_LAYER)
// -----------------------------------------------------------------------------
// windowing

proc void render_equip_window(Gfx_Handle window);

proc void render_begin_frame(void);
proc void render_end_frame(void);

#endif
// -----------------------------------------------------------------------------
// textures

#define render_create_texture(width, height, pixels, ...)\
  render_create_texture_ex(width, height, pixels, &(R_TextureCreateParams){.pixel_fmt = R_PixelFormat_rgba, .internal_fmt = R_PixelFormat_rgba, __VA_ARGS__})
proc R_Texture render_create_texture_ex(S32 width, S32 height, void *pixels, R_TextureCreateParams *params);
proc void render_update_texture(R_Texture *texture, S32 pos_x, S32 pos_y, S32 width, S32 height, R_PixelFormat format, void *pixels);

proc R_Handle render_create_framebuffer(S32 width, S32 height);
proc void render_read_framebuffer_pixels(R_Handle framebuffer, V2S32 pos, V2S32 dim, U32 *pixels);

// -----------------------------------------------------------------------------
// transforms

proc void render_set_viewport_dim(V2S32 dim);

/* proc R_ProjTransformId render_orthographic_projection(void); */
/* proc R_ProjTransformId render_perspective_projection(void); */

proc R_ProjTransformId render_projection(R_ProjTransformKind kind);
proc R_ViewTransformId render_default_view(void);
proc R_ViewTransformId render_set_view(V3 camera_x, V3 camera_y, V3 camera_z, V3 camera_pos);

/* proc R_TransformId render_use_projection(R_ProjTransformId proj_id); */
/* proc R_TransformId render_use_view(R_ViewTransformId view_id); */
proc R_TransformId render_make_and_use_transform(R_ViewTransformId view_id, R_ProjTransformId proj_id);
proc void render_use_transform(R_TransformId id);

proc V3 render_unproject(V2 screen_point, R_TransformId transform);

//proc void render_set_world_transform(Mat4 mat);
//proc void render_equip_push_transform(R_TransformKind transform);

#define render_transform_pair_id(f, i) render_transform_pair_id_ex(f, i)
#define render_transform_pair_ortho(f, i, ...) render_transform_pair_ortho_ex(f, i, &(R_OrthoParams){ R_ORTHO_PARAMS_DEFAULT __VA_ARGS__ })
#define render_transform_pair_perspective(f, i, ...) render_transform_pair_perspective_ex(f, i, &(R_PerspectiveParams){ R_PERSPECTIVE_PARAMS_DEFAULT __VA_ARGS__ })
proc void render_transform_pair_view(Mat4 *forward, Mat4 *inverse, R_ViewParams *params);
proc void render_transform_pair_id_ex(Mat4 *forward, Mat4 *inverse);
proc void render_transform_pair_ortho_ex(Mat4 *forward, Mat4 *inverse, R_OrthoParams *params);
proc void render_transform_pair_perspective_ex(Mat4 *forward, Mat4 *inverse, R_PerspectiveParams *params);

// -----------------------------------------------------------------------------
// drawing

proc void render_set_clear_color(V4 color);

// TODO: separate drawing layer?
proc void render_texture(R_Texture *texture, Rect2 rect, Rect2 uv, R32 angle, R32 level, V4 color);
proc void render_rect(Rect2 rect, R32 angle, R32 level, V4 color);
proc void render_line_segment(V2 p0, V2 p1, R32 thickness, R32 level, V4 color);

// -----------------------------------------------------------------------------
// batch helpers

proc R_Batch* render_batch_alloc(B32 push_front);
proc R_Batch* render_batch_for_texture(R_Texture *texture);
