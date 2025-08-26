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

typedef struct R_Vertex
{
  V3 pos;
  V2 uv;
  U32 color;
} R_Vertex;

typedef struct R_Texture
{
  R_Handle handle;
  V2S32 dim;
} R_Texture;

typedef struct R_Batch R_Batch;
struct R_Batch
{
  R_Batch *next;

  U32 vertex_cap;
  U32 vertex_count;
  R_Vertex *vertex_buffer;
  
  R_Texture *texture;
};

typedef struct R_Commands
{
  Arena *arena;

  Os_Handle window;

  V2S32 window_dim;
  V2 ndc_scale;
  Mat4 transform;

  R_Batch *first_batch;
  R_Batch *last_batch;
  U32 batch_count;

  R_Texture white_texture;

  R_Batch *batch_freelist;

  R_Handle renderer;
} R_Commands;

global R_Commands *render_commands = 0;

// NOTE: interface
proc B32 render_init(void);
proc R_Handle render_backend_init(Arena *arena);

// TODO: these forward declarations are kinda weird. Maybe include font before
//       render, and only have to forward declare the atlas texture
struct LooseFont;
typedef struct PackedFont R_Font;
proc R_Font* render_alloc_font(struct LooseFont *loose_font);

proc void render_equip_window(Os_Handle window);

proc void render_begin_frame(void);
proc void render_end_frame(void);

proc R_Texture render_create_texture(S32 width, S32 height, R_PixelFormat internal_fmt, R_PixelFormat pixel_fmt, void *pixels);
proc void render_update_texture(R_Texture *texture, S32 pos_x, S32 pos_y, S32 width, S32 height, R_PixelFormat format, void *pixels);

// TODO: separate drawing layer?
proc void render_rect(R_Texture *texture, Rect2 rect, Rect2 uv, R32 level, V4 color);
proc void render_string(R_Font *font, String8 string, V2 pos, R32 level, V4 color);
