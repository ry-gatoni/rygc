#if OS_LINUX
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
  
  Ogl_Shader vert_shader;
  Ogl_Shader frag_shader;
  Ogl_Shader shader_prog;
} Ogl_Renderer;

global Ogl_Renderer *ogl_renderer = 0;

global char vert_shader_src[] =
  "#version 330\n"
  "layout (location = 0) in vec3 v_p;\n"
  "layout (location = 1) in vec2 v_uv;\n"
  "layout (location = 2) in vec4 v_c;\n"
  "out vec2 f_uv;\n"
  "out vec4 f_c;\n"
  "void main() {\n"
  "gl_Position = vec4(v_p, 1);\n"
  "f_uv = v_uv;\n"
  "f_c = v_c;\n"  
  "}\n";

global char frag_shader_src[] =
  "#version 330\n"
  "in vec2 f_uv;\n"
  "in vec4 f_c;\n"
  "uniform sampler2D atlas;\n"
  "out vec4 out_color;\n"
  "void main() {\n"
  "vec4 sampled = texture(atlas, f_uv);\n"
  "out_color = f_c * vec4(sampled.r, sampled.r, sampled.r, sampled.r);\n"
  "}\n";

//proc U32 ogl_create_texture(S32 width, S32 height, Ogl_PixelFormat tex_fmt, Ogl_PixelFormat pix_fmt, void *pix_data);

proc Ogl_Shader ogl_make_shader(Arena *arena, char *src, GLenum kind);
proc Ogl_Shader ogl_make_program(Arena *arena, GLuint *shaders, U32 shader_count);
