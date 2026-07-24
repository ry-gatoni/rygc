#include "font/font_common.c"

#if FONT_BACKEND == FONT_BACKEND_FREETYPE
#  include "font/FreeType/font_freetype.c"
#elif FONT_BACKEND == FONT_BACKEND_DWRITE
#  include "font/DirectWrite/font_directwrite.c"
#elif FONT_BACKEND == FONT_BACKEND_CORE_TEXT
#  include "font.core_text.c"
#else
#  error unsupported platform
#endif
