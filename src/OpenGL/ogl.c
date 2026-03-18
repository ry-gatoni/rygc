#if OS_WINDOWS
#  include "OpenGl/windows/ogl_win32.c"
#elif OS_LINUX
#  include "ogl.linux.c"
#endif

proc B32
ogl_init(void)
{
  B32 result = 1;

#if OS_WINDOWS
  result = ogl_win32_init();
#elif OS_LINUX
  result = ogl_linux_init();
#endif

  if(result)
  {
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(ogl_debug_message_callback, 0);

    glEnable(GL_BLEND);

    glEnable(GL_DEPTH_TEST);
  }

  return(result);
}

proc Ogl_Shader
ogl_make_shader(Arena *arena, char *src, GLenum kind)
{
  GLuint shader_id = glCreateShader(kind);
  glShaderSource(shader_id, 1, (const GLchar*const*)&src, 0);
  glCompileShader(shader_id);

  String8 log = {0};
  GLint info_log_length;
  glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
  if(info_log_length)
  {
    char *buffer = arena_push_array(arena, char, info_log_length + 1);
    GLint len = 0;
    glGetShaderInfoLog(shader_id, info_log_length + 1, &len, buffer);
    log.count = len;
    log.string = (U8*)buffer;
  }

  GLint compile_status;
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_status);
  if(compile_status == 0)
  {
    glDeleteShader(shader_id);
    shader_id = 0;
  }

  Ogl_Shader result = {0};
  result.handle = shader_id;
  result.log = log;
  return(result);
}

proc Ogl_Shader
ogl_make_program(Arena *arena, GLuint *shaders, U32 shader_count)
{
  GLuint program_id = glCreateProgram();
  for(U32 shader_idx = 0; shader_idx < shader_count; ++shader_idx)
  { glAttachShader(program_id, shaders[shader_idx]); }
  glLinkProgram(program_id);

  String8 log = {0};
  GLint info_log_length;
  glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
  if(info_log_length)
  {
    char *buffer = arena_push_array(arena, char, info_log_length + 1);
    GLint len = 0;
    glGetProgramInfoLog(program_id, info_log_length + 1, &len, buffer);
    log.count = len;
    log.string = (U8*)buffer;
  }

  GLint link_status;
  glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);
  if(link_status == 0)
  {
    glDeleteProgram(program_id);
    program_id = 0;
  }

  Ogl_Shader result = {0};
  result.handle = program_id;
  result.log = log;
  return(result);
}

proc GLuint
ogl_create_texture(V2S32 dim, void *pixels, GLint internal_fmt, GLint pixel_fmt, B32 wrap_x, B32 wrap_y)
{
  GLuint handle;
  glGenTextures(1, &handle);
  glBindTexture(GL_TEXTURE_2D, handle);

  glTexImage2D(GL_TEXTURE_2D, 0, internal_fmt, dim.width, dim.height, 0, pixel_fmt, GL_UNSIGNED_BYTE, pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_x ? GL_REPEAT : GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_y ? GL_REPEAT : GL_CLAMP_TO_EDGE);

  return(handle);
}

proc void
ogl_update_texture(GLuint tex, V2S32 pos, V2S32 dim, GLint pixel_fmt, void *pixels)
{
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexSubImage2D(GL_TEXTURE_2D, 0, pos.x, pos.y, dim.width, dim.height, pixel_fmt, GL_UNSIGNED_BYTE, pixels);
}

proc GLuint
ogl_create_framebuffer(S32 width, S32 height)
{
  GLuint color_tex, depth_tex;
  glGenTextures(1, &color_tex);
  glGenTextures(1, &depth_tex);

  glBindTexture(GL_TEXTURE_2D, color_tex);
  GLTextureDefaultParams();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
	       width, height, 0, GL_RGBA,
	       GL_UNSIGNED_BYTE, 0);

  glBindTexture(GL_TEXTURE_2D, depth_tex);
  GLTextureDefaultParams();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8,
	       width, height, 0, GL_DEPTH_STENCIL,
	       GL_UNSIGNED_INT_24_8, 0);

  GLuint fbo;
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_tex, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_tex, 0);

  return(fbo);
}

proc void
ogl_read_framebuffer_pixels(GLuint fbo, V2S32 pos, V2S32 dim, void *pixels)
{
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glReadPixels(pos.x, pos.y, dim.width, dim.height, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
}

proc void
ogl_debug_message_callback(GLenum src, GLenum type, GLuint id, GLenum severity, GLsizei len,
			   const GLchar *msg, const void *user_param)
{
  Unused(user_param);
  fprintf(stderr, "src=%u, type=%u, id=%u, severity=%u: %.*s\n", src, type, id, severity, len, msg);
}


#if 0
// -----------------------------------------------------------------------------
// render implementations

proc R_Texture
render_create_texture_ex(S32 width, S32 height, void *pixels, R_TextureCreateParams *params)
{
  U32 handle;
  glGenTextures(1, &handle);
  glBindTexture(GL_TEXTURE_2D, handle);

  GLint internal_fmt = ogl_fmts[params->internal_fmt];
  GLint pixel_fmt = ogl_fmts[params->pixel_fmt];
  glTexImage2D(GL_TEXTURE_2D, 0, internal_fmt, width, height, 0, pixel_fmt, GL_UNSIGNED_BYTE, pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, params->wrap_x ? GL_REPEAT : GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, params->wrap_y ? GL_REPEAT : GL_CLAMP_TO_EDGE);

  R_Texture result = {0};
  result.handle.handle = PtrFromInt(handle);
  result.dim = v2s32(width, height);
  return(result);
}

proc void
render_update_texture(R_Texture *texture, S32 pos_x, S32 pos_y, S32 width, S32 height, R_PixelFormat format, void *pixels)
{
  U32 handle = (U32)IntFromPtr(texture->handle.handle);
  glBindTexture(GL_TEXTURE_2D, handle);

  glTexSubImage2D(GL_TEXTURE_2D, 0, pos_x, pos_y, width, height, ogl_fmts[format], GL_UNSIGNED_BYTE, pixels);
}
#endif
