#include "bench.common.c"

#if OS_LINUX
#  include "bench.linux.c"
#elif OS_MAC
#  error not implemented
#elif OS_WINDOWS
#  error not implemented
#else
#  error unsupported os
#endif
