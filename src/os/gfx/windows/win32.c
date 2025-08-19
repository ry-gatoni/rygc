global Win32State *w32_state = 0;

proc B32
win32_init(void)
{
  Arena *arena = arena_alloc();
  w32_state = arena_push_struct(arena, Win32State);
  return(w32_state != 0);
}

proc Win32Window*
win32_open_window(String8 name, S32 width, S32 height)
{
  return(0);
}

proc B32
win32_end_frame(Win32Window *window)
{
  return(0);
}

proc void
win32_close_window(Win32Window *window)
{
  Unused(window);
}

// NOTE: public api
proc B32
os_gfx_init(void)
{
  B32 result = win32_init();
  return(result);
}

proc Os_Handle
os_open_window(String8 name, S32 width, S32 height)
{
  Win32Window *window = win32_open_window(name, width, height);
  Os_Handle result = {0};
  result.handle = window;
  return(result);
}

proc void
os_close_window(Os_Handle window)
{
  win32_close_window(window.handle);
}

proc Os_EventList
os_events_from_window(Os_Handle window)
{
  Unused(window);
  Os_EventList result = {0};
  return(result);
}

proc V2S32
os_window_get_dim(Os_Handle window)
{
  Win32Window *w32_window = window.handle;
  V2S32 result = w32_window->dim;
  return(result);
}

// TODO: move to render module
proc void
os_window_begin_frame(Os_Handle window)
{
  Unused(window);
}

proc void
os_window_end_frame(Os_Handle window)
{
  Unused(window);
}

