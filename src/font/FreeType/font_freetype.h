#include "freetype/freetype.h"

// TODO: pull out into font_common (align with directwrite)
proc LooseFont font_parse(Arena *arena, String8 font_file_contents, U32 font_size_pt);
