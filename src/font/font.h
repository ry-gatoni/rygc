#include "font/font_common.h"

#define FONT_BACKEND_FREETYPE 1
#define FONT_BACKEND_DWRITE   2

#ifndef FONT_BACKEND
#  if OS_LINUX
#    define FONT_BACKEND FONT_BACKEND_FREETYPE
#  elif OS_WINDOWS
#    define FONT_BACKEND FONT_BACKEND_DWRITE
#  else
#    error platform not implemented
#  endif
#endif

#if FONT_BACKEND == FONT_BACKEND_FREETYPE
#  include "font/Freetype/font_freetype.h"
#elif FONT_BACKEND == FONT_BACKEND_DWRITE
#  include "font/DirectWrite/font_directwrite.h"
#else
#  error unrecognized font backend provided
#endif

/* #if OS_LINUX */
/* #  include "font/FreeType/font_freetype.h" */
/* #elif OS_WINDOWS */
/* #  include "font/DirectWrite/font_directwrite.h" */
/* #else */
/* #  error unsupported platform */
/* #endif */
