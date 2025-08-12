proc U32
ogl_create_texture(S32 width, S32 height, Ogl_PixelFormat tex_fmt, Ogl_PixelFormat pix_fmt, void *pix_data)
{ 
  U32 result;
  glGenTextures(1, &result);
  glBindTexture(GL_TEXTURE_2D, result);
  glTexImage2D(GL_TEXTURE_2D, 0, ogl_fmts[tex_fmt], width, height, 0, ogl_fmts[pix_fmt], GL_UNSIGNED_BYTE, pix_data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  return(result);
}

proc GlShader
gl_make_shader(Arena *arena, char *src, GLenum kind)
{  
  GLuint shader_id = glCreateShader(kind);
  glShaderSource(shader_id, 1, (const GLchar*const*)&src, 0);
  glCompileShader(shader_id);

  String8 log = {0};
  GLint info_log_length;
  glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
  if(info_log_length) {
    
    char *buffer = arena_push_array(arena, char, info_log_length + 1);
    GLint len = 0;
    glGetShaderInfoLog(shader_id, info_log_length + 1, &len, buffer);
    log.count = len;
    log.string = (U8*)buffer;
  }

  GLint compile_status;
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_status);
  if(compile_status == 0) {
    glDeleteShader(shader_id);
    shader_id = 0;
  }

  GlShader result = {0};
  result.handle = shader_id;
  result.log = log;
  return(result);
}

proc GlShader
gl_make_program(Arena *arena, GLuint *shaders, U32 shader_count)
{
  GLuint program_id = glCreateProgram();
  for(U32 shader_idx = 0; shader_idx < shader_count; ++shader_idx)
    { glAttachShader(program_id, shaders[shader_idx]); }
  glLinkProgram(program_id);

  String8 log = {0};
  GLint info_log_length;
  glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
  if(info_log_length) {
    
    char *buffer = arena_push_array(arena, char, info_log_length + 1);
    GLint len = 0;
    glGetProgramInfoLog(program_id, info_log_length + 1, &len, buffer);
    log.count = len;
    log.string = (U8*)buffer;
  }

  GLint link_status;
  glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);
  if(link_status == 0) {
    glDeleteProgram(program_id);
    program_id = 0;
  }  

  GlShader result = {0};
  result.handle = program_id;
  result.log = log;
  return(result);
}
