proc LoadedFont
font_pack(Arena *arena, LooseFont *loose_font)
{
  LoadedFont result = {0};
  result.ascender = loose_font->ascender;
  result.descender = loose_font->descender;
  result.line_height = loose_font->line_height;
  result.glyph_count = rng_u32_len(loose_font->glyph_idx_rng);
  result.glyphs = arena_push_array(arena, LoadedGlyph, result.glyph_count);
  result.codepoint_map_count = result.glyph_count*2;
  result.codepoint_map = arena_push_array(arena, CodepointMapBucket, result.codepoint_map_count);

  LooseGlyph *loose_glyph = loose_font->first_glyph;
  CodepointIndex *loose_cp_idx = loose_font->first_cp_index;
  for(U32 glyph_index = 0;
      loose_glyph != 0 && loose_cp_idx != 0;
      loose_glyph = loose_glyph->next, loose_cp_idx = loose_cp_idx->next, ++glyph_index) {

    // NOTE: glyph
    // TODO: only allocate pixels for glyphs that have nonzero size!
    LoadedGlyph *packed_glyph = result.glyphs + glyph_index;
    packed_glyph->advance = loose_glyph->advance;
    packed_glyph->bitmap.width = loose_glyph->width;
    packed_glyph->bitmap.height = loose_glyph->height;
    packed_glyph->bitmap.stride = packed_glyph->bitmap.width*sizeof(U32);
    packed_glyph->bitmap.align_pos = v2s32(loose_glyph->left_bearing, loose_glyph->top_bearing);
    packed_glyph->bitmap.pixels = arena_push_array(arena, U32, packed_glyph->bitmap.width*packed_glyph->bitmap.height);

    // NOTE: copy pixels
    U8 *dest_row = (U8*)packed_glyph->bitmap.pixels, *src_row = loose_glyph->bitmap;
    for(S32 j = 0; j < packed_glyph->bitmap.height; ++j) {

      U8 *src_pixels = src_row;
      U32 *dest_pixels = (U32*)dest_row;
      for(S32 i = 0; i < packed_glyph->bitmap.width; ++i) {

	U8 src_pixel = *src_pixels;
	U32 dest_pixel = ((src_pixel << 3*8) |
			  (src_pixel << 2*8) |
			  (src_pixel << 1*8) |
			  (src_pixel << 0*8));
	
	*dest_pixels++ = dest_pixel;
	++src_pixels;
      }

      dest_row += packed_glyph->bitmap.stride;
      src_row += loose_glyph->stride;
    }

    // NOTE: codepoint -> glyph index map
    // TODO: better hash function!
    U32 cp_map_idx = loose_cp_idx->codepoint % result.codepoint_map_count;
    CodepointMapBucket *cp_map_bucket = result.codepoint_map + cp_map_idx;
    
    CodepointMapNode *cp_map_node = arena_push_struct(arena, CodepointMapNode);
    cp_map_node->codepoint = loose_cp_idx->codepoint;
    cp_map_node->glyph_index = glyph_index;
    
    SLLQueuePush(cp_map_bucket->first, cp_map_bucket->last, cp_map_node);
  }

  return(result);
}

proc LoadedGlyph*
font_get_glyph_from_codepoint(LoadedFont *font, U32 codepoint)
{
  U32 glyph_index = 0;
  U32 cp_map_idx = codepoint % font->codepoint_map_count;
  CodepointMapBucket *cp_map_bucket = font->codepoint_map + cp_map_idx;
  for(CodepointMapNode *n = cp_map_bucket->first; n; n = n->next) {

    if(n->codepoint == codepoint) {

      glyph_index = n->glyph_index;
      break;
    }
  }

  LoadedGlyph *result = font->glyphs + glyph_index;
  return(result);
}
