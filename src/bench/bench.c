#include "bench.common.c"

#if OS_LINUX
#  include "bench.linux.c"
#elif OS_MAC
#  include "bench.mac.c"
#elif OS_WINDOWS
#  error not implemented
#else
#  error unsupported os
#endif
