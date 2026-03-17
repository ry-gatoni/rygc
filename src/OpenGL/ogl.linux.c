proc B32
ogl_linux_init(void)
{
  if(egl_state.initialized) return(1);

  B32 result = 0;
  EGLDisplay display = EGL_NO_DISPLAY;
  EGLContext context = EGL_NO_CONTEXT;

  display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if(display == EGL_NO_DISPLAY) goto ogl_linux_init_failure;

  EGLint egl_major, egl_minor;
  if(!eglInitialize(display, &egl_major, &egl_minor)) goto ogl_linux_init_failure;
  if(!eglBindAPI(EGL_OPENGL_API)) goto ogl_linux_init_failure;

  const char *egl_extensions = eglQueryString(display, EGL_EXTENSIONS);

  EGLConfig config;
  EGLint num_config;
  if(!eglChooseConfig(display, egl_attrib_list, &config, 1, &num_config)) goto ogl_linux_init_failure;

  context = eglCreateContext(display, config, EGL_NO_CONTEXT, 0);
  if(context == EGL_NO_CONTEXT) goto ogl_linux_init_failure;
  if(!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, context)) goto ogl_linux_init_failure;

  result = 1;

  ogl_linux_init_failure:
  if(!result){
    return(result);
  }

  egl_state.initialized = 1;
  egl_state.display = display;
  egl_state.major_version = egl_major;
  egl_state.minor_version = egl_minor;
  egl_state.extensions = egl_extensions;
  egl_state.config = config;
  egl_state.num_config = num_config;
  egl_state.context = context;

  return(result);
}

/* proc B32 */
/* ogl_init(void) */
/* { */
/*   result = ogl_linux_init(); */
/*   return(result); */
/* } */
