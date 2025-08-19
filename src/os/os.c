#if OS_WINDOWS
#  include "core/os_core_win32.c"
#elif OS_LINUX
#  include "core/os_core_linux.c"
#elif OS_MAC
#  include "os_mac.c"
#else
#  error "ERROR: os not supported"
#endif

#if defined(OS_FEATURE_GFX)
#  include "os_gfx.c"
#  if OS_LINUX
#    include "gfx/linux/wayland.c"
#  elif OS_WINDOWS
#    include "gfx/windows/win32.c"
#  else
#    error platform not implemented
#  endif
#endif
