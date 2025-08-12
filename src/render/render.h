typedef struct R_Vertex
{
  V3 pos;
  V2 uv;
  U32 color;
} R_Vertex;

typedef struct R_Texture
{
  GLuint handle;
  V2S32 dim;
} R_Texture;

typedef struct R_Font
{
  PackedFont *font;
  R_Texture *atlas;
} R_Font;

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

  V2S32 window_dim;
  V2 ndc_scale;

  R_Batch *first_batch;
  R_Batch *last_batch;
  U32 batch_count;

  R_Texture *white_texture;

  R_Batch *batch_freelist;

  GLuint vao;
  GLuint vbo;
  GLuint sampler;
  GLint sampler_loc;
  
  GlShader vert_shader;
  GlShader frag_shader;
  GlShader shader_prog;
} R_Commands;

proc R_Font* render_alloc_font(Arena *arena, PackedFont *font);
proc R_Commands* render_alloc_commands(Arena *arena);

proc void render_begin_frame(R_Commands *commands, S32 window_width, S32 window_height);
proc void render_rect(R_Commands *commands, R_Texture *texture, Rect2 rect, Rect2 uv, R32 level, V4 color);
proc void render_string(R_Commands *commands, R_Font *font, String8 string, V2 pos, R32 level, V4 color);
proc void render_from_commands(R_Commands *commands);
