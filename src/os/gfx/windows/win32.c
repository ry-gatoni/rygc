global Win32State *w32_state = 0;
global wchar_t w32_class_name[] = L"os-gfx-win32-class";

proc LRESULT
win32_window_proc(HWND wnd, UINT msg, WPARAM w_param, LPARAM l_param)
{
  // TODO: implement event handling
  LRESULT result = 0;
  switch(msg) {    
  default: {
    result = DefWindowProcW(wnd, msg, w_param, l_param);
  }break;
  }
  return(result);
}

proc B32
win32_init(void)
{
  Arena *arena = arena_alloc();
  w32_state = arena_push_struct(arena, Win32State);
  w32_state->arena = arena;
  w32_state->h_instance = GetModuleHandle(0);

  // NOTE: register window class
  {
    WNDCLASS wnd_class = {0};
    wnd_class.lpfnWndProc = win32_window_proc;
    wnd_class.hInstance = w32_state->h_instance;
    wnd_class.lpszClassName = (LPCSTR)w32_class_name;
    RegisterClass(&wnd_class);
  }

  // TODO: opengl initialization

  return(w32_state != 0);
}

proc Win32Window*
win32_alloc_window(void)
{
  Win32Window *result = 0;
  if(w32_state) {
    if(w32_state->window_freelist) {
      result = w32_state->window_freelist;
      SLLStackPop(w32_state->window_freelist);
    }else {
      result = arena_push_struct(w32_state->arena, Win32Window);
    }
    if(result) {
      DLLPushBack(w32_state->first_window, w32_state->last_window, result);
    }
  }
  return(result);
}

proc void
win32_destroy_window(Win32Window *window)
{
  if(w32_state) {
    DLLRemove(w32_state->first_window, w32_state->last_window, window);
    SLLStackPush(w32_state->window_freelist, window);
  }
}

proc Win32Window*
win32_open_window(String8 name, S32 width, S32 height)
{
  Win32Window *window = win32_alloc_window();
  if(window) {
    ArenaTemp scratch = arena_get_scratch(0, 0);
    String16 name_w = str16_from_str8(scratch.arena, name);
    window->name = name;
    window->dim = v2s32(width, height);
    window->h_wnd =
      CreateWindowEx(0, (LPCSTR)w32_class_name, (LPCSTR)name_w.string, WS_OVERLAPPEDWINDOW,
		     CW_USEDEFAULT, CW_USEDEFAULT, width, height,
		     0, 0, w32_state->h_instance, 0);
    ShowWindow(window->h_wnd, SW_SHOW);
    arena_release_scratch(scratch);    
  }
  return(window);
}

proc B32
win32_end_frame(Win32Window *window)
{
  // TODO: implement
  return(0);
}

proc void
win32_close_window(Win32Window *window)
{
  DestroyWindow(window->h_wnd);  
  win32_destroy_window(window);
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
  // TODO: implement
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
  // TODO: implement
  Unused(window);
}

proc void
os_window_end_frame(Os_Handle window)
{
  // TODO: implement
  Unused(window);
}

