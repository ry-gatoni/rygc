/** TODO:
 * - sort out the various coordinate systems/projections (world, screen,
 *   perspective, orthographic), how the transforms are created and how usage
 *   code specifies which (combination of) transform(s) to use for a particular
 *   push call.
 * - clean up font integration: allocation, usage code, how we identify textures
 *   when rendering from commands.
 */

typedef struct R_Handle
{
  void *handle;
} R_Handle;

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

typedef enum R_TransformKind
{
  R_Transform_device_from_screen,
  R_Transform_screen_from_world,
  R_Transform_Count,
} R_TransformKind;

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

  Os_Handle window;

  V2S32 viewport_dim;

  V4 clear_color;

  R_Texture white_texture;

  R_TransformKind active_transform;
  union {
    struct {
      Mat4 transform_device_from_screen;
      Mat4 transform_screen_from_world;
    };
    Mat4 transforms[R_Transform_Count];
  };
  union {
    struct {
      R_BatchList screen_space_batches;
      R_BatchList world_space_batches;
    };
    R_BatchList batch_lists[R_Transform_Count];
  };

  R_Batch *batch_freelist;

  R_Handle renderer;
} R_Commands;

global R_Commands *render_commands = 0;

// -----------------------------------------------------------------------------
// initializers

proc B32 render_init(void);
proc R_Handle render_backend_init(Arena *arena);

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

proc void render_equip_window(Os_Handle window);

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
proc void render_set_world_transform(Mat4 mat);
proc void render_equip_push_transform(R_TransformKind transform);

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
