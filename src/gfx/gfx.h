#include "gfx.common.h"

#define GFX_BACKNED_NULL 0
#define GFX_BACKEND_WAYLAND 1
#define GFX_BACKEND_WIN32 2
#define GFX_BACKEND_COCOA 3

// NOTE: default gfx backends
#ifndef GFX_BACKEND
#  if OS_LINUX
#    define GFX_BACKEND GFX_BACKEND_WAYLAND
#  elif OS_WINDOWS
#    define GFX_BACKEND GFX_BACKEND_WIN32
#  elif OS_MAC
#    define GFX_BACKEND GFX_BACKEND_COCOA
#  else
#    define GFX_BACKEND GFX_BACKEND_NULL
#  endif
#endif

#if GFX_BACKEND == GFX_BACKEND_WAYLAND
#  include "gfx.wayland.h"
#elif GFX_BACKEND == GFX_BACKEND_WIN32
#  include "gfx.win32.h"
#elif GFX_BACKEND == GFX_BACKEND_COCOA
#  include "gfx.cocoa.h"
#else
#  error ERROR: unrecognized value for `GFX_BACKEND`
#endif

#define GFX_LAYER
