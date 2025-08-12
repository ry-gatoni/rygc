#include "os_core.h"

#if defined(OS_FEATURE_GFX)
#  include "os_gfx.h"
#  if OS_LINUX
#    include "gfx/linux/wayland.h"
#  endif
#endif
/* #if OS_WINDOWS */
/* #  include "os_windows.h" */
/* #elif OS_LINUX */
/* #  include "os_linux.h" */
/* #elif OS_MAC */
/* #  include "os_mac.h" */
/* #else */
/* #  error "ERROR: os not supported" */
/* #endif */
