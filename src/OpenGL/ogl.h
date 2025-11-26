#if OS_LINUX
// TODO: we should probably run-time load opengl on all platforms
#  define GL_GLEXT_PROTOTYPES
#  include "GL/gl.h"
#elif OS_WINDOWS
#  include "ogl_api_defs.h"
#else
#  error platform not implemented
#endif

global GLint ogl_fmts[] ={
  [R_PixelFormat_rgba] = GL_RGBA,
  [R_PixelFormat_abgr] = GL_BGRA,
  [R_PixelFormat_red] = GL_RED,
};

typedef struct Ogl_Shader
{
  GLuint handle;
  String8 log;
} Ogl_Shader;

typedef struct Ogl_Renderer
{
  GLuint vao;
  GLuint vbo;
  GLuint sampler;
  GLint sampler_loc;
  GLint transform_loc;

  Ogl_Shader vert_shader;
  Ogl_Shader frag_shader;
  Ogl_Shader shader_prog;
} Ogl_Renderer;

global Ogl_Renderer *ogl_renderer = 0;

global char vert_shader_src[] =
  "#version 330\n"

  "layout (location = 0) in vec4 pattern;"
  "layout (location = 1) in vec4 p_min_max;\n"
  "layout (location = 2) in vec4 uv_min_max;\n"
  "layout (location = 3) in vec4 color;\n"
  "layout (location = 4) in float angle;\n"
  "layout (location = 5) in float level;\n"

  "uniform mat4 transform;\n"

  "out vec2 f_uv;\n"
  "out vec4 f_color;\n"

  "void main() {\n"
  "  vec2 pattern_p = pattern.xy;\n"
  "  vec2 pattern_uv = pattern.zw;\n"

  "  vec2 quad_center = (p_min_max.zw + p_min_max.xy)*0.5;\n"
  "  vec2 quad_half_dim = (p_min_max.zw - p_min_max.xy)*0.5;\n"

  "  vec2 uv_min = uv_min_max.xy;\n"
  "  vec2 uv_dim = uv_min_max.zw - uv_min_max.xy;\n"

  "  float cosa = cos(angle);\n"
  "  float sina = sin(angle);\n"
  "  vec2 rot_pattern = vec2(cosa*pattern_p.x - sina*pattern_p.y, sina*pattern_p.x + cosa*pattern_p.y);\n"

  "  vec2 pos = quad_center + quad_half_dim*rot_pattern;\n"
  "  gl_Position = transform * vec4(pos, level, 1.0);\n"

  "  f_uv = uv_min + uv_dim*pattern_uv;\n"

  "  f_color = color;\n"
  "}\n";

global char frag_shader_src[] =
  "#version 330\n"

  "in vec2 f_uv;\n"
  "in vec4 f_color;\n"

  "uniform sampler2D atlas;\n"

  "out vec4 out_color;\n"

  "void main() {\n"
  "  vec4 sampled = texture(atlas, f_uv);\n"
  "  out_color = f_color * vec4(sampled.r, sampled.r, sampled.r, sampled.r);\n"
  "}\n";

global V4 quad_pattern[] = {
  // NOTE: format is v_pos.x, v_pos.y, v_uv.x, v_uv.y
  {-1.f, -1.f, 0.f, 0.f}, // bottom left
  { 1.f, -1.f, 1.f, 0.f}, // bottom right
  {-1.f,  1.f, 0.f, 1.f}, // top left
  { 1.f,  1.f, 1.f, 1.f}, // top right
};

proc B32 ogl_init(void);

proc Ogl_Shader ogl_make_shader(Arena *arena, char *src, GLenum kind);
proc Ogl_Shader ogl_make_program(Arena *arena, GLuint *shaders, U32 shader_count);

#if OS_WINDOWS
#  include "OpenGl/windows/ogl_win32.h"
#endif
