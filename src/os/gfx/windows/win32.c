global Win32State *w32_state = 0;
global wchar_t w32_class_name[] = L"os-gfx-win32-class";
global Os_EventList *os_event_list = 0;
global Arena *event_arena = 0;

proc LRESULT
win32_window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  // TODO: implement event handling
  LRESULT result = 0;
  if(os_event_list != 0 && event_arena != 0) {
    Win32Window *w32_window = win32_window_from_hwnd(hwnd);
    Os_Handle window = win32_handle_from_window(w32_window);
    B32 release = 0;
    switch(msg)
      {
	// TODO: resize
      /* case WM_SIZE: */
      /* 	{ */
	
      /* 	}break; */
      case WM_CLOSE:
	{
	  win32_push_event(Os_EventKind_close, window);
	}break;
	// NOTE: mouse events
      case WM_LBUTTONUP:
      case WM_MBUTTONUP:
      case WM_RBUTTONUP:
	{
	  release = 1;
	}
      case WM_LBUTTONDOWN:
      case WM_MBUTTONDOWN:
      case WM_RBUTTONDOWN:
	{
	  Os_Event *event = win32_push_event(release ? Os_EventKind_release : Os_EventKind_press, window);
	  switch(msg)
	    {
	    case WM_LBUTTONUP:
	    case WM_LBUTTONDOWN:
	      {
		event->key = Os_Key_mouse_left;
	      }break;
	    case WM_MBUTTONUP:
	    case WM_MBUTTONDOWN:
	      {
		event->key = Os_Key_mouse_middle;
	      }break;
	    case WM_RBUTTONUP:
	    case WM_RBUTTONDOWN:
	      {
		event->key = Os_Key_mouse_right;
	      }break;
	    }
	  event->pos = v2((R32)GET_X_LPARAM(lparam), (R32)GET_Y_LPARAM(lparam));
	}break;
      case WM_MOUSEMOVE:
	{
	  Os_Event *event = win32_push_event(Os_EventKind_move, window);
	  event->pos = v2((R32)GET_X_LPARAM(lparam), (R32)GET_Y_LPARAM(lparam));
	}break;
      case WM_MOUSEWHEEL:
      case WM_MOUSEHWHEEL:
	{
	  Os_Event *event = win32_push_event(Os_EventKind_scroll, window);
	  R32 delta = (R32)GET_WHEEL_DELTA_WPARAM(wparam);
	  event->pos = v2((R32)GET_X_LPARAM(lparam), (R32)GET_Y_LPARAM(lparam));
	  event->scroll = msg == WM_MOUSEWHEEL ? v2(0, -delta) : v2(delta, 0);
	}break;
	// NOTE: keyboard events
      case WM_KEYDOWN:
      case WM_KEYUP:
	{
	  release = msg == WM_KEYUP;
	  Os_Event *event = win32_push_event(release ? Os_EventKind_press : Os_EventKind_release, window);
	  // TOOD: get key
	}break;
      default:
	{
	  result = DefWindowProcW(hwnd, msg, wparam, lparam);
	}break;
    }
  }else {
    result = DefWindowProcW(hwnd, msg, wparam, lparam);
  }
  return(result);
}

proc Win32Window*
win32_window_from_hwnd(HWND hwnd)
{
  // TODO: accelerate this lookup?
  Win32Window *result = 0;
  for(Win32Window *w = w32_state->first_window; w; w = w->next) {
    if(w->hwnd == hwnd) {
      result = w;
      break;
    }
  }
  return(result);
}

proc Os_Handle
win32_handle_from_window(Win32Window *window)
{
  Os_Handle result = {0};
  result.handle = window;
  return(result);
}

proc Os_Event*
win32_push_event(Os_EventKind kind, Os_Handle window)
{
  Os_Event *event = os_event_list_push_new(event_arena, os_event_list, kind);
  event->window = window;
  return(event);
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
    window->hwnd =
      CreateWindowEx(0, (LPCSTR)w32_class_name, (LPCSTR)name_w.string, WS_OVERLAPPEDWINDOW,
		     CW_USEDEFAULT, CW_USEDEFAULT, width, height,
		     0, 0, w32_state->h_instance, 0);
    ShowWindow(window->hwnd, SW_SHOW);
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
  DestroyWindow(window->hwnd);  
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
  Unused(window);
  Os_EventList result = {0};
  return(result);
}

proc Os_EventList
os_events(Arena *arena)
{
  Os_EventList result = {0};
  os_event_list = &result;
  event_arena = arena;

  MSG msg = {0};
  while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {    
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
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
