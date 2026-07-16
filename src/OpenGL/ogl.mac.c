proc B32
ogl_mac_init(void)
{
  void *ogl_handle = dlopen("/System/Library/Frameworks/OpenGL.framework/OpenGL", RTLD_LAZY|RTLD_LOCAL);
  if(ogl_handle == 0)
  { goto ogl_mac_init_failure; }

#define X(N, R, A)\
  N = dlsym(ogl_handle, Stringify(N));\
  if(N == 0)\
  { goto ogl_mac_init_failure; }
  OGL_FUNCTION_XLIST;
#undef X

  dlclose(ogl_handle);
  return 1;

ogl_mac_init_failure:
  if(ogl_handle)
  { dlclose(ogl_handle); }
  return 0;
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
