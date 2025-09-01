#include "freetype/freetype.h"

#if OS_WINDOWS
#  pragma comment(lib, "freetype.lib")
#endif

typedef struct FreetypeFont
{
  FT_Face face;
} FreetypeFont;

typedef struct FreetypeState
{
  Arena *arena;
  FT_Library library;
} FreetypeState;

global FreetypeState *ft_state = 0;

proc B32 freetype_init(void);
proc void freetype_uninit(void);

proc FreetypeFont freetype_open_font(Arena *arena, String8 font_path);

// TODO: pull out into font_common (align with directwrite)
//proc LooseFont font_parse(Arena *arena, String8 font_file_contents, U32 font_size_pt);
proc LooseFont font_parse(Arena *arena, String8 font_path, U32 font_size_pt);


