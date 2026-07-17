// NOTE: copied with modifications from
// https://github.com/KhronosGroup/OpenGL-Registry/blob/main/api/GL/glcorearb.h,
// https://github.com/KhronosGroup/OpenGL-Registry/blob/main/api/GL/glext.h, and
// https://registry.khronos.org/EGL/api/KHR/khrplatform.h

// NOTE: typedefs
// TODO: maybe define these in terms of base layer types/defines
typedef void			GLvoid;
typedef unsigned int		GLenum;
typedef float			GLfloat;
typedef int			GLint;
typedef int			GLsizei;
typedef unsigned int		GLbitfield;
typedef double			GLdouble;
typedef unsigned int		GLuint;
typedef unsigned char		GLboolean;
typedef unsigned char		GLbyte;
typedef char			GLchar;
typedef float			GLclampf;
typedef double			GLclampd;
#if defined(_WIN64)
typedef signed long long int    GLintptr;
typedef signed long long int	GLsizeiptr;
#else
typedef signed long int         GLintptr;
typedef signed long int		GLsizeiptr;
#endif

// NOTE: constants
#define GL_RGBA                           0x1908
#define GL_BGRA                           0x80E1
#define GL_RED                            0x1903
#define GL_INFO_LOG_LENGTH                0x8B84
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_TEXTURE_2D                     0x0DE1
#define GL_TEXTURE_MAG_FILTER             0x2800
#define GL_TEXTURE_MIN_FILTER             0x2801
#define GL_TEXTURE_WRAP_S                 0x2802
#define GL_TEXTURE_WRAP_T                 0x2803
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_LINEAR                         0x2601
#define GL_UNSIGNED_BYTE                  0x1401
#define GL_FLOAT                          0x1406
#define GL_BLEND                          0x0BE2
#define GL_SRC_ALPHA                      0x0302
#define GL_ONE_MINUS_SRC_ALPHA            0x0303
#define GL_LESS                           0x0201
#define GL_UNPACK_ALIGNMENT               0x0CF5
#define GL_PACK_ALIGNMENT                 0x0D05
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_ARRAY_BUFFER                   0x8892
#define GL_DEPTH_BUFFER_BIT               0x00000100
#define GL_STENCIL_BUFFER_BIT             0x00000400
#define GL_COLOR_BUFFER_BIT               0x00004000
#define GL_STREAM_DRAW                    0x88E0
#define GL_TEXTURE0                       0x84C0
#define GL_TRIANGLES                      0x0004
#define GL_TRIANGLE_STRIP                 0x0005
#define GL_DEBUG_OUTPUT                   0x92E0
#define GL_DEPTH_TEST                     0x0B71
#define GL_REPEAT                         0x2901
#define GL_DEPTH_STENCIL                  0x84F9
#define GL_DEPTH24_STENCIL8               0x88F0
#define GL_UNSIGNED_INT_24_8              0x84FA
#define GL_FRAMEBUFFER                    0x8D40
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_DEPTH_STENCIL_ATTACHMENT       0x821A
#define GL_MAJOR_VERSION                  0x821B
#define GL_MINOR_VERSION                  0x821C
#define GL_FALSE                          0
#define GL_TRUE                           1
#define GL_NO_ERROR                       0
#define GL_INVALID_ENUM                   0x0500
#define GL_INVALID_VALUE                  0x0501
#define GL_INVALID_OPERATION              0x0502
#define GL_OUT_OF_MEMORY                  0x0505


// NOTE: functions
// NOTE: x macro format: name, return, args
#define OGL_FUNCTION_XLIST__1_0__LEGACY\
  X(glVertex3f, void, (GLfloat x, GLfloat y, GLfloat z))\
  X(glColor3f, void, (GLfloat red, GLfloat green, GLfloat blue))\
  X(glTexCoord2f, void, (GLfloat s, GLfloat t))\
  X(glBegin, void, (GLenum mode))\
  X(glEnd, void, (void))\

#define OGL_FUNCTION_XLIST__1_0\
  X(glTexParameteri, void, (GLenum target, GLenum pname, GLint param))\
  X(glTexImage2D, void, (GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *data))\
  X(glClear, void, (GLbitfield mask))\
  X(glClearColor, void, (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha))\
  X(glClearDepth, void, (GLclampd depth))\
  X(glDisable, void, (GLenum cap))\
  X(glEnable, void, (GLenum cap))\
  X(glFlush, void, (void))\
  X(glBlendFunc, void, (GLenum sfactor, GLenum dfactor))\
  X(glDepthFunc, void, (GLenum func))\
  X(glPixelStorei, void, (GLenum pname, GLint param))\
  X(glReadPixels, void, (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels))\
  X(glGetError, GLenum, (void))\
  X(glGetIntegerv, void, (GLenum pname, GLint *data))\
  X(glViewport, void, (GLint x, GLint y, GLsizei width, GLsizei height))\

#define OGL_FUNCTION_XLIST__1_1\
  X(glDrawArrays, void, (GLenum mode, GLint first, GLsizei count))\
  X(glTexSubImage2D, void, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *data))\
  X(glBindTexture, void, (GLenum target, GLuint texture))\
  X(glGenTextures, void, (GLsizei n, GLuint *textures))\

#define OGL_FUNCTION_XLIST__1_2

#define OGL_FUNCTION_XLIST__1_3\
  X(glActiveTexture, void, (GLenum texture))\

#define OGL_FUNCTION_XLIST__1_4

#define OGL_FUNCTION_XLIST__1_5\
  X(glBindBuffer, void, (GLenum target, GLuint buffer))\
  X(glGenBuffers, void, (GLsizei n, GLuint *buffers))\
  X(glBufferData, void, (GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage))\
  X(glBufferSubData, void, (GLenum target, GLintptr offset, GLsizeiptr size, const void *data))\

#define OGL_FUNCTION_XLIST__2_0\
  X(glAttachShader, void, (GLuint program, GLuint shader))\
  X(glCompileShader, void, (GLuint shader))\
  X(glCreateProgram, GLuint, (void))\
  X(glCreateShader, GLuint, (GLenum shaderType))\
  X(glDeleteProgram, void, (GLuint program))\
  X(glDeleteShader, void, (GLuint shader))\
  X(glEnableVertexAttribArray, void, (GLuint index))\
  X(glGetProgramiv, void, (GLuint program, GLenum pname, GLint *params))\
  X(glGetProgramInfoLog, void, (GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog))\
  X(glGetShaderiv, void, (GLuint shader, GLenum pname, GLint *params))\
  X(glGetShaderInfoLog, void, (GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog))\
  X(glGetUniformLocation, GLint, (GLuint program, const GLchar *name))\
  X(glLinkProgram, void, (GLuint program))\
  X(glShaderSource, void, (GLuint shader, GLsizei count, const GLchar*const*string, const GLint *length))\
  X(glUseProgram, void, (GLuint program))\
  X(glUniform1i, void, (GLint location, GLint v0))\
  X(glUniformMatrix4fv, void, (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value))\
  X(glVertexAttribPointer, void, (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer))\

#define OGL_FUNCTION_XLIST__2_1

#define OGL_FUNCTION_XLIST__3_0\
  X(glBindFramebuffer, void, (GLenum target, GLuint framebuffer))\
  X(glGenFramebuffers, void, (GLsizei n, GLuint *framebuffers))\
  X(glFramebufferTexture2D, void, (GLenum target, GLenum attachment, GLenum tex_target, GLuint texture, GLint level))\
  X(glBindVertexArray, void, (GLuint array))\
  X(glGenVertexArrays, void, (GLsizei n, GLuint *arrays))\

#define OGL_FUNCTION_XLIST__3_1\
  X(glDrawArraysInstanced, void, (GLenum mode, GLint first, GLsizei count, GLsizei instance_count))\

#define OGL_FUNCTION_XLIST__3_2

#define OGL_FUNCTION_XLIST__3_3\
  X(glGenSamplers, void, (GLsizei n, GLuint *samplers))\
  X(glVertexAttribDivisor, void, (GLuint index, GLuint divisor))\

#define OGL_FUNCTION_XLIST__4_0
#define OGL_FUNCTION_XLIST__4_1
#define OGL_FUNCTION_XLIST__4_2

#define OGL_FUNCTION_XLIST__4_3\
  X(glDebugMessageCallback, void, (void (*callback)(GLenum sourc, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *user_param), const void *user_param))\

#define OGL_FUNCTION_XLIST__4_4
#define OGL_FUNCTION_XLIST__4_5
#define OGL_FUNCTION_XLIST__4_6

#define OGL_FUNCTION_XLIST\
  OGL_FUNCTION_XLIST__1_0__LEGACY\
  OGL_FUNCTION_XLIST__1_0\
  OGL_FUNCTION_XLIST__1_1\
  OGL_FUNCTION_XLIST__1_2\
  OGL_FUNCTION_XLIST__1_3\
  OGL_FUNCTION_XLIST__1_4\
  OGL_FUNCTION_XLIST__1_5\
  OGL_FUNCTION_XLIST__2_0\
  OGL_FUNCTION_XLIST__2_1\
  OGL_FUNCTION_XLIST__3_0\
  OGL_FUNCTION_XLIST__3_1\
  OGL_FUNCTION_XLIST__3_2\
  OGL_FUNCTION_XLIST__3_3\
  OGL_FUNCTION_XLIST__4_0\
  OGL_FUNCTION_XLIST__4_1\
  OGL_FUNCTION_XLIST__4_2\
  OGL_FUNCTION_XLIST__4_3\
  OGL_FUNCTION_XLIST__4_4\
  OGL_FUNCTION_XLIST__4_5\
  OGL_FUNCTION_XLIST__4_6\

// NOTE: function typedefs
#define X(N, R, A) typedef R Glue(GL_, N)A;
OGL_FUNCTION_XLIST;
#undef X

// NOTE: global function pointers
#define X(N, R, A) global Glue(GL_, N) *N = 0;
OGL_FUNCTION_XLIST;
#undef X
