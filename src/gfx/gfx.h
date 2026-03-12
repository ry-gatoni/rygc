#include "gfx.common.h"

#define GFX_BACKNED_NULL 0
#define GFX_BACKEND_WAYLAND 1
#define GFX_BACKEND_WIN32 2

// NOTE: default gfx backends
#ifndef GFX_BACKEND
#  if OS_LINUX
#    define GFX_BACKEND GFX_BACKEND_WAYLAND
#  elif OS_WINDOWS
#    define GFX_BACKEND GFX_BACKEND_WIN32
#  elif OS_MAC
#    error TODO: mac
#  else
#    define GFX_BACKEND GFX_BACKEND_NULL
#  endif
#endif

#if GFX_BACKEND == GFX_BACKEND_WAYLAND
#  include "gfx.wayland.h"
#elif GFX_BACKEND == GFX_BACKEND_WIN32
#  include "gfx.win32.h"
#else
#  error ERROR: unrecognized value for `GFX_BACKEND`
#endif
