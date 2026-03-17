#if OS_LINUX
// TODO: we should probably run-time load opengl on all platforms
#  define GL_GLEXT_PROTOTYPES
#  include "GL/gl.h"
#elif OS_WINDOWS
#  include "ogl_api_defs.h"
#else
#  error platform not implemented
#endif

typedef struct Ogl_Shader
{
  GLuint handle;
  String8 log;
} Ogl_Shader;

proc B32 ogl_init(void);

proc Ogl_Shader ogl_make_shader(Arena *arena, char *src, GLenum kind);
proc Ogl_Shader ogl_make_program(Arena *arena, GLuint *shaders, U32 shader_count);

proc void ogl_debug_message_callback(GLenum src, GLenum type, GLuint id, GLenum severity, GLsizei len, const GLchar *msg, const void *user_param);

proc GLuint ogl_create_texture(V2S32 dim, void *pixels, GLint internal_fmt, GLint pixel_fmt, B32 wrap_x, B32 wrap_y);
proc void ogl_update_texture(GLuint tex, V2S32 pos, V2S32 dim, GLint pixel_fmt, void *pixels);

#define GLTextureDefaultParams() \
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); \
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); \
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); \
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); \

proc GLuint ogl_create_framebuffer(S32 width, S32 height);

#if OS_WINDOWS
#  include "OpenGL/windows/ogl_win32.h"
#elif OS_LINUX
#  include "ogl.linux.h"
#endif
