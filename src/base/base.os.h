#include "base.os.common.h"
#if OS_LINUX
#  include "base.os.linux.h"
#elif OS_WINDOWS
#  include "base.os.windows.h"
#elif OS_MAC
#  include "base.os.mac.h"
#else
#  error ERROR: base os layer not implemented for this os
#endif
