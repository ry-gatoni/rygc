proc B32
freetype_init(void)
{
  B32 result = 0;
  Arena *arena = arena_alloc();
  ft_state = arena_push_struct(arena, FreetypeState);
  if(ft_state) {
    ft_state->arena = arena;
    FT_Error init_result = FT_Init_FreeType(&ft_state->library);
    result = init_result == FT_Err_Ok;
  }
  return(result);
}

proc FreetypeFont
freetype_open_font(Arena *arena, String8 font_path)
{
  String8 file_contents = os_read_entire_file(arena, font_path);
  FT_Open_Args open_args = {0};
  open_args.flags = FT_OPEN_MEMORY;
  open_args.memory_base = file_contents.string;
  open_args.memory_size = (FT_Long)file_contents.count;

  FT_Face face = 0;
  FT_Error open_face_result = FT_Open_Face(ft_state->library, &open_args, 0, &face);
  
  FreetypeFont result = {0};
  result.face = (open_face_result == FT_Err_Ok) ? face : 0;
  return(result);
}

proc void
freetype_uninit(void)
{
  FT_Done_FreeType(ft_state->library);
  arena_release(ft_state->arena);
  ft_state = 0;
}

/* proc LooseFont */
/* font_parse(Arena *arena, String8 font_file_contents, U32 font_size_pt) */
proc LooseFont
font_parse(Arena *arena, String8 font_path, U32 font_size_pt)
{
  LooseFont result = {0};
#if 1
  if(freetype_init()) {
    FreetypeFont ft_font = freetype_open_font(arena, font_path);
    if(ft_font.face) {

      // NOTE: character mapping
      RangeU32 glyph_idx_rng = rangeu32_invalid();
      FT_UInt ft_glyph_idx;
      for(FT_ULong ft_char_idx = FT_Get_First_Char(ft_font.face, &ft_glyph_idx);
	  ft_glyph_idx != 0;
	  ft_char_idx = FT_Get_Next_Char(ft_font.face, ft_char_idx, &ft_glyph_idx)) {
	  
	glyph_idx_rng.min = Min(glyph_idx_rng.min, ft_glyph_idx);
	glyph_idx_rng.max = Max(glyph_idx_rng.max, ft_glyph_idx);

	CodepointIndex *cp_idx = arena_push_struct(arena, CodepointIndex);
	cp_idx->codepoint = ft_char_idx;
	cp_idx->index = ft_glyph_idx;
	SLLQueuePush(result.first_cp_index, result.last_cp_index, cp_idx);
      }

      result.glyph_idx_rng = glyph_idx_rng;
	
      // NOTE: sizing and scaling
      U32 dpi = 81;
      FT_Error ft_set_char_size_result = FT_Set_Char_Size(ft_font.face, 0, font_size_pt*64, dpi, dpi);

      if(ft_set_char_size_result == FT_Err_Ok) {
	  
	FT_Size ft_size = ft_font.face->size;
	result.ascender = ft_size->metrics.ascender >> 6;
	result.descender = ft_size->metrics.descender >> 6;
	S32 specified_height = ft_size->metrics.height >> 6;	  
	result.line_height = Max(result.ascender - result.descender, specified_height);

	// NOTE: render glyphs
	for EachRange(glyph_idx, U32, glyph_idx_rng) {
	      
	    FT_Error ft_load_glyph_result = FT_Load_Glyph(ft_font.face, glyph_idx, FT_LOAD_DEFAULT);
	    if(ft_load_glyph_result == FT_Err_Ok) {

	      FT_GlyphSlot ft_glyph = ft_font.face->glyph;
	      FT_Error ft_render_glyph_result = FT_Render_Glyph(ft_glyph, FT_RENDER_MODE_NORMAL);
	      if(ft_render_glyph_result == FT_Err_Ok) {

		// TODO: only allocate pixels for glyphs that have nonzero size!
		// TODO: extract hinting data
		FT_Bitmap ft_bitmap = ft_glyph->bitmap;
		LooseGlyph *glyph = arena_push_struct(arena, LooseGlyph);
		glyph->width = ft_bitmap.width;
		glyph->height = ft_bitmap.rows;
		glyph->stride = glyph->width;
		glyph->left_bearing = ft_glyph->bitmap_left;
		glyph->top_bearing = ft_glyph->bitmap_top;
		glyph->advance = ft_glyph->advance.x >> 6;
		glyph->bitmap = 0;

		if(glyph->width && glyph->height) {
		  glyph->bitmap = arena_push_array(arena, U8, glyph->width*glyph->height);

		  U8 *dest_row = glyph->bitmap + glyph->stride*(glyph->height-1);
		  U8 *src_row = ft_bitmap.buffer;
		  for(U32 row_idx = 0; row_idx < ft_bitmap.rows; ++row_idx) {		  
		    CopyArray(dest_row, src_row, U8, ft_bitmap.width);

		    dest_row -= glyph->stride;
		    src_row += ft_bitmap.pitch;
		  }
		}
		
		SLLQueuePush(result.first_glyph, result.last_glyph, glyph);
	      }
	    }
	  }
      }
    }
    
    freetype_uninit();
  }
#else
  // NOTE: library setup
  FT_Library ft_library = {0};
  FT_Error ft_init_result = FT_Init_FreeType(&ft_library);    
  if(ft_init_result == FT_Err_Ok) {

    // NOTE: create face
    FT_Open_Args ft_open_args = {0};
    ft_open_args.flags = FT_OPEN_MEMORY;
    ft_open_args.memory_base = font_file_contents.string;
    ft_open_args.memory_size = font_file_contents.count;

    FT_Face ft_face = {0};
    FT_Error ft_open_face_result = FT_Open_Face(ft_library, &ft_open_args, 0, &ft_face);
    if(ft_open_face_result == FT_Err_Ok) {

      // NOTE: character mapping
      RangeU32 glyph_idx_rng = rangeu32_invalid();
      FT_UInt ft_glyph_idx;
      for(FT_ULong ft_char_idx = FT_Get_First_Char(ft_face, &ft_glyph_idx);
	  ft_glyph_idx != 0;
	  ft_char_idx = FT_Get_Next_Char(ft_face, ft_char_idx, &ft_glyph_idx)) {
	  
	glyph_idx_rng.min = Min(glyph_idx_rng.min, ft_glyph_idx);
	glyph_idx_rng.max = Max(glyph_idx_rng.max, ft_glyph_idx);

	CodepointIndex *cp_idx = arena_push_struct(arena, CodepointIndex);
	cp_idx->codepoint = ft_char_idx;
	cp_idx->index = ft_glyph_idx;
	SLLQueuePush(result.first_cp_index, result.last_cp_index, cp_idx);
      }

      result.glyph_idx_rng = glyph_idx_rng;
	
      // NOTE: sizing and scaling
      U32 dpi = 81;
      FT_Error ft_set_char_size_result = FT_Set_Char_Size(ft_face, 0, font_size_pt*64, dpi, dpi);

      if(ft_set_char_size_result == FT_Err_Ok) {
	  
	FT_Size ft_size = ft_face->size;
	result.ascender = ft_size->metrics.ascender >> 6;
	result.descender = ft_size->metrics.descender >> 6;
	S32 specified_height = ft_size->metrics.height >> 6;	  
	result.line_height = Max(result.ascender - result.descender, specified_height);

	// NOTE: render glyphs
	for EachRange(glyph_idx, U32, glyph_idx_rng) {
	      
	    FT_Error ft_load_glyph_result = FT_Load_Glyph(ft_face, glyph_idx, FT_LOAD_DEFAULT);
	    if(ft_load_glyph_result == FT_Err_Ok) {

	      FT_GlyphSlot ft_glyph = ft_face->glyph;
	      FT_Error ft_render_glyph_result = FT_Render_Glyph(ft_glyph, FT_RENDER_MODE_NORMAL);
	      if(ft_render_glyph_result == FT_Err_Ok) {

		// TODO: only allocate pixels for glyphs that have nonzero size!
		// TODO: extract hinting data
		FT_Bitmap ft_bitmap = ft_glyph->bitmap;
		LooseGlyph *glyph = arena_push_struct(arena, LooseGlyph);
		glyph->width = ft_bitmap.width;
		glyph->height = ft_bitmap.rows;
		glyph->stride = glyph->width;
		glyph->left_bearing = ft_glyph->bitmap_left;
		glyph->top_bearing = ft_glyph->bitmap_top;
		glyph->advance = ft_glyph->advance.x >> 6;
		glyph->bitmap = 0;

		if(glyph->width && glyph->height) {
		  glyph->bitmap = arena_push_array(arena, U8, glyph->width*glyph->height);

		  U8 *dest_row = glyph->bitmap + glyph->stride*(glyph->height-1);
		  U8 *src_row = ft_bitmap.buffer;
		  for(U32 row_idx = 0; row_idx < ft_bitmap.rows; ++row_idx) {		  
		    CopyArray(dest_row, src_row, U8, ft_bitmap.width);

		    dest_row -= glyph->stride;
		    src_row += ft_bitmap.pitch;
		  }
		}
		
		SLLQueuePush(result.first_glyph, result.last_glyph, glyph);
	      }
	    }
	  }
      }
    }

    FT_Done_FreeType(ft_library);
  }
#endif

  return(result);
}
