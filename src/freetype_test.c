#include "base/base.h"
#include "os/os.h"

//#include "wayland.h"

#include "freetype/freetype.h"

#include "base/base.c"
#include "os/os.c"

//#include "wayland.c"

typedef struct CodepointIndex CodepointIndex;
struct CodepointIndex
{
  CodepointIndex *next;
  U32 codepoint;
  U32 index;
};

typedef struct LooseGlyph LooseGlyph;
struct LooseGlyph
{
  LooseGlyph *next;
  S32 width;
  S32 height;
  S32 stride;
  U8 *bitmap;
};

typedef struct LooseFont
{
  CodepointIndex *first_cp_index;
  CodepointIndex *last_cp_index;
  
  LooseGlyph *first_glyph;
  LooseGlyph *last_glyph;
  
  RangeU32 glyph_idx_rng;
  
  S32 ascender;
  S32 descender;
  U32 line_height;
} LooseFont;

typedef struct LoadedBitmap
{
  S32 width;
  S32 height;
  S32 stride;
  U32 *pixels;
} LoadedBitmap;

typedef struct CodepointMapNode CodepointMapNode;
struct CodepointMapNode
{
  CodepointMapNode *next;
  U32 codepoint;
  U32 glyph_index;
};

typedef struct CodepointMapBucket
{
  CodepointMapNode *first;
  CodepointMapNode *last;
} CodepointMapBucket;

typedef struct LoadedFont
{
  S32 ascender;
  S32 descender;
  U32 line_height;

  U32 glyph_count;
  LoadedBitmap *glyphs;
  CodepointMapBucket *codepoint_map;
} LoadedFont;

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  LooseFont loose_font = {0};

  ArenaTemp scratch = arena_get_scratch(0, 0);
  String8 font_file_path = Str8Lit("/usr/share/fonts/liberation-mono-fonts/LiberationMono-Regular.ttf");
  String8 font_file = os_read_entire_file(scratch.arena, font_file_path);  
  if(font_file.count) {

    // NOTE: library setup
    FT_Library ft_library = {0};
    FT_Error ft_init_result = FT_Init_FreeType(&ft_library);    
    if(ft_init_result == FT_Err_Ok) {

      // NOTE: create face
      FT_Open_Args ft_open_args = {0};
      ft_open_args.flags = FT_OPEN_MEMORY;
      ft_open_args.memory_base = font_file.string;
      ft_open_args.memory_size = font_file.count;

      FT_Face ft_face = {0};
      FT_Error ft_open_face_result = FT_Open_Face(ft_library, &ft_open_args, 0, &ft_face);
      if(ft_open_face_result == FT_Err_Ok) {

	// NOTE: character mapping
	RangeU32 glyph_idx_rng = make_rng_u32_invalid();
	FT_UInt ft_glyph_idx;
	for(FT_ULong ft_char_idx = FT_Get_First_Char(ft_face, &ft_glyph_idx);
	    ft_glyph_idx != 0;
	    ft_char_idx = FT_Get_Next_Char(ft_face, ft_char_idx, &ft_glyph_idx)) {
	  
	  printf("(char index, glyph index): (%lu, %u)\n", ft_char_idx, ft_glyph_idx);
	  glyph_idx_rng.min = Min(glyph_idx_rng.min, ft_glyph_idx);
	  glyph_idx_rng.max = Max(glyph_idx_rng.max, ft_glyph_idx);

	  CodepointIndex *cp_idx = arena_push_struct(scratch.arena, CodepointIndex);
	  cp_idx->codepoint = ft_char_idx;
	  cp_idx->index = ft_glyph_idx;
	  SLLQueuePush(loose_font.first_cp_index, loose_font.last_cp_index, cp_idx);
	}

	printf("glyph index (min, max): (%u, %u)\n", glyph_idx_rng.min, glyph_idx_rng.max);
	loose_font.glyph_idx_rng = glyph_idx_rng;
	
	// NOTE: sizing and scaling
	U32 font_size_pt = 16;
	U32 dpi = 81;
	FT_Error ft_set_char_size_result = FT_Set_Char_Size(ft_face, 0, font_size_pt*64, dpi, dpi);

	if(ft_set_char_size_result == FT_Err_Ok) {
	  
	  FT_Size ft_size = ft_face->size;
	  loose_font.ascender = ft_size->metrics.ascender >> 6;
	  loose_font.descender = ft_size->metrics.descender >> 6;
	  S32 specified_height = ft_size->metrics.height >> 6;	  
	  loose_font.line_height = Max(loose_font.ascender - loose_font.descender, specified_height);

	  // NOTE: render glyphs
	  for EachRange(glyph_idx, U32, glyph_idx_rng) {
	      
	    FT_Error ft_load_glyph_result = FT_Load_Glyph(ft_face, glyph_idx, FT_LOAD_DEFAULT);
	    if(ft_load_glyph_result == FT_Err_Ok) {

	      FT_GlyphSlot ft_glyph = ft_face->glyph;
	      FT_Error ft_render_glyph_result = FT_Render_Glyph(ft_glyph, FT_RENDER_MODE_NORMAL);
	      if(ft_render_glyph_result == FT_Err_Ok) {

		FT_Bitmap ft_bitmap = ft_glyph->bitmap;
		LooseGlyph *glyph = arena_push_struct(scratch.arena, LooseGlyph);
		glyph->width = ft_bitmap.width;
		glyph->height = ft_bitmap.rows;
		glyph->stride = glyph->width;
		glyph->bitmap = arena_push_array(scratch.arena, U8, glyph->width*glyph->height);

		U8 *dest_row = glyph->bitmap, *src_row = ft_bitmap.buffer;
		for(U32 row_idx = 0; row_idx < ft_bitmap.rows; ++row_idx) {		  
		  CopyArray(dest_row, src_row, U8, ft_bitmap.width);
		  dest_row += glyph->stride;
		  src_row += ft_bitmap.pitch;
		}

		SLLQueuePush(loose_font.first_glyph, loose_font.last_glyph, glyph);
	      }
	    }
	  }
	}
      }

      FT_Done_FreeType(ft_library);
    }
  }

  arena_release_scratch(scratch);
  return(0);
}
