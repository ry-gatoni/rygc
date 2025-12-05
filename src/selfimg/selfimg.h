#include "selfimg.common.h"

#if OS_LINUX
#  include "selfimg.elf.h"
#elif OS_WINDOWS
#  include "selfimg.pe.h"
#elif OS_MAC
#  error TODO: mac/mach-o
#else
#  error ERROR: unsupported os
#endif
