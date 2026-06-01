// -----------------------------------------------------------------------------
// global helpers

proc Xcb_BackendFlag
xcb_backend_flag(Xcb_Backend backend)
{
  return(1 << backend);
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
xcb_init(void)
{
  Arena *arena = arena_alloc();
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

  xcb_state->screen = screen;
  xcb_state->g_ctxt = g_ctxt;
  xcb_state->extensions = extension_flags;
  for(Xcb_Backend b = 0; b < Xcb_Backend_Count; ++b) xcb_init_backend(b);
  // TODO: set enabled backend
  return(1);

  xcb_init_failure:
  if(conn) xcb_disconnect(conn);
  arena_release(arena);
  xcb_state = 0;
  return(0);
}

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
      // TODO: implement
    }break;

    // NOTE: invalid default case
    default: { Assert(0); }break;
  }
}

// -----------------------------------------------------------------------------
// window open/close

proc Xcb_Window*
xcb_window_open(V2S32 dim)
{
  Xcb_Window *win = xcb_window_alloc();

  // NOTE: create window
  xcb_window_t window_id = xcb_generate_id(xcb_state->conn);
  {
    xcb_connection_t *conn = xcb_state->conn;
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
      XCB_EVENT_MASK_BUTTON_MOTION,
    };
    XcbCheckRequest(xcb_create_window, (conn, depth, window_id, parent, 0, 0, dim.width, dim.height, border, _class, visual, value_mask, values),
		    "window open: create window failure", xcb_window_open_failure);
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
    xcb_connection_t *conn = xcb_state->conn;
    XcbCheckRequest(xcb_map_window, (conn, window_id),
		    "window open: window map failure", xcb_window_open_failure);
  }

  xcb_flush(xcb_state->conn);

  win->id = window_id;
  win->dim = dim;
  return(win);

xcb_window_open_failure:
  // TODO: cleanup
  xcb_window_close(win);
  return(0);
}

proc void
xcb_window_close(Xcb_Window *win)
{
  // TODO: release resources
  xcb_window_free(win);
}

proc void
xcb_window_init_backend(Xcb_Window *win, Xcb_Backend backend)
{
  // TODO: maybe don't put all the backends implemenations in one function, in
  // case dependencies are not available on some machine and the code can't
  // compile
  xcb_pixmap_t pixmap = xcb_generate_id(xcb_state->conn);
  switch(backend)
  {
    case Xcb_Backend_base:
    {
      xcb_connection_t *conn = xcb_state->conn;
      U8 depth = xcb_state->screen->root_depth;
      xcb_drawable_t drawable = xcb_state->screen->root;
      U16 width = xcb_state->screen->width_in_pixels;
      U16 height = xcb_state->screen->height_in_pixels;
      XcbCheckRequest(xcb_create_pixmap, (conn, depth, pixmap, drawable, width, height),
		      "window init backend: base: create pixmap failure", xcb_window_init_backend_failure);

      // TODO: we need freelists for backend states and pixel buffers in order
      // to make arena allocation work here
      Arena *arena = xcb_state->arena;
      Xcb_WindowBase *base = arena_push_struct_z(arena, Xcb_WindowBase);
      base->pixmap = pixmap;
      base->pixels = arena_push_array_z(arena, U32, width*height);
      base->pixels_width = width;
      base->pixels_height = height;
      win->backend_states[backend] = base;
    }break;

    case Xcb_Backend_shm:
    {
      // TODO: error handling
      U16 width = xcb_state->screen->width_in_pixels;
      U16 height = xcb_state->screen->height_in_pixels;
      int shmid = shmget(IPC_PRIVATE, width*height*sizeof(U32), IPC_CREAT|0600);
      U32 *pixels = shmat(shmid, 0, 0);

      // NOTE: attach shm segment and create pixmap
      xcb_shm_seg_t shm_segment = xcb_generate_id(xcb_state->conn);
      {
	xcb_connection_t *conn = xcb_state->conn;
	XcbCheckRequest(xcb_shm_attach, (conn, shm_segment, shmid, 0),
			"window init backend: shm: attach shm failure", xcb_window_init_backend_failure);

	xcb_drawable_t drawable = xcb_state->screen->root;
	U8 depth = xcb_state->screen->root_depth;
	XcbCheckRequest(xcb_shm_create_pixmap, (conn, pixmap, drawable, width, height, depth, shm_segment, 0),
			"window init backend: shm: create pixmap failure", xcb_window_init_backend_failure);

	// TODO: we need freelists for backend states and pixel buffers in order
	// to make arena allocation work here
	Arena *arena = xcb_state->arena;
	Xcb_WindowShm *shm = arena_push_struct_z(arena, Xcb_WindowShm);
	shm->pixmap = pixmap;
	shm->pixels = pixels;
	shm->pixels_width = width;
	shm->pixels_height = height;
	shm->shm_id = shmid;
	shm->shm_segment = shm_segment;
	win->backend_states[backend] = shm;
      }
    }break;

    case Xcb_Backend_ogl:
    {
      // TODO: implement
      Assert(0);
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

  SLLQueuePush(xcb_state->first_window, xcb_state->last_window, result);
  ++xcb_state->window_count;
  return(result);
}

proc void
xcb_window_free(Xcb_Window *win)
{
  --xcb_state->window_count;
  SLLQueuePop(xcb_state->first_window, xcb_state->last_window);
  SLLStackPush(xcb_state->window_freelist, win);
}
