#include "font/font_common.c"

#if OS_LINUX
#  include "font/FreeType/font_freetype.c"
#elif OS_WINDOWS
#  include "font/DirectWrite/font_directwrite.c"
#else
#  error unsupported platform
#endif
