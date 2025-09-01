#include "font/font_common.c"

#if FONT_BACKEND == FONT_BACKEND_FREETYPE
#  include "font/FreeType/font_freetype.c"
#elif FONT_BACKEND == FONT_BACKEND_DWRITE
#  include "font/DirectWrite/font_directwrite.c"
#else
#  error unsupported platform
#endif

/* #if OS_LINUX */
/* #  include "font/FreeType/font_freetype.c" */
/* #elif OS_WINDOWS */
/* #  include "font/DirectWrite/font_directwrite.c" */
/* #else */
/* #  error unsupported platform */
/* #endif */
