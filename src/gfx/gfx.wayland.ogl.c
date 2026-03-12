proc void
gl_debug_message_callback(GLenum src, GLenum type, GLuint id, GLenum severity, GLsizei len,
                          const GLchar *msg, const void *user_param)
{
  Unused(user_param);
  fprintf(stderr, "src=%u, type=%u, id=%u, severity=%u: %.*s\n", src, type, id, severity, len, msg);
}

proc WaylandGl_Window*
wayland_gl__window_get(Wayland_Window *window)
{
  WaylandGl_Window *result = window->ogl_backend;
  return(result);
}

proc WaylandGl_Framebuffer
wayland_gl__window_get_framebuffer(WaylandGl_Window *window)
{
  WaylandGl_Framebuffer result = window->framebuffers[window->backbuffer_index];
  return(result);
}

proc B32
wayland_gl__create_buffer(WaylandGl_Window *window)
{
  B32 result = 1;

  Wayland_Window *wayland_window = window->wayland_window;

  ArenaTemp scratch = arena_get_scratch(0, 0);

  Wayland_Id *params = wayland__new_id(wayland_window);
  if(zwp_linux_dmabuf_v1_create_params(wayland_state->zwp_linux_dmabuf_v1->id, params->id))
  {
    window->zwp_linux_buffer_params_v1 = params;
    DLLPushBack(wayland_state->first_zwp_linux_buffer_params_v1,
		wayland_state->last_zwp_linux_buffer_params_v1,
		params);
    ++wayland_state->zwp_linux_buffer_params_v1_count;

    WaylandGl_Framebuffer framebuffer = window->framebuffers[window->backbuffer_index];
    if(zwp_linux_buffer_params_v1_add(window->zwp_linux_buffer_params_v1->id,
                                      framebuffer.fd,
                                      0,
                                      framebuffer.offset,
                                      framebuffer.stride,
                                      framebuffer.modifier_hi,
                                      framebuffer.modifier_lo))
    {
      Wayland_Id *buffer = wayland__new_id(wayland_window);
      if(zwp_linux_buffer_params_v1_create_immed(window->zwp_linux_buffer_params_v1->id, buffer->id,
                                                 wayland_window->width, wayland_window->height, framebuffer.fourcc, 0))
      {
        window->buffers[window->backbuffer_index] = buffer;
        // TODO: do we really need separate lists for gl and non-gl buffers?
        DLLPushBack(wayland_state->first_gl_buffer, wayland_state->last_gl_buffer, buffer);
        ++wayland_state->gl_buffer_count;
      } else {
        result = 0;
      }
    } else {
      result = 0;
    }
  } else {
    result = 0;
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wayland_gl__allocate_textures(WaylandGl_Window *window)
{
  B32 result = 1;

  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(gl_debug_message_callback, 0);

  glEnable(GL_DEPTH_TEST);

  S32 texture_width = 1920, texture_height =  1080;

  for(U32 i = 0; i < ArrayCount(window->framebuffers) && result; ++i)
  {
    GLuint color_texture, depth_texture;
    glGenTextures(1, &color_texture);
    glGenTextures(1, &depth_texture);

    glBindTexture(GL_TEXTURE_2D, color_texture);
    GlTextureDefaultParams();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 texture_width, texture_height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glBindTexture(GL_TEXTURE_2D, depth_texture);
    GlTextureDefaultParams();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8,
                 texture_width, texture_height,
                 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);

    window->framebuffers[i].color_texture = color_texture;
    window->framebuffers[i].depth_texture = depth_texture;
    window->framebuffers[i].fbo = fbo;
    window->framebuffers[i].width = texture_width;
    window->framebuffers[i].height = texture_height;

    EGLImage egl_image = wayland_gl_state->eglCreateImageKHR(wayland_gl_state->egl_display,
							     wayland_gl_state->egl_context,
							     EGL_GL_TEXTURE_2D,
							     PtrFromInt(color_texture),
							     0);

    int num_planes, fourcc;
    EGLuint64KHR modifiers;
    if(wayland_gl_state->eglExportDMABUFImageQueryMESA(wayland_gl_state->egl_display,
						       egl_image,
						       &fourcc,
						       &num_planes,
						       &modifiers))
    {
      Assert(num_planes == 1); // TODO: guarantee this somehow

      int fd;
      EGLint stride, offset;
      if(wayland_gl_state->eglExportDMABUFImageMESA(wayland_gl_state->egl_display,
						    egl_image,
						    &fd,
						    &stride,
						    &offset))
      {
        U32 modifier_hi = modifiers >> 32;
        U32 modifier_lo = modifiers & U32_MAX;

        window->framebuffers[i].egl_image = egl_image;
        window->framebuffers[i].num_planes = num_planes;
        window->framebuffers[i].fourcc = fourcc;
        window->framebuffers[i].fd = fd;
        window->framebuffers[i].stride = stride;
        window->framebuffers[i].offset = offset;
        window->framebuffers[i].modifier_hi = modifier_hi;
        window->framebuffers[i].modifier_lo = modifier_lo;
      } else {
        result = 0;
      }
    } else {
      result = 0;
    }
  }

  glFlush(); // NOTE: avoid some bugs on some drivers

  return(result);
}

proc B32
wayland_gl_init(void)
{
  B32 result = 0;

  wayland_gl_state = arena_push_struct(wayland_state->arena, WaylandGl_State);

  EGLDisplay egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if(egl_display != EGL_NO_DISPLAY)
  {
    EGLint egl_major, egl_minor;
    if(eglInitialize(egl_display, &egl_major, &egl_minor))
    {
      if(eglBindAPI(EGL_OPENGL_API))
      {
	const char *egl_extensions_cstr = eglQueryString(egl_display, EGL_EXTENSIONS);
	fprintf(stderr, "egl extensions: %s\n", egl_extensions_cstr);

	String8 egl_extensions_str = Str8Cstr((char*)egl_extensions_cstr);
	if(str8_contains(egl_extensions_str, Str8Lit("EGL_MESA_image_dma_buf_export")))
	{
          EGLint attrib_list[] = {EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT, EGL_NONE};
          EGLConfig egl_config;
          EGLint num_config;
          if(eglChooseConfig(egl_display, attrib_list, &egl_config, 1, &num_config))
	  {
            EGLContext egl_context = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, 0);
            if(egl_context != EGL_NO_CONTEXT)
	    {
              if(eglMakeCurrent(egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, egl_context))
	      {
                wayland_gl_state->egl_display = egl_display;
                wayland_gl_state->egl_context = egl_context;
		wayland_gl_state->eglCreateImageKHR =
		  (PFNEGLCREATEIMAGEKHRPROC)eglGetProcAddress("eglCreateImageKHR");
		wayland_gl_state->eglExportDMABUFImageQueryMESA =
		  (PFNEGLEXPORTDMABUFIMAGEQUERYMESAPROC)eglGetProcAddress("eglExportDMABUFImageQueryMESA");
		wayland_gl_state->eglExportDMABUFImageMESA =
		  (PFNEGLEXPORTDMABUFIMAGEMESAPROC)eglGetProcAddress("eglExportDMABUFImageMESA");
		result = 1;
              }
            }
          }
        }
      }
    }
  }

  return(result);
}

proc void
wayland_gl_close(void)
{

}

proc B32
wayland_gl_window_open(Wayland_Window *window)
{
  WaylandGl_Window *gl_window = wayland_gl__window_create(window);
  B32 result = wayland_gl__allocate_textures(gl_window);
  return(result);
}

proc void
wayland_gl_window_close(Wayland_Window *window)
{
  WaylandGl_Window *gl_window = wayland_gl__window_get(window);

  zwp_linux_buffer_params_v1_destroy(gl_window->zwp_linux_buffer_params_v1->id);
  wayland__release_id(gl_window->zwp_linux_buffer_params_v1);

  for(U32 i = 0; i < ArrayCount(gl_window->framebuffers); ++i)
  {
    glDeleteTextures(1, &gl_window->framebuffers[i].color_texture);
    glDeleteTextures(1, &gl_window->framebuffers[i].depth_texture);
    glDeleteFramebuffers(1, &gl_window->framebuffers[i].fbo);
  }

  wayland_gl__window_destroy(gl_window);
}

proc B32
wayland_gl_window_begin(Wayland_Window *window)
{
  WaylandGl_Window *gl_window = wayland_gl__window_get(window);
  WaylandGl_Framebuffer framebuffer = wayland_gl__window_get_framebuffer(gl_window);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);
}

proc B32
wayland_gl_window_end(Wayland_Window *window)
{

}
