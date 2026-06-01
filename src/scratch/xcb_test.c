#include <xcb/xcb.h>
#include <xcb/shm.h>
#include <xcb/present.h>

#include <EGL/egl.h>
//#define EGL_EGLEXT_PROTOTYPES
#include <EGL/eglext.h>

#include <sys/shm.h>

#include "base/base.h"

#include "base/base.c"

// TODO:
// - vulkan context
// - read keysyms/keycodes
// - gracefully handle window close
// - handle window resize

typedef enum Xcb_ExtensionFlags
{
  Xcb_Extension_shm	= 1 << 0,
  Xcb_Extension_present = 1 << 1,
} Xcb_ExtensionFlags;

typedef enum Xcb_Backend
{
  Xcb_Backend_base,
  Xcb_Backend_shm,
  Xcb_Backend_ogl,
  Xcb_Backend_Count,
} Xcb_Backend;

typedef enum Xcb_BackendFlags
{
  Xcb_BackendFlag_base = 1 << Xcb_Backend_base,
  Xcb_BackendFlag_shm  = 1 << Xcb_Backend_shm,
  Xcb_BackendFlag_ogl  = 1 << Xcb_Backend_ogl,
} Xcb_BackendFlags;

typedef struct Xcb_Window Xcb_Window;
struct Xcb_Window
{
  Xcb_Window *next;

  xcb_window_t id;
  xcb_pixmap_t pixmap;

  V2S32 dim;
};

typedef struct Xcb_State
{
  Arena *arena;
  xcb_connection_t *conn;

  Xcb_Window *first_window;
  Xcb_Window *last_window;
  U64 window_count;
  Xcb_Window *window_freelist;

  xcb_screen_t *screen;

  xcb_gcontext_t g_ctxt;

  U32 extensions;

  U32 supported_backends;
  Xcb_Backend enabled_backend;
  void *backend_state[Xcb_Backend_Count];
} Xcb_State;

global Xcb_State *xcb_state = 0;
global xcb_void_cookie_t xcb_request_cookie = {0};
global xcb_generic_error_t *xcb_error = 0;

#define XcbCheckRequest(req, args, failure_msg, failure_label)\
  xcb_request_cookie = Glue(req, _checked)args;\
  if((xcb_error = xcb_request_check(xcb_state->conn, xcb_request_cookie)))\
  {\
    log_xcb_error(xcb_error, failure_msg);\
    xcb_error = 0;\
    goto failure_label;\
  }

proc void
log_xcb_error(xcb_generic_error_t *xcb_error, char *msg)
{
  int xcb_error_code = xcb_error->error_code;
  free(xcb_error);
  if(xcb_error_code != 0) fprintf(stderr, "%s\n", msg);
}

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

  // TODO: init backend states, set enabled backend

  xcb_state->screen = screen;
  xcb_state->g_ctxt = g_ctxt;
  xcb_state->extensions = extension_flags;
  return(1);

  xcb_init_failure:
  if(conn) xcb_disconnect(conn);
  arena_release(arena);
  xcb_state = 0;
  return(0);
}

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

proc Xcb_Window*
xcb_window_open(V2S32 dim)
{
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

  // NOTE: create pixmap
  xcb_pixmap_t pixmap = xcb_generate_id(xcb_state->conn);
  // TODO: only initialize for selected backend, or initialize for all backends
  // to allow hotswapping renderer?
  switch(xcb_state->enabled_backend)
  {
    case Xcb_Backend_base:
    {
      xcb_connection_t *conn = xcb_state->conn;
      U8 depth = xcb_state->screen->root_depth;
      xcb_drawable_t drawable = xcb_state->screen->root;
      XcbCheckRequest(xcb_create_pixmap, (conn, depth, pixmap, drawable, dim.width, dim.height),
		      "window open: base: create pixmap failure", xcb_window_open_failure);
      // TODO: allocate pixels
      Assert(0);
    }break;

    case Xcb_Backend_shm:
    {
      // TODO: error handling
      int shmid = shmget(IPC_PRIVATE, dim.width*dim.height*sizeof(U32), IPC_CREAT|0600);
      U8 *pixels = shmat(shmid, 0, 0);

      // NOTE: attach shm segment and create pixmap
      xcb_shm_seg_t shm_segment = xcb_generate_id(xcb_state->conn);
      {
	xcb_connection_t *conn = xcb_state->conn;
	XcbCheckRequest(xcb_shm_attach, (conn, shm_segment, shmid, 0),
			"window open: shm: attach shm failure", xcb_window_open_failure);

	xcb_drawable_t drawable = xcb_state->screen->root;
	U8 depth = xcb_state->screen->root_depth;
	XcbCheckRequest(xcb_shm_create_pixmap, (conn, pixmap, drawable, dim.width, dim.height, depth, shm_segment, 0),
			"window open: shm: create pixmap failure", xcb_window_open_failure);
	// TODO: save to backend state
	Assert(0);
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

  // NOTE: put the window on the screen
  {
    xcb_connection_t *conn = xcb_state->conn;
    XcbCheckRequest(xcb_map_window, (conn, window_id),
		    "window open: window map failure", xcb_window_open_failure);
  }

  Xcb_Window *win = xcb_window_alloc();
  win->id = window_id;
  win->pixmap = pixmap;
  win->dim = dim;
  return(win);

xcb_window_open_failure:
  // TODO: cleanup
  return(0);
}

proc void
xcb_window_close(Xcb_Window *win)
{
  // TODO: release resources
  xcb_window_free(win);
}

typedef enum XcbExpose
{
  XcbExpose_fill_rect,
  XcbExpose_put_image,
  XcbExpose_put_shm_image,
} XcbExpose;

global XcbExpose xcb_expose = XcbExpose_put_shm_image;

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  // NOTE: persistent resources that should be released at program end
  int xcb_error_code = 0;
  xcb_generic_error_t *xcb_error = 0;
  xcb_void_cookie_t xcb_request_cookie = {0};

  xcb_window_t window_id = (U32)-1;

  xcb_shm_seg_t xcb_shm_segment = {0};
  U8 *shm = 0;

  EGLDisplay egl_display = EGL_NO_DISPLAY;
  EGLSurface egl_surface = EGL_NO_SURFACE;
  EGLContext egl_context = EGL_NO_CONTEXT;

  Arena *xcb_arena = arena_alloc();

  if(!xcb_init()) return(1);

  // NOTE: open connection
  // TODO: optionally override X server
  // TODO: get preferred screen
  xcb_connection_t *xcb_connection = xcb_connect(0, 0);
  xcb_error_code = xcb_connection_has_error(xcb_connection);
  if(xcb_error_code != 0) goto finish;

  // NOTE: get connection info
  xcb_setup_t const *xcb_setup = xcb_get_setup(xcb_connection);
  xcb_screen_iterator_t xcb_screen_it = xcb_setup_roots_iterator(xcb_setup);
  xcb_screen_t *xcb_screen = xcb_screen_it.data;
  V2S32 screen_dim = { .width = xcb_screen->width_in_pixels, .height = xcb_screen->height_in_pixels};

  // NOTE: query extensions
  B32 shm_supported = 0, present_supported = 0;
  {
    xcb_shm_query_version_cookie_t shm_query_version_cookie =
      xcb_shm_query_version(xcb_connection);

    xcb_shm_query_version_reply_t *shm_query_version_reply =
      xcb_shm_query_version_reply(xcb_connection, shm_query_version_cookie, 0);
    const xcb_query_extension_reply_t *present_query_extension_reply =
      xcb_get_extension_data(xcb_connection, &xcb_present_id);

    if(shm_query_version_reply)
    {
      shm_supported = shm_query_version_reply->shared_pixmaps;
    }
    if(present_query_extension_reply)
    {
      present_supported = present_query_extension_reply->present;
    }
  }

  xcb_pixmap_t shm_pixmap = {0};
  if(shm_supported)
  {
    int shmid = shmget(IPC_PRIVATE, screen_dim.width*screen_dim.height*4, IPC_CREAT|0660);
    shm = shmat(shmid, 0, 0);

    xcb_shm_segment = xcb_generate_id(xcb_connection);
    xcb_request_cookie = xcb_shm_attach_checked(xcb_connection, xcb_shm_segment, shmid, 0);
    if((xcb_error = xcb_request_check(xcb_connection, xcb_request_cookie)))
    {
      log_xcb_error(xcb_error, "create shm failure");
      xcb_error = 0;
      goto finish;
    }

    shm_pixmap = xcb_generate_id(xcb_connection);
    xcb_request_cookie = xcb_shm_create_pixmap_checked(xcb_connection,
						       shm_pixmap,
						       xcb_screen->root,
						       xcb_screen->width_in_pixels,
						       xcb_screen->height_in_pixels,
						       xcb_screen->root_depth,
						       xcb_shm_segment,
						       0);
    if((xcb_error = xcb_request_check(xcb_connection, xcb_request_cookie)))
    {
      log_xcb_error(xcb_error, "create shm pixmap failure");
      xcb_error = 0;
      goto finish;
    }

    U32 *shm_pixels = (U32*)shm;
    for(S32 j = 0; j < screen_dim.height; ++j)
    {
      for(S32 i = 0; i < screen_dim.width; ++i)
      {
	shm_pixels[j*screen_dim.width + i] = 0xFF7733FF;
      }
    }
  }

  if(present_supported)
  {
  }

  // NOTE: create graphics context
  xcb_gcontext_t xcb_graphics_context = xcb_generate_id(xcb_connection);
  xcb_drawable_t xcb_drawable = xcb_screen->root;
  {
    U32 value_mask = XCB_GC_FOREGROUND|XCB_GC_GRAPHICS_EXPOSURES;
    U32 values[] = {
      xcb_screen->white_pixel,
      0,
    };
    xcb_request_cookie = xcb_create_gc_checked(xcb_connection,
					       xcb_graphics_context,
					       xcb_drawable,
					       value_mask,
					       values);
    if((xcb_error = xcb_request_check(xcb_connection, xcb_request_cookie)))
    {
      log_xcb_error(xcb_error, "create graphics context failure");
      xcb_error = 0;
      goto finish;
    }
  }

  // NOTE: create pixmap
  xcb_pixmap_t xcb_pixmap = xcb_generate_id(xcb_connection);
  xcb_request_cookie =
    xcb_create_pixmap_checked(xcb_connection,
			      xcb_screen->root_depth,
			      xcb_pixmap,
			      xcb_drawable,
			      xcb_screen->width_in_pixels, xcb_screen->height_in_pixels);
  if((xcb_error = xcb_request_check(xcb_connection, xcb_request_cookie)))
  {
    xcb_error_code = xcb_error->error_code;
    free(xcb_error);
    xcb_error = 0;
    if(xcb_error_code != 0) fprintf(stderr, "create pixmap failure\n"); goto finish;
  }
  U32 *backbuffer_pixels = arena_push_array(xcb_arena, U32, screen_dim.width*screen_dim.height);
  for(S32 i = 0; i < screen_dim.height; ++i)
  {
    for(S32 j = 0; j < screen_dim.width; ++j)
    {
      // NOTE: lo BGRA hi
      backbuffer_pixels[i*screen_dim.width + j] = 0xFFFF007F;
    }
  }

  // NOTE: open window
  U32 value_mask = XCB_CW_OVERRIDE_REDIRECT|XCB_CW_EVENT_MASK;
  U32 values[] = {
    0,
    /* xcb_screen->white_pixel, */
    XCB_EVENT_MASK_EXPOSURE|
    XCB_EVENT_MASK_KEY_PRESS|
    XCB_EVENT_MASK_KEY_RELEASE|
    XCB_EVENT_MASK_BUTTON_PRESS|
    XCB_EVENT_MASK_BUTTON_RELEASE|
    XCB_EVENT_MASK_POINTER_MOTION|
    XCB_EVENT_MASK_BUTTON_MOTION,
  };
  window_id = xcb_generate_id(xcb_connection);
  xcb_request_cookie = xcb_create_window_checked(xcb_connection,
						 XCB_COPY_FROM_PARENT,
						 window_id, xcb_screen->root,
						 0, 0, 640, 480,
						 10,
						 XCB_WINDOW_CLASS_INPUT_OUTPUT,
						 xcb_screen->root_visual,
						 value_mask, values);
  if((xcb_error = xcb_request_check(xcb_connection, xcb_request_cookie)))
  {
    xcb_error_code = xcb_error->error_code;
    free(xcb_error);
    xcb_error = 0;
    if(xcb_error_code != 0) fprintf(stderr, "window open failure\n"); goto finish;
  }
  xcb_request_cookie = xcb_map_window_checked(xcb_connection, window_id);
  if((xcb_error = xcb_request_check(xcb_connection, xcb_request_cookie)))
  {
    xcb_error_code = xcb_error->error_code;
    free(xcb_error);
    xcb_error = 0;
    if(xcb_error_code != 0) fprintf(stderr, "window map failure\n"); goto finish;
  }
  xcb_flush(xcb_connection);

  // NOTE: get opengl context
  {
    EGLBoolean egl_result = 1;

    PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT =
      (PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");
    PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC eglCreatePlatformWindowSurfaceEXT =
      (PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC)eglGetProcAddress("eglCreatePlatformWindowSurfaceEXT");
    if(eglGetPlatformDisplayEXT == 0 || eglCreatePlatformWindowSurfaceEXT == 0) goto finish;

    egl_display = eglGetPlatformDisplayEXT(EGL_PLATFORM_XCB_EXT, xcb_connection, 0);
    if(egl_display == EGL_NO_DISPLAY) goto finish;

    EGLint egl_major, egl_minor;
    egl_result = eglInitialize(egl_display, &egl_major, &egl_minor);
    if(!egl_result) goto finish;

    egl_result = eglBindAPI(EGL_OPENGL_API);
    if(!egl_result) goto finish;

    EGLint attrib_list[] = {EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT, EGL_NONE};
    EGLConfig egl_config;
    EGLint num_config;
    egl_result = eglChooseConfig(egl_display, attrib_list, &egl_config, 1, &num_config);
    if(!egl_result) goto finish;

    egl_surface = eglCreatePlatformWindowSurfaceEXT(egl_display, egl_config, &window_id, 0);
    egl_context = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, 0);
    if(egl_surface == EGL_NO_SURFACE || egl_context == EGL_NO_CONTEXT) goto finish;

    egl_result = eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context);
    if(!egl_result) goto finish;
  }

  // NOTE: main loop
  B32 running = 1;
  while(running)
  {
    // NOTE: event loop
    xcb_generic_event_t *xcb_event = 0;
    while((xcb_event = xcb_poll_for_event(xcb_connection)))
    {
      switch(xcb_event->response_type)
      {
	case XCB_EXPOSE:
	{
	  printf("xcb expose\n");
	  xcb_expose_event_t *expose_event = (xcb_expose_event_t*)xcb_event;

	  switch(xcb_expose)
	  {
	    case XcbExpose_put_shm_image:
	    {
	      xcb_request_cookie =
		xcb_shm_put_image_checked(xcb_connection,
					  window_id,
					  xcb_graphics_context,
					  xcb_screen->width_in_pixels,
					  xcb_screen->height_in_pixels,
					  0,
					  0,
					  xcb_screen->width_in_pixels,
					  xcb_screen->height_in_pixels,
					  0,
					  0,
					  xcb_screen->root_depth,
					  XCB_IMAGE_FORMAT_Z_PIXMAP,
					  0,
					  xcb_shm_segment,
					  0);
	      if((xcb_error = xcb_request_check(xcb_connection, xcb_request_cookie)))
	      {
		log_xcb_error(xcb_error, "put shm image failure");
		xcb_error = 0;
	        goto finish;
	      }
	    }break;

	    case XcbExpose_put_image:
	    {
	      xcb_request_cookie =
		xcb_put_image_checked(xcb_connection,
				      XCB_IMAGE_FORMAT_Z_PIXMAP,
				      window_id,
				      xcb_graphics_context,
				      1920, 1080,
				      0, 0,
				      0,
				      xcb_screen->root_depth,
				      screen_dim.width*screen_dim.height*sizeof(*backbuffer_pixels),
				      (U8*)backbuffer_pixels);
	      if((xcb_error = xcb_request_check(xcb_connection, xcb_request_cookie)))
	      {
		log_xcb_error(xcb_error, "put image failure");
		xcb_error = 0;
	        goto finish;
	      }
	    }break;

	    case XcbExpose_fill_rect:
	    {
	      xcb_rectangle_t rect = {
		.x = 0,
		.y = 0,
		.width = expose_event->width,
		.height = expose_event->height,
	      };
	      xcb_request_cookie =
		xcb_poly_fill_rectangle_checked(xcb_connection, window_id, xcb_graphics_context, 1, &rect);
	      if((xcb_error = xcb_request_check(xcb_connection, xcb_request_cookie)))
	      {
		log_xcb_error(xcb_error, "fill rectangle failure");
		xcb_error = 0;
		goto finish;
	      }
	    }break;
	  }

	  xcb_flush(xcb_connection);
	}break;

	case XCB_KEY_PRESS:
	{
	  xcb_key_press_event_t *key_press = (xcb_key_press_event_t*)xcb_event;
	  printf("key press: %u\n", key_press->detail);
	  running = 0;
	  // TODO: exit immediately?
	}break;

	case XCB_KEY_RELEASE:
	{
	  xcb_key_release_event_t *key_release = (xcb_key_release_event_t*)xcb_event;
	  printf("key release: %u\n", key_release->detail);
	}break;

	case XCB_BUTTON_PRESS:
	{
	  xcb_button_press_event_t *button_press = (xcb_button_press_event_t*)xcb_event;
	  printf("button press: %u\n", button_press->detail);
	}break;

	case XCB_BUTTON_RELEASE:
	{
	  xcb_button_release_event_t *button_release = (xcb_button_release_event_t*)xcb_event;
	  printf("button release: %u\n", button_release->detail);
	}break;

	case XCB_MOTION_NOTIFY:
	{
	  xcb_motion_notify_event_t *pointer_motion = (xcb_motion_notify_event_t*)xcb_event;
	  printf("pointer motion: (%d, %d)\n", pointer_motion->event_x, pointer_motion->event_y);
	}break;

	default:
	{
	  printf("unknown event\n");
	}break;
      }
      free(xcb_event);
    }
  }

finish:
  if(egl_surface != EGL_NO_SURFACE) eglDestroySurface(egl_display, egl_surface);
  if(egl_context != EGL_NO_CONTEXT) eglDestroyContext(egl_display, egl_context);
  if(egl_display != EGL_NO_DISPLAY) eglTerminate(egl_display);

  if(xcb_shm_segment) xcb_shm_detach(xcb_connection, xcb_shm_segment);
  if(shm) shmdt(shm);

  if(window_id != (U32)-1) xcb_unmap_window(xcb_connection, window_id);
  xcb_disconnect(xcb_connection);

  return(xcb_error_code);
}
