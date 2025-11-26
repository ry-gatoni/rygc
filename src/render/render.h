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

typedef struct R_Texture
{
  R_Handle handle;
  V2S32 dim;
} R_Texture;

typedef struct R_Batch R_Batch;
struct R_Batch
{
  R_Batch *next;

  U32 quad_cap;
  U32 quad_count;
  R_Quad *quads;

  R_Texture *texture;
};

typedef struct R_Commands
{
  Arena *arena;

  Os_Handle window;

  V2S32 window_dim;
  Mat4 transform;

  R_Batch *first_batch;
  R_Batch *last_batch;
  U32 batch_count;
  U32 total_quad_count;

  R_Texture white_texture;

  R_Batch *batch_freelist;

  R_Handle renderer;
} R_Commands;

global R_Commands *render_commands = 0;

// -----------------------------------------------------------------------------
// initializers

proc B32 render_init(void);
proc R_Handle render_backend_init(Arena *arena);

// -----------------------------------------------------------------------------
// font

// TODO: these forward declarations are kinda weird. Maybe include font before
//       render, and only have to forward declare the atlas texture
struct LooseFont;
typedef struct PackedFont R_Font;
proc R_Font* render_alloc_font(struct LooseFont *loose_font);

// -----------------------------------------------------------------------------
// windowing

proc void render_equip_window(Os_Handle window);

proc void render_begin_frame(void);
proc void render_end_frame(void);

// -----------------------------------------------------------------------------
// textures

proc R_Texture render_create_texture(S32 width, S32 height, R_PixelFormat internal_fmt, R_PixelFormat pixel_fmt, void *pixels);
proc void render_update_texture(R_Texture *texture, S32 pos_x, S32 pos_y, S32 width, S32 height, R_PixelFormat format, void *pixels);
proc R_Batch* render_batch_for_texture(R_Texture *texture);

// -----------------------------------------------------------------------------
// drawing

// TODO: separate drawing layer?
proc void render_texture(R_Texture *texture, Rect2 rect, Rect2 uv, R32 angle, R32 level, V4 color);
proc void render_rect(Rect2 rect, R32 angle, R32 level, V4 color);
proc void render_string(R_Font *font, String8 string, V2 pos, R32 level, V4 color);
proc void render_line_segment(V2 p0, V2 p1, R32 thickness, R32 level, V4 color);
