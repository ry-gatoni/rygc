#if OS_LINUX
#  include "base.os.linux.c"
#elif OS_WINDOWS
#  include "base.os.windows.c"
#elif OS_MAC
#  include "base.os.mac.c"
#else
#  error ERROR: base os layer not implemented for this os
#endif
