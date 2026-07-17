proc B32
ogl_mac_init(void)
{
  void *ogl_handle = 0;

  mac_ogl_ctxt = ogl_mac_create_context();
  if(mac_ogl_ctxt.ns_ctxt == 0)
  { goto ogl_mac_init_failure; }

  ogl_handle = dlopen("/System/Library/Frameworks/OpenGL.framework/OpenGL", RTLD_LAZY|RTLD_LOCAL);
  if(ogl_handle == 0)
  { goto ogl_mac_init_failure; }

#define X(N, R, A)\
  N = dlsym(ogl_handle, Stringify(N));\
  if(N == 0)\
  { fprintf(stderr, "ogl mac init failure: failed to load symbol: %s\n", Stringify(N)); goto ogl_mac_init_failure; }
  OGL_FUNCTION_XLIST__1_0__LEGACY;
  OGL_FUNCTION_XLIST__1_0;
  OGL_FUNCTION_XLIST__1_1;
  OGL_FUNCTION_XLIST__1_2;
  OGL_FUNCTION_XLIST__1_3;
  OGL_FUNCTION_XLIST__1_4;
  OGL_FUNCTION_XLIST__1_5;
  OGL_FUNCTION_XLIST__2_0;
  OGL_FUNCTION_XLIST__2_1;
  OGL_FUNCTION_XLIST__3_0;
  OGL_FUNCTION_XLIST__3_1;
  OGL_FUNCTION_XLIST__3_2;
  OGL_FUNCTION_XLIST__3_3;
  OGL_FUNCTION_XLIST__4_0;
  OGL_FUNCTION_XLIST__4_1;
#undef X

  dlclose(ogl_handle);

  glGetIntegerv(GL_MAJOR_VERSION, &ogl_context_major_version);
  glGetIntegerv(GL_MINOR_VERSION, &ogl_context_minor_version);

  return 1;

ogl_mac_init_failure:
  if(ogl_handle)
  { dlclose(ogl_handle); }
  return 0;
}

proc Ogl_MacContext
ogl_mac_create_context(void)
{
  const NSOpenGLPixelFormatAttribute attr[] = {
    NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
    NSOpenGLPFAColorSize, 24,
    NSOpenGLPFADepthSize, 16,
    NSOpenGLPFADoubleBuffer,
    0,
  };
  NSOpenGLPixelFormat *fmt = NSOpenGLPixelFormat_initWithAttributes(attr);

  NSOpenGLContext *ns_ctxt = NSOpenGLContext_initWithFormat(fmt, 0);

  GLint swap_interval = 1;
  NSOpenGLContext_setValues(ns_ctxt, &swap_interval, NSOpenGLContextParameterSwapInterval);

  NSOpenGLContext_makeCurrentContext(ns_ctxt);

  NSRelease(fmt);

  Ogl_MacContext result = {0};
  result.ns_ctxt = ns_ctxt;
  return result;
}

proc void
ogl_mac_make_context_current(Ogl_MacContext *ctxt)
{
  NSOpenGLContext *ns_ctxt = ctxt->ns_ctxt;
  NSOpenGLContext_makeCurrentContext(ns_ctxt);
}

proc void
ogl_mac_swap_buffers(Ogl_MacContext *ctxt)
{
  NSOpenGLContext *ns_ctxt = ctxt->ns_ctxt;
  NSOpenGLContext_flushBuffer(ns_ctxt);
}
