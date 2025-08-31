#include <windowsx.h>
#pragma comment(lib, "user32.lib")

typedef struct Win32_Window Win32_Window;
struct Win32_Window
{
  Win32_Window *next;
  Win32_Window *prev;
  String8 name;
  V2S32 dim;
  HWND hwnd;
  HDC dc;
};

typedef struct Win32_GfxState
{
  Arena *arena;

  Win32_Window *first_window;
  Win32_Window *last_window;
  U64 window_count;
  Win32_Window *window_freelist;

  HINSTANCE h_instance;

  int pf; // NOTE: pixel format
  HGLRC gfx_ctxt;
} Win32_GfxState;

// NOTE: internal functions
proc LRESULT win32_window_proc(HWND wnd, UINT msg, WPARAM w_param, LPARAM l_param);
proc Win32_Window* win32_alloc_window(void);
proc void win32_destroy_window(Win32_Window *window);
proc Win32_Window* win32_window_from_hwnd(HWND hwnd);
proc Os_Handle win32_handle_from_window(Win32_Window *window);
proc Os_Event* win32_push_event(Os_EventKind kind, Os_Handle window);

proc B32 win32_gfx_init(void);
proc Win32_Window* win32_open_window(String8 name, S32 width, S32 height);
proc B32 win32_begin_frame(Win32_Window *window);
proc B32 win32_end_frame(Win32_Window *window);
proc void win32_close_window(Win32_Window *window);
