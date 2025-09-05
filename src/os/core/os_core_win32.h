#pragma warning(push)
#pragma warning(disable: 4042)
#include <windows.h>
#pragma warning(pop)
/* #pragma comment(lib, "user32") */
/* #pragma comment(lib, "winmm") */
/* #pragma comment(lib, "gdi32") */

typedef struct Win32_ThreadInfo Win32_ThreadInfo;
struct Win32_ThreadInfo
{
  Win32_ThreadInfo *next;
  Os_ThreadProc *procedure;
  void *data;
  HANDLE handle;
  DWORD id;
};

typedef struct Win32_State
{
  Arena *arena;

  SYSTEM_INFO sys_info;

  Win32_ThreadInfo *first_thread_info;
  Win32_ThreadInfo *last_thread_info;
  U64 thread_info_count;
  Win32_ThreadInfo *thread_info_freelist;
} Win32_State;

global Win32_State *w32_state = 0;

proc B32 win32_init(void);
proc Win32_ThreadInfo* win32_alloc_thread_info(void);

proc DWORD WINAPI w32_thread_entry_point(void *param);

#define Win32LogError(err) do { char *msg = 0; FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM, 0, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&msg, 0, 0); OutputDebugStringA(msg); } while(0)
