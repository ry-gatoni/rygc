#include <EGL/egl.h>
#include <EGL/eglext.h> // TODO: runtime link egl

global EGLint egl_attrib_list[] = {
  EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT, EGL_NONE,
};

typedef struct EGL_State
{
  B32 initialized;

  EGLDisplay display;

  EGLint major_version;
  EGLint minor_version;

  const char *extensions;

  EGLConfig config;
  EGLint num_config;

  EGLContext context;
} EGL_State;

global EGL_State egl_state = {0};

proc B32 ogl_linux_init(void);
