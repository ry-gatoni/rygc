#define OS_FEATURE_GFX
#include "base/base.h"
#include "os/os.h"

#include "base/base.c"
#include "os/os.c"

#pragma comment(lib, "user32.lib")

#if 0
global B32 running = 1;

proc LRESULT
w32_window_proc(HWND wnd, UINT msg, WPARAM w_param, LPARAM l_param)
{
  LRESULT result = 0;

  switch(msg) {
  case WM_DESTROY:
  case WM_CLOSE: {
    PostQuitMessage(0);
    running = 0;
  }break;
  case WM_SIZE: {
    
  }break;
  default: {
    result = DefWindowProcW(wnd, msg, w_param, l_param);
  }break;
  }

  return(result);
}
#endif

int
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
  int result = 0;
  ArenaTemp scratch = arena_get_scratch(0, 0);
#if 0
  const wchar_t CLASS_NAME[] = L"test-window-class";

  WNDCLASS wnd_class = {0};
  wnd_class.lpfnWndProc = w32_window_proc;
  wnd_class.hInstance = hInstance;
  wnd_class.lpszClassName = (LPCSTR)CLASS_NAME;
  RegisterClass(&wnd_class); 

  HWND w32_window =
    CreateWindowEx(0, (LPCSTR)CLASS_NAME, (LPCSTR)L"w32-window-test", WS_OVERLAPPEDWINDOW,
		   CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		   0, 0, hInstance, 0);
  ShowWindow(w32_window, nShowCmd);
#else
  if(os_gfx_init()) {

    // TODO: I think the window title unicode coversion isn't working correctly
    Os_Handle window = os_open_window(Str8Lit("w32-window-test"), 640, 480);
    if(window.handle) {
#endif
      B32 running = 1;
      Arena *frame_arena = arena_alloc();
      while(running) {
#if 1
	Os_EventList events = os_events(frame_arena);
	for(Os_Event *event = events.first; event; event = event->next) {
	  if(event->window.handle == window.handle) {
	    switch(event->kind)
	      {
	      case Os_EventKind_close:
		{
		  running = 0;
		}break;
	      }
	  }
	}
	arena_clear(frame_arena);
#else
	// NOTE: poll input
	MSG msg = {0};
	while(GetMessage(&msg, 0, 0, 0)) {
	  TranslateMessage(&msg);
	  DispatchMessage(&msg);
	}
#endif
      }
      os_close_window(window);
#if 1
    }  
  }
#endif
  
  arena_release_scratch(scratch);
  return(result);
}
