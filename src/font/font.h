#include "font/font_common.h"

#if OS_LINUX
#  include "font/FreeType/font_freetype.h"
#elif OS_WINDOWS
#  include "font/DirectWrite/font_directwrite.h"
#else
#  error unsupported platform
#endif
