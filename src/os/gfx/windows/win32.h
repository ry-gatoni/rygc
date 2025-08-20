#include <windowsx.h>

typedef struct Win32Window Win32Window;
struct Win32Window
{
  Win32Window *next;
  Win32Window *prev;
  String8 name;
  V2S32 dim;
  HWND hwnd;
};

typedef struct Win32State
{
  Arena *arena;

  Win32Window *first_window;
  Win32Window *last_window;
  U64 window_count;
  Win32Window *window_freelist;

  HINSTANCE h_instance;
} Win32State;

// NOTE: internal functions
proc LRESULT win32_window_proc(HWND wnd, UINT msg, WPARAM w_param, LPARAM l_param);
proc Win32Window* win32_alloc_window(void);
proc void win32_destroy_window(Win32Window *window);
proc Win32Window* win32_window_from_hwnd(HWND hwnd);
proc Os_Handle win32_handle_from_window(Win32Window *window);
proc Os_Event* win32_push_event(Os_EventKind kind, Os_Handle window);

proc B32 win32_init(void);
proc Win32Window* win32_open_window(String8 name, S32 width, S32 height);
proc B32 win32_end_frame(Win32Window *window);
proc void win32_close_window(Win32Window *window);
