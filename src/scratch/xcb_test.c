#include "base/base.h"

#include <xcb/xcb.h>

#include "base/base.c"

// TODO:
// - opengl/vulkan context
// - read keysyms/keycodes
// - gracefully handle window close
// - handle window resize

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  int xcb_error_code = 0;
  xcb_generic_error_t *xcb_error = 0;
  xcb_void_cookie_t xcb_request_cookie = {0};
  xcb_window_t window_id = (U32)-1;

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
  //V2S32 screen_dim = { .width = xcb_screen->width_in_pixels, .height = xcb_screen->height_in_pixels};

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
      xcb_error_code = xcb_error->error_code;
      free(xcb_error);
      xcb_error = 0;
      if(xcb_error_code != 0) fprintf(stderr, "create graphics context failure\n"); goto finish;
    }
  }

  // NOTE: open window
  U32 value_mask = XCB_CW_OVERRIDE_REDIRECT|XCB_CW_EVENT_MASK;
  U32 values[] = {
    0,
    /* xcb_screen->white_pixel, */
    XCB_EVENT_MASK_EXPOSURE|XCB_EVENT_MASK_KEY_PRESS,
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
	  xcb_rectangle_t rect = { .x = 0, .y = 0, .width = 640, .height = 480, };
	  xcb_request_cookie = xcb_poly_fill_rectangle_checked(xcb_connection,
							       window_id,
							       xcb_graphics_context,
							       1, &rect);
	  if((xcb_error = xcb_request_check(xcb_connection, xcb_request_cookie)))
	  {
	    xcb_error_code = xcb_error->error_code;
	    free(xcb_error);
	    xcb_error = 0;
	    if(xcb_error_code != 0) fprintf(stderr, "fill rectangle failure\n"); goto finish;
	  }
	  xcb_flush(xcb_connection);
	}break;
	case XCB_KEY_PRESS:
	{
	  printf("key press\n");
	  running = 0;
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
  if(window_id != (U32)-1) xcb_unmap_window(xcb_connection, window_id);
  xcb_disconnect(xcb_connection);
  return(xcb_error_code);
}
