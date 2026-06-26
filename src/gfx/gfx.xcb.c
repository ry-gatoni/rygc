// -----------------------------------------------------------------------------
// global helpers

proc Xcb_BackendFlag
xcb_backend_flag(Xcb_Backend backend)
{
  return(1 << backend);
}

proc B32
xcb_backend_is_supported(Xcb_Backend backend)
{
  B32 result = xcb_state->supported_backends & xcb_backend_flag(backend);
  return(result);
}

proc void
xcb_log_error(xcb_generic_error_t *error, char *msg)
{
  int error_code = error->error_code;
  free(error);
  if(error_code != 0) fprintf(stderr, "%s\n", msg); // TODO: better logging
}

#define XcbCheckRequest(req, args, failure_msg, failure_label)\
  xcb_request_cookie = Glue(req, _checked)args;\
  if((xcb_error = xcb_request_check(xcb_state->conn, xcb_request_cookie)))\
  {\
    xcb_log_error(xcb_error, failure_msg);\
    xcb_error = 0;\
    goto failure_label;\
  }

// -----------------------------------------------------------------------------
// init/uninit

proc B32
xcb_init(Arena *arena)
{
  xcb_state = arena_push_struct(arena, Xcb_State);
  xcb_state->arena = arena;

  // TODO: dynamically load libs?

  // NOTE: open connection
  // TODO: optionally override X server
  xcb_connection_t *conn = xcb_connect(0, 0);
  int xcb_error_code = xcb_connection_has_error(conn);
  if(xcb_error_code != 0) goto xcb_init_failure;
  xcb_state->conn = conn;

  // NOTE: get screen
  // TODO: get preferred screen
  xcb_setup_t const *setup = xcb_get_setup(conn);
  xcb_screen_iterator_t screen_it = xcb_setup_roots_iterator(setup);
  xcb_screen_t *screen = screen_it.data;

  // NOTE: query extensions
  U32 extension_flags = 0;
  {
    xcb_shm_query_version_cookie_t shm_cookie = xcb_shm_query_version(conn);
    xcb_shm_query_version_reply_t *shm_reply = xcb_shm_query_version_reply(conn, shm_cookie, 0);
    xcb_query_extension_reply_t const *present_reply = xcb_get_extension_data(conn, &xcb_present_id);

    if(shm_reply && shm_reply->shared_pixmaps)
    {
      extension_flags |= Xcb_Extension_shm;
    }
    if(present_reply && present_reply->present)
    {
      extension_flags |= Xcb_Extension_present;
    }
  }

  // NOTE: get wm protocols and wm delete window cookies
  xcb_atom_t wm_proto_atom, wm_del_win_atom;
  {
    String8 wm_proto_name = Str8Lit("WM_PROTOCOLS");
    xcb_intern_atom_cookie_t wm_proto_cookie =
      xcb_intern_atom(conn, 1, wm_proto_name.count, (const char*)wm_proto_name.string);
    xcb_intern_atom_reply_t *wm_proto_reply = xcb_intern_atom_reply(conn, wm_proto_cookie, 0);
    wm_proto_atom = wm_proto_reply->atom;

    String8 wm_del_win_name = Str8Lit("WM_DELETE_WINDOW");
    xcb_intern_atom_cookie_t wm_del_win_cookie =
      xcb_intern_atom(conn, 1, wm_del_win_name.count, (const char*)wm_del_win_name.string);
    xcb_intern_atom_reply_t *wm_del_win_reply = xcb_intern_atom_reply(conn, wm_del_win_cookie, 0);
    wm_del_win_atom = wm_del_win_reply->atom;
  }

  // NOTE: create graphics context
  xcb_gcontext_t g_ctxt = xcb_generate_id(conn);
  {
    xcb_drawable_t drawable = screen->root;
    U32 value_mask = XCB_GC_FOREGROUND|XCB_GC_GRAPHICS_EXPOSURES;
    U32 values[] = {
      screen->white_pixel,
      0,
    };
    XcbCheckRequest(xcb_create_gc, (conn, g_ctxt, drawable, value_mask, values),
		    "create gc failure", xcb_init_failure);
  }
  xcb_state->g_ctxt = g_ctxt;

  xcb_state->screen = screen;
  xcb_state->extensions = extension_flags;
  xcb_state->wm_proto_atom = wm_proto_atom;
  xcb_state->wm_del_win_atom = wm_del_win_atom;
  for(Xcb_Backend b = 0; b < Xcb_Backend_Count; ++b) xcb_init_backend(b);
  // TODO: set enabled backend
  return(1);

xcb_init_failure:
  xcb_uninit();
  return(0);
}

proc void
xcb_uninit(void)
{
  if(xcb_state)
  {
    // TODO: release backend state (if we ever have any)
    if(xcb_state->g_ctxt) xcb_free_gc(xcb_state->conn, xcb_state->g_ctxt);
    if(xcb_state->conn) xcb_disconnect(xcb_state->conn);

    xcb_state = 0;
  }
}

global PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT = 0;
global PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC eglCreatePlatformWindowSurfaceEXT = 0;

proc void
xcb_init_backend(Xcb_Backend backend)
{
  // TODO: maybe don't put all the backends implemenations in one function, in
  // case dependencies are not available on some machine and the code can't
  // compile
  switch(backend)
  {
    case Xcb_Backend_base:
    {
      // NOTE: always supported, no state needed
      xcb_state->supported_backends |= xcb_backend_flag(backend);
    }break;

    case Xcb_Backend_shm:
    {
      // NOTE: supported if the shm extension is available, no state needed
      if(xcb_state->extensions & Xcb_Extension_shm)
      { xcb_state->supported_backends |= xcb_backend_flag(backend); }
    }break;

    case Xcb_Backend_ogl:
    {
      // TODO: load lib dynamically?

      EGLDisplay egl_display = EGL_NO_DISPLAY;
      EGLConfig egl_config = 0;
      EGLContext egl_context = EGL_NO_CONTEXT;
      EGLBoolean egl_result = 1;

      // NOTE: load extensions
      eglGetPlatformDisplayEXT =
	(PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");
      eglCreatePlatformWindowSurfaceEXT =
	(PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC)eglGetProcAddress("eglCreatePlatformWindowSurfaceEXT");

      // NOTE: get egl display
      egl_display = eglGetPlatformDisplayEXT(EGL_PLATFORM_XCB_EXT, xcb_state->conn, 0);
      if(egl_display == EGL_NO_DISPLAY) goto xcb_init_backend__ogl_failure;

      // NOTE: initialize egl
      EGLint egl_major, egl_minor;
      egl_result = eglInitialize(egl_display, &egl_major, &egl_minor);
      if(!egl_result) goto xcb_init_backend__ogl_failure;

      // NOTE: bind api
      egl_result = eglBindAPI(EGL_OPENGL_API);
      if(!egl_result) goto xcb_init_backend__ogl_failure;

      // NOTE: get egl config
      EGLint attrib_list[] = {EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT, EGL_NONE};
      EGLint num_config;
      egl_result = eglChooseConfig(egl_display, attrib_list, &egl_config, 1, &num_config);
      if(!egl_result) goto xcb_init_backend__ogl_failure;

      // NOTE: create egl context
      egl_context = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, 0);
      if(egl_context == EGL_NO_CONTEXT) goto xcb_init_backend__ogl_failure;

      // NOTE: allocate backend state, set backend enabled
      Arena *arena = xcb_state->arena;
      Xcb_BackendOgl *ogl = arena_push_struct(arena, Xcb_BackendOgl);
      ogl->egl_display = egl_display;
      ogl->egl_config = egl_config;
      ogl->egl_context = egl_context;

      xcb_state->backend_states[Xcb_Backend_ogl] = ogl;
      xcb_state->supported_backends |= xcb_backend_flag(Xcb_Backend_ogl);
      return;

  xcb_init_backend__ogl_failure:
      if(egl_context != EGL_NO_CONTEXT) eglDestroyContext(egl_display, egl_context);
      if(egl_display != EGL_NO_DISPLAY) eglTerminate(egl_display);
    }break;

    // NOTE: invalid default case
    default: { Assert(0); }break;
  }
}

// -----------------------------------------------------------------------------
// window open/close

proc Xcb_Window*
xcb_window_open(V2S32 dim, String8 title)
{
  xcb_connection_t *conn = xcb_state->conn;

  Xcb_Window *win = xcb_window_alloc();

  // NOTE: create window
  xcb_window_t window_id = xcb_generate_id(xcb_state->conn);
  {
    U8 depth = XCB_COPY_FROM_PARENT;
    xcb_window_t parent = xcb_state->screen->root;
    U16 border = 10;
    U16 _class = XCB_WINDOW_CLASS_INPUT_OUTPUT;
    xcb_visualid_t visual = xcb_state->screen->root_visual;
    U32 value_mask = XCB_CW_OVERRIDE_REDIRECT|XCB_CW_EVENT_MASK;
    U32 values[] = {
      0,
      XCB_EVENT_MASK_EXPOSURE|
      XCB_EVENT_MASK_KEY_PRESS|
      XCB_EVENT_MASK_KEY_RELEASE|
      XCB_EVENT_MASK_BUTTON_PRESS|
      XCB_EVENT_MASK_BUTTON_RELEASE|
      XCB_EVENT_MASK_BUTTON_MOTION|
      XCB_EVENT_MASK_STRUCTURE_NOTIFY,
    };
    XcbCheckRequest(xcb_create_window, (conn, depth, window_id, parent, 0, 0, dim.width, dim.height, border, _class, visual, value_mask, values),
		    "window open: create window failure", xcb_window_open_failure);
  }

  win->id = window_id;

  // NOTE: set title
  {
    U8 mode = XCB_PROP_MODE_REPLACE;
    xcb_atom_t property = XCB_ATOM_WM_NAME;
    xcb_atom_t type = XCB_ATOM_STRING;
    U8 fmt = 8;
    U32 data_len = title.count;
    const void *data = title.string;
    XcbCheckRequest(xcb_change_property, (conn, mode, window_id, property, type, fmt, data_len, data),
		    "window open: set title failure", xcb_window_open_failure);
  }

  // NOTE: set up window close event
  {
    U8 mode = XCB_PROP_MODE_REPLACE;
    xcb_atom_t property = xcb_state->wm_proto_atom;
    xcb_atom_t type = XCB_ATOM_ATOM;
    U8 fmt = 32;
    U32 data_len = 1;
    const void *data = &xcb_state->wm_del_win_atom;
    XcbCheckRequest(xcb_change_property, (conn, mode, window_id, property, type, fmt, data_len, data),
		    "window open: configure close event failure", xcb_window_open_failure);
  }

  // NOTE: init supported backends
  for(Xcb_Backend b = 0; b < Xcb_Backend_Count; ++b)
  {
    // TODO: disable the backend if window initialization fails
    if(xcb_state->supported_backends & xcb_backend_flag(b))
    { xcb_window_init_backend(win, b); }
  }

  // NOTE: put the window on the screen
  {
    XcbCheckRequest(xcb_map_window, (conn, window_id),
		    "window open: window map failure", xcb_window_open_failure);
  }

  xcb_flush(xcb_state->conn);

  win->dim = dim;
  return(win);

xcb_window_open_failure:
  xcb_window_close(win);
  return(0);
}

proc void
xcb_window_close(Xcb_Window *win)
{
  if(win)
  {
    if(win->id) xcb_destroy_window(xcb_state->conn, win->id);
    xcb_window_free(win);
  }
}

proc Gfx_Handle
gfx_window_open(S32 width, S32 height, String8 name)
{
  Xcb_Window *win = xcb_window_open(v2s32(width, height), name);
  Gfx_Handle result = xcb__gfx_handle_from_window(win);
  return result;
}

proc void
gfx_window_close(Gfx_Handle window)
{
  Xcb_Window *win = xcb__window_from_gfx_handle(window);
  xcb_window_close(win);
}

proc void
xcb_window_init_backend(Xcb_Window *win, Xcb_Backend backend)
{
  // TODO: maybe don't put all the backends implemenations in one function, in
  // case dependencies are not available on some machine and the code can't
  // compile
  xcb_connection_t *conn = xcb_state->conn;
  switch(backend)
  {
    case Xcb_Backend_base:
    {
      Xcb_WindowBase *base = xcb_state->window_backend_freelist[Xcb_Backend_base];
      if(base)
      {
	Xcb_WindowBase *freelist = xcb_state->window_backend_freelist[Xcb_Backend_base];
	SLLStackPop_N(freelist, next_free);
      }
      else
      {
	U8 depth = xcb_state->screen->root_depth;
	xcb_pixmap_t pixmap = xcb_generate_id(conn);
	xcb_drawable_t drawable = xcb_state->screen->root;
	U16 width = xcb_state->screen->width_in_pixels;
	U16 height = xcb_state->screen->height_in_pixels;
	XcbCheckRequest(xcb_create_pixmap, (conn, depth, pixmap, drawable, width, height),
			"window init backend: base: create pixmap failure", xcb_window_init_backend_failure);

	Arena *arena = xcb_state->arena;
	base = arena_push_struct_z(arena, Xcb_WindowBase);
	base->pixmap = pixmap;
	base->pixels = arena_push_array_z(arena, U32, width*height);
	base->pixels_width = width;
	base->pixels_height = height;

	U32 const base_pixel_color = 0xFF3377FF;
	for(U16 j = 0; j < height; ++j)
	{
	  for(U16 i = 0; i < width; ++i)
	  {
	    base->pixels[j*width + i] = base_pixel_color;
	  }
	}
      }
      Assert(base);

      win->backend_states[backend] = base;
    }break;

    case Xcb_Backend_shm:
    {
      Xcb_WindowShm *shm = xcb_state->window_backend_freelist[Xcb_Backend_shm];
      if(shm)
      {
	Xcb_WindowShm *freelist = xcb_state->window_backend_freelist[Xcb_Backend_shm];
	SLLStackPop_N(freelist, next_free);
      }
      else
      {
	// TODO: error handling
	// NOTE: allocate shared memory segment
	U16 width = xcb_state->screen->width_in_pixels;
	U16 height = xcb_state->screen->height_in_pixels;
	int shmid = shmget(IPC_PRIVATE, width*height*sizeof(U32), IPC_CREAT|0600);
	U32 *pixels = shmat(shmid, 0, 0);

	// NOTE: attach segment
	xcb_shm_seg_t shm_segment = xcb_generate_id(conn);
	XcbCheckRequest(xcb_shm_attach, (conn, shm_segment, shmid, 0),
			"window init backend: shm: attach shm failure", xcb_window_init_backend_failure);

	// NOTE: create pixmap
	xcb_pixmap_t pixmap = xcb_generate_id(conn);
	xcb_drawable_t drawable = xcb_state->screen->root;
	U8 depth = xcb_state->screen->root_depth;
	XcbCheckRequest(xcb_shm_create_pixmap, (conn, pixmap, drawable, width, height, depth, shm_segment, 0),
			"window init backend: shm: create pixmap failure", xcb_window_init_backend_failure);


	Arena *arena = xcb_state->arena;
	shm = arena_push_struct_z(arena, Xcb_WindowShm);
	shm->pixmap = pixmap;
	shm->pixels = pixels;
	shm->pixels_width = width;
	shm->pixels_height = height;
	shm->shm_id = shmid;
	shm->shm_segment = shm_segment;

	U32 const shm_pixel_color = 0xFF7733FF;
	for(U16 j = 0; j < height; ++j)
	{
	  for(U16 i = 0; i < width; ++i)
	  {
	    shm->pixels[j*width + i] = shm_pixel_color;
	  }
	}
      }
      Assert(shm);

      win->backend_states[backend] = shm;
    }break;

    case Xcb_Backend_ogl:
    {
      Xcb_WindowOgl *ogl = xcb_state->window_backend_freelist[Xcb_Backend_ogl];
      if(ogl)
      {
	Xcb_WindowOgl *freelist = xcb_state->window_backend_freelist[Xcb_Backend_ogl];
	SLLStackPop_N(freelist, next_free);
      }
      else
      {
	xcb_pixmap_t pixmap = xcb_generate_id(conn);
	xcb_drawable_t drawable = xcb_state->screen->root;
	U16 width = xcb_state->screen->width_in_pixels;
	U16 height = xcb_state->screen->height_in_pixels;
	U8 depth = xcb_state->screen->root_depth;
	XcbCheckRequest(xcb_create_pixmap, (conn, depth, pixmap, drawable, width, height),
			"window init backend: ogl: create pixmap failure", xcb_window_init_backend_failure);

	Xcb_BackendOgl *ogl_backend = xcb_state->backend_states[Xcb_Backend_ogl];
	EGLDisplay dpy = ogl_backend->egl_display;
	EGLConfig cfg = ogl_backend->egl_config;
	xcb_window_t win_id = win->id;
	EGLSurface egl_surface = eglCreatePlatformWindowSurfaceEXT(dpy, cfg, &win_id, 0);
	if(egl_surface == EGL_NO_SURFACE) goto xcb_window_init_backend_failure; // TODO: destroy pixmap?

	Arena *arena = xcb_state->arena;
	ogl = arena_push_struct(arena, Xcb_WindowOgl);
	ogl->pixmap = pixmap;
	ogl->egl_surface = egl_surface;
      }
      Assert(ogl);

      win->backend_states[backend] = ogl;
    }break;

    // NOTE: invalid default case
    default: { Assert(0); } break;
  }

  return;

xcb_window_init_backend_failure:
  // TODO: clean up resources on failure
  // NOTE: mark backend state as disabled if its initialization failed
  if(!win->backend_states[backend])
  { xcb_state->supported_backends &= ~xcb_backend_flag(backend); }
}

proc B32
xcb_select_backend(Xcb_Window *win, Xcb_Backend backend)
{
  B32 result = 0;
  if(xcb_state->supported_backends & xcb_backend_flag(backend))
  {
    win->selected_backend = backend;
    result = 1;
  }
  return(result);
}

// -----------------------------------------------------------------------------
// window helpers

proc Xcb_Window*
xcb_window_alloc(void)
{
  Xcb_Window *result = xcb_state->window_freelist;
  if(result)
  {
    SLLStackPop(xcb_state->window_freelist);
  }
  else
  {
    result = arena_push_struct(xcb_state->arena, Xcb_Window);
  }
  Assert(result);
  ZeroStruct(result);

  SLLQueuePush(xcb_state->first_window, xcb_state->last_window, result);
  ++xcb_state->window_count;
  return(result);
}

proc void
xcb_window_free(Xcb_Window *win)
{
  --xcb_state->window_count;
  SLLQueuePop(xcb_state->first_window, xcb_state->last_window);

  for(Xcb_Backend b = 0; b < Xcb_Backend_Count; ++b)
  {
    if(!xcb_backend_is_supported(b)) continue;

    // NOTE: this is annoying
    switch(b)
    {
      case Xcb_Backend_base:
      {
	Xcb_WindowBase *freelist = xcb_state->window_backend_freelist[b];
	Xcb_WindowBase *base = win->backend_states[b];
	SLLStackPush_N(freelist, base, next_free);
      }break;

      case Xcb_Backend_shm:
      {
	Xcb_WindowShm *freelist = xcb_state->window_backend_freelist[b];
	Xcb_WindowShm *shm = win->backend_states[b];
	SLLStackPush_N(freelist, shm, next_free);
      }break;

      case Xcb_Backend_ogl:
      {
	Xcb_WindowOgl *freelist = xcb_state->window_backend_freelist[b];
	Xcb_WindowOgl *ogl = win->backend_states[b];
	SLLStackPush_N(freelist, ogl, next_free);
      }break;

      // NOTE: invalid default case
      default: { Assert(0); }break;
    }
  }

  SLLStackPush(xcb_state->window_freelist, win);
}

proc Xcb_Window*
xcb_window_from_id(xcb_window_t id)
{
  Xcb_Window *win = 0;

  // TODO: accelerate
  for(Xcb_Window *w = xcb_state->first_window; w; w = w->next)
  {
    if(w->id == id)
    {
      win = w;
      break;
    }
  }

  return(win);
}

proc Gfx_Handle
xcb__gfx_handle_from_window(Xcb_Window *win)
{
  Gfx_Handle result = {0};
  result.handle = win;
  return(result);
}

proc Xcb_Window*
xcb__window_from_gfx_handle(Gfx_Handle handle)
{
  Xcb_Window *result = handle.handle;
  return(result);
}

// -----------------------------------------------------------------------------
// events

proc void
xcb_events(void)
{
  xcb_connection_t *conn = xcb_state->conn;
  xcb_generic_event_t *xcb_event = 0;
  while((xcb_event = xcb_poll_for_event(conn)))
  {
    // NOTE: mask off top bit to properly identify CLIENT_MESSAGE events
    switch(xcb_event->response_type & 0x7f)
    {
      case XCB_EXPOSE:
      {
	// TODO: we make some requests/api calls here, so it'd be good log/handle any errors

	xcb_expose_event_t *expose_event = (xcb_expose_event_t*)xcb_event;
	xcb_window_t win_id = expose_event->window;
	Xcb_Window *win = xcb_window_from_id(win_id);
	switch(win->selected_backend)
	{
	  case Xcb_Backend_base:
	  {
	    Xcb_WindowBase *win_base = win->backend_states[Xcb_Backend_base];

	    U8 format = XCB_IMAGE_FORMAT_Z_PIXMAP;
	    xcb_drawable_t drawable = win_id;
	    xcb_gcontext_t g_ctxt = xcb_state->g_ctxt;
	    U16 width = win->dim.width;
	    U16 height = win->dim.height;
	    S16 dst_x = 0;
	    S16 dst_y = 0;
	    U8 left_pad = 0;
	    U8 depth = xcb_state->screen->root_depth;
	    U32 data_len = width*height*sizeof(*win_base->pixels);
	    U8 *data = (U8*)win_base->pixels;
	    xcb_put_image(conn, format, drawable, g_ctxt, width, height, dst_x, dst_y, left_pad, depth, data_len, data);
	  }break;

	  case Xcb_Backend_shm:
	  {
	    Xcb_WindowShm *win_shm = win->backend_states[Xcb_Backend_shm];

	    xcb_drawable_t drawable = win_id;
	    xcb_gcontext_t g_ctxt = xcb_state->g_ctxt;
	    U16 total_width = xcb_state->screen->width_in_pixels;
	    U16 total_height = xcb_state->screen->height_in_pixels;
	    U16 src_x = 0;
	    U16 src_y = 0;
	    U16 src_width = win->dim.width;
	    U16 src_height = win->dim.height;
	    S16 dst_x = 0;
	    S16 dst_y = 0;
	    U8 depth = xcb_state->screen->root_depth;
	    U8 format = XCB_IMAGE_FORMAT_Z_PIXMAP;
	    U8 send_event = 0;
	    xcb_shm_seg_t shm_seg = win_shm->shm_segment;
	    U32 offset = 0;
	    xcb_shm_put_image(conn, drawable, g_ctxt, total_width, total_height, src_x, src_y, src_width, src_height, dst_x, dst_y, depth, format, send_event, shm_seg, offset);
	  }break;

	  case Xcb_Backend_ogl:
	  {
	    Xcb_BackendOgl *backend_ogl = xcb_state->backend_states[Xcb_Backend_ogl];
	    Xcb_WindowOgl *win_ogl = win->backend_states[Xcb_Backend_ogl];

	    EGLDisplay dpy = backend_ogl->egl_display;
	    EGLSurface surf = win_ogl->egl_surface;
	    EGLContext ctxt = backend_ogl->egl_context;
	    eglMakeCurrent(dpy, surf, surf, ctxt);

	    glClearColor(1.f, 0.2f, 0.5f, 1.f);
	    glClear(GL_COLOR_BUFFER_BIT);

	    eglSwapBuffers(dpy, surf);
	  }break;

	  // NOTE: invalid default case
	  default: { Assert(0); }break;
	}

	xcb_flush(conn);
      }break;

      case XCB_CLIENT_MESSAGE:
      {
	xcb_client_message_event_t *client_message = (xcb_client_message_event_t*)xcb_event;
	Xcb_Window *win = xcb_window_from_id(client_message->window);
	if(client_message->data.data32[0] == xcb_state->wm_del_win_atom)
	{
	  // NOTE: close window
	  Gfx_Event *event = gfx__event_new();
	  event->kind = Gfx_EventKind_close;
	  event->window = xcb__gfx_handle_from_window(win);
	  gfx__event_push(event);
	}
      }break;

      // NOTE: resize
      case XCB_CONFIGURE_NOTIFY:
      {
	xcb_configure_notify_event_t *configure_event = (xcb_configure_notify_event_t*)xcb_event;
	Xcb_Window *win = xcb_window_from_id(configure_event->window);
	win->dim.width = configure_event->width;
	win->dim.height = configure_event->height;
      }break;

      // NOTE: mouse move
      case XCB_MOTION_NOTIFY:
      {
	xcb_motion_notify_event_t *motion_event = (xcb_motion_notify_event_t*)xcb_event;
	Xcb_Window *win = xcb_window_from_id(motion_event->event);
	Gfx_Event *event = gfx__event_new();
	event->kind = Gfx_EventKind_move;
	event->window = xcb__gfx_handle_from_window(win);
	event->pos = v2(motion_event->event_x, motion_event->event_y);
	gfx__event_push(event);
      }break;

      // NOTE: mouse button press/release, discrete mouse wheel scroll
      case XCB_BUTTON_PRESS:
      case XCB_BUTTON_RELEASE:
      {
	xcb_button_press_event_t *button_event = (xcb_button_press_event_t*)xcb_event;

	Xcb_Window *win = xcb_window_from_id(button_event->event);
	Gfx_Handle gfx_win = xcb__gfx_handle_from_window(win);
	V2 pos = v2(button_event->event_x, button_event->event_y);
	if(button_event->detail == XCB_BUTTON_INDEX_4 || button_event->detail == XCB_BUTTON_INDEX_5)
	{
	  Gfx_Event *event = gfx__event_new();
	  event->kind = Gfx_EventKind_scroll;
	  event->window = gfx_win;
	  event->pos = pos;
	  event->scroll = v2(0, button_event->detail == XCB_BUTTON_INDEX_4 ? 1 : -1);
	  gfx__event_push(event);
	}
	else
	{
	  Gfx_Event *event = gfx__event_new();
	  event->kind = xcb_event->response_type == XCB_BUTTON_PRESS ? Gfx_EventKind_press : Gfx_EventKind_release;
	  event->window = gfx_win;
	  event->pos = pos;
	  event->key = xcb_button_map[button_event->detail];
	  gfx__event_push(event);

	}
      }break;

      // TODO: translate events to gfx layer format, push to buffer
      case XCB_KEY_PRESS:
      case XCB_KEY_RELEASE:

      default: { printf("xcb_events: unhandled response type: %u\n", xcb_event->response_type); }break;
    }

    free(xcb_event);
  }
}
