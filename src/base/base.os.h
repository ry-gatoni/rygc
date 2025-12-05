#include "base.os.common.h"
#if OS_LINUX
#  include "base.os.linux.h"
#elif OS_WINDOWS
#  include "base.os.windows.h"
#else
#  error TODO: mac
#endif
