#if OS_LINUX
#  include "selfimg.elf.c"
#elif OS_WINDOWS
#  include "selfimg.pe.c"
#elif OS_MAC
#  error TODO: mac/mach-o
#else
#  error ERROR: unsupported os
#endif
