#include "gfx.common.c"

#if GFX_BACKEND == GFX_BACKEND_WAYLAND
#  include "gfx.wayland.c"
#elif GFX_BACKEND == GFX_BACKEND_WIN32
#  include "gfx.win32.c"
#elif GFX_BACKEND == GFX_BACKEND_COCOA
#  include "gfx.cocoa.c"
#elif GFX_BACKEND == GFX_BACKEND_XCB
#  include "gfx.xcb.c"
#else
#  error ERROR: unsupported gfx backend
#endif
