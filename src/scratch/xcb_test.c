#include <EGL/egl.h>
//#define EGL_EGLEXT_PROTOTYPES
#include <EGL/eglext.h>

#include "base/base.h"
#include "gfx/gfx.common.h"
#include "gfx/gfx.xcb.h"

#include "base/base.c"
#include "gfx/gfx.common.c"
#include "gfx/gfx.xcb.c"

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

  if(!xcb_init()) return(1);

  Xcb_Window *window = xcb_window_open(v2s32(640, 480));
  xcb_select_backend(window, Xcb_Backend_shm);

  Arena *frame_arena = arena_alloc();
  B32 running = 1;
  while(running)
  {
    Os_EventList events = xcb_events(frame_arena);
    for(Os_Event *event = events.first; event; event = event->next)
    {
      if(xcb__window_from_gfx_handle(event->window) != window) continue;

      switch(event->kind)
      {
	case Os_EventKind_press:
	{
	  if(event->key == Os_Key_mouse_right) running = 0;
	}break;

	default: break;
      }
    }

    arena_clear(frame_arena);
  }

  xcb_window_close(window);

  xcb_uninit();

  return(0);

#if 0
  // NOTE: persistent resources that should be released at program end
  int xcb_error_code = 0;
  /* xcb_generic_error_t *xcb_error = 0; */
  /* xcb_void_cookie_t xcb_request_cookie = {0}; */

  xcb_window_t window_id = (U32)-1;

  xcb_shm_seg_t xcb_shm_segment = {0};
  U8 *shm = 0;

  EGLDisplay egl_display = EGL_NO_DISPLAY;
  EGLSurface egl_surface = EGL_NO_SURFACE;
  EGLContext egl_context = EGL_NO_CONTEXT;

  Arena *xcb_arena = arena_alloc();

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
      xcb_log_error(xcb_error, "create shm failure");
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
      xcb_log_error(xcb_error, "create shm pixmap failure");
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
      xcb_log_error(xcb_error, "create graphics context failure");
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
		xcb_log_error(xcb_error, "put shm image failure");
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
		xcb_log_error(xcb_error, "put image failure");
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
		xcb_log_error(xcb_error, "fill rectangle failure");
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
#endif
}
