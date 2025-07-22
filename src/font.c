proc LoadedFont
font_pack(Arena *arena, LooseFont *loose_font)
{
  LoadedFont result = {0};
  result.ascender = loose_font->ascender;
  result.descender = loose_font->descender;
  result.line_height = loose_font->line_height;
  result.glyph_count = rng_u32_len(loose_font->glyph_idx_rng);
  result.glyphs = arena_push_array(arena, LoadedBitmap, result.glyph_count);
  result.codepoint_map_count = result.glyph_count*2;
  result.codepoint_map = arena_push_array(arena, CodepointMapBucket, result.codepoint_map_count);

  LooseGlyph *loose_glyph = loose_font->first_glyph;
  CodepointIndex *loose_cp_idx = loose_font->first_cp_index;
  for(U32 glyph_index = 0;
      loose_glyph != 0 && loose_cp_idx != 0;
      loose_glyph = loose_glyph->next, loose_cp_idx = loose_cp_idx->next, ++glyph_index) {

    // NOTE: glyph
    LoadedBitmap *packed_glyph = result.glyphs + glyph_index;
    packed_glyph->width = loose_glyph->width;
    packed_glyph->height = loose_glyph->height;
    packed_glyph->stride = packed_glyph->width*sizeof(U32);
    packed_glyph->pixels = arena_push_array(arena, U32, packed_glyph->width*packed_glyph->height);

    // NOTE: copy pixels
    U8 *dest_row = (U8*)packed_glyph->pixels, *src_row = loose_glyph->bitmap;
    for(S32 j = 0; j < packed_glyph->height; ++j) {

      U8 *src_pixels = src_row;
      U32 *dest_pixels = (U32*)dest_row;
      for(S32 i = 0; i < packed_glyph->width; ++i) {

	U8 src_pixel = *src_pixels;
	U32 dest_pixel = ((src_pixel << 3*8) |
			  (src_pixel << 2*8) |
			  (src_pixel << 1*8) |
			  (0xFF      << 0*8));
	
	*dest_pixels++ = dest_pixel;
	++src_pixels;
      }

      dest_row += packed_glyph->stride;
      src_row += loose_glyph->stride;
    }

    // NOTE: codepoint -> glyph index map
    U32 cp_map_idx = loose_cp_idx->codepoint % result.codepoint_map_count;
    CodepointMapBucket *cp_map_bucket = result.codepoint_map + cp_map_idx;
    
    CodepointMapNode *cp_map_node = arena_push_struct(arena, CodepointMapNode);
    cp_map_node->codepoint = loose_cp_idx->codepoint;
    cp_map_node->glyph_index = glyph_index;
    
    SLLQueuePush(cp_map_bucket->first, cp_map_bucket->last, cp_map_node);
  }

  return(result);
}
