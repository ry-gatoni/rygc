#if OS_LINUX
// TODO: we should probably run-time load opengl on all platforms
#  define GL_GLEXT_PROTOTYPES
#  include "GL/gl.h"
#elif OS_WINDOWS
#  include "ogl_api_defs.h"
#else
#  error platform not implemented
#endif

proc B32 ogl_init(void);

#if OS_WINDOWS
#  include "OpenGl/windows/ogl_win32.h"
#endif
