#include "gfx.wayland.common.c"

/* #if WAYLAND_BACKEND == WAYLAND_BACKEND_OPENGL */
/* #  error test */
/* #endif */

#if WAYLAND_BACKEND & WAYLAND_BACKEND_SOFTWARE
#  error TODO
#endif
#if WAYLAND_BACKEND & WAYLAND_BACKEND_OPENGL
#  include "gfx.wayland.ogl.c"
#endif
#if WAYLAND_BACKEND & WAYLAND_BACKEND_VULKAN
#  error TODO
#endif
#if (WAYLAND_BACKEND & (WAYLAND_BACKEND_SOFTWARE|WAYLAND_BACKEND_OPENGL|WAYLAND_BACKEND_VULKAN)) == 0
#  error ERROR: unrecognized value for `WAYLAND_BACKEND`
#endif

proc B32
gfx_init(void)
{
  B32 result = wayland_init();
  if(!result)
  {
    wayland_uninit();
    return(result);
  }
#if (WAYLAND_BACKEND & WAYLAND_BACKEND_SOFTWARE) == 1
  result = wayland_software_init();
  if(!result)
  {
    wayland_uninit();
    return(result);
  }
#endif
#if (WAYLAND_BACKEND & WAYLAND_BACKEND_OPENGL) == 1
  result = wayland_ogl_init();
  if(!result)
  {
    wayland_uninit();
    return(result);
  }
#endif
#if (WAYLAND_BACKEND & WAYLAND_BACKEND_VULKAN) == 1
  result = wayland_vulkan_init();
  if(!result)
  {
    wayland_uninit();
    return(result);
  }
#endif
  return(result);
}

proc void
gfx_uninit()
{
  wayland_uninit();
}

proc Os_Handle
gfx_window_open(String8 name, S32 width, S32 height)
{
  Wayland_Window *window = wayland_window_open(name, width, height);
#if (WAYLAND_BACKEND & WAYLAND_BACKEND_SOFTWARE) == 1
  window->software_backend = wayland_software_window_open(window);
#endif
#if (WAYLAND_BACKEND & WAYLAND_BACKEND_OPENGL) == 1
  window->ogl_backend = wayland_ogl_window_open(window);
#endif
#if (WAYLAND_BACKEND & WAYLAND_BACKEND_VULKAN) == 1
  window->vulkan_backend = wayland_vulkan_window_open(window);
#endif
  Os_Handle result = wayland__os_handle_from_window(window);
  return(result);
}

proc void
gfx_window_close(Os_Handle window)
{
  Wayland_Window *wayland_window = wayland__window_from_os_handle(window);
#if (WAYLAND_BACKEND & WAYLAND_BACKEND_SOFTWARE) == 1
  wayland_software_window_close(wayland_window);
#endif
#if (WAYLAND_BACKEND & WAYLAND_BACKEND_OPENGL) == 1
  wayland_ogl_window_close(wayland_window);
#endif
#if (WAYLAND_BACKEND & WAYLAND_BACKEND_VULKAN) == 1
  wayland_vulkan_window_close(wayland_window);
#endif
  wayland_window_close(wayland_window);
}

proc Os_EventList
gfx_events(Arena *arena)
{
  Os_EventList events = wayland_events(arena);
  return(events);
}

proc V2S32
gfx_window_get_dim(Os_Handle window)
{
  Wayland_Window *wayland_window = wayland__window_from_os_handle(window);
  V2S32 result = wayland_window_get_dim(wayland_window);
  return(result);
}

proc void
gfx_window_begin_frame(Os_Handle window)
{
  Wayland_Window *wayland_window = wayland__window_from_os_handle(window);
  wayland_window_begin_frame(wayland_window);
#if (WAYLAND_BACKEND & WAYLAND_BACKEND_SOFTWARE) == 1
  wayland_software_window_begin_frame(wayland_window);
#endif
#if (WAYLAND_BACKEND & WAYLAND_BACKEND_OPENGL) == 1
  wayland_ogl_window_begin_frame(wayland_window);
#endif
#if (WAYLAND_BACKEND & WAYLAND_BACKEND_VULKAN) == 1
  wayland_vulkan_window_begin_frame(wayland_window);
#endif
}

proc void
gfx_window_end_frame(Os_Handle window)
{
  Wayland_Window *wayland_window = wayland__window_from_os_handle(window);
  wayland_window_end_frame(wayland_window);
#if (WAYLAND_BACKEND & WAYLAND_BACKEND_SOFTWARE) == 1
  wayland_software_window_end_frame(wayland_window);
#endif
#if (WAYLAND_BACKEND & WAYLAND_BACKEND_OPENGL) == 1
  wayland_ogl_window_end_frame(wayland_window);
#endif
#if (WAYLAND_BACKEND & WAYLAND_BACKEND_VULKAN) == 1
  wayland_vulkan_window_end_frame(wayland_window);
#endif
}
