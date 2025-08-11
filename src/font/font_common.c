proc PackedFont*
font_pack(Arena *arena, LooseFont *loose_font)
{
  PackedFont *result = arena_push_struct_z(arena, PackedFont);
  result->ascender = loose_font->ascender;
  result->descender = loose_font->descender;
  result->line_height = loose_font->line_height;
  result->glyph_count = rangeu32_len(loose_font->glyph_idx_rng);
  result->glyphs = arena_push_array(arena, PackedGlyph, result->glyph_count);
  result->codepoint_map_count = 2*result->glyph_count;
  result->codepoint_map = arena_push_array(arena, CodepointMapBucket, result->codepoint_map_count);

  // NOTE: codepoint map
  for(CodepointIndex *cp_idx = loose_font->first_cp_index; cp_idx; cp_idx = cp_idx->next) {

    // TODO: better hash function!
    U32 cp_map_idx = cp_idx->codepoint % result->codepoint_map_count;
    CodepointMapBucket *cp_map_bucket = result->codepoint_map + cp_map_idx;
    
    CodepointMapNode *cp_map_node = arena_push_struct(arena, CodepointMapNode);
    cp_map_node->codepoint = cp_idx->codepoint;
    cp_map_node->glyph_index = cp_idx->index - loose_font->glyph_idx_rng.min;
    
    SLLQueuePush(cp_map_bucket->first, cp_map_bucket->last, cp_map_node);
  }

  // NOTE: glyphs
  typedef struct FontPackNode FontPackNode;
  struct FontPackNode
  {
    FontPackNode *next;
    U32 x;
    U32 y;
    LooseGlyph *loose_glyph;
  } *first_pack_node = 0, *last_pack_node = 0;

  U32 atlas_w = 256;
  U32 atlas_h = 256;
  U32 layout_x = 0;
  U32 layout_y = 0;
  U32 row_max_h = 0;
  U32 space_x = 0;
  U32 grow_count = 0;
  U32 max_grow_count = 10;
  U32 glyph_idx = 0;
  ArenaTemp scratch = arena_get_scratch(&arena, 1);
  for(LooseGlyph *loose_glyph = loose_font->first_glyph;
      loose_glyph;
      loose_glyph = loose_glyph->next, ++glyph_idx) {

    PackedGlyph *packed_glyph = result->glyphs + glyph_idx;
    packed_glyph->rect.min.x = loose_glyph->left_bearing;
    packed_glyph->rect.min.y = loose_glyph->top_bearing - loose_glyph->height;
    packed_glyph->rect.max.x = loose_glyph->left_bearing + loose_glyph->width;
    packed_glyph->rect.max.y = loose_glyph->top_bearing;
    packed_glyph->advance = loose_glyph->advance;

    B32 inserted = 0;
    while(!inserted && grow_count <= max_grow_count) {
      
      U32 max_x = layout_x + loose_glyph->width;
      U32 max_y = layout_y + loose_glyph->height;
      B32 fits = (max_x <= atlas_w && max_y <= atlas_h);
      if(fits) {

	FontPackNode *new_pack_node = arena_push_struct(scratch.arena, FontPackNode);
	new_pack_node->x = layout_x;
	new_pack_node->y = layout_y;
	new_pack_node->loose_glyph = loose_glyph;
	SLLQueuePush(first_pack_node, last_pack_node, new_pack_node);

	layout_x += loose_glyph->width;
	row_max_h = Max(row_max_h, (U32)loose_glyph->height);
	inserted = 1;
      }else {
	
	if(max_y > atlas_h) {
	  // NOTE: grow atlas
	  ++grow_count;
	  if(grow_count & 1) {
	    // NOTE: grow x
	    space_x = atlas_w;
	    layout_x = atlas_w;
	    layout_y = 0;
	    row_max_h = 0;
	    atlas_w *= 2;
	  }else {
	    // NOTE: grow y
	    space_x = 0;
	    layout_x = 0;
	    layout_y = atlas_h;
	    row_max_h = 0;
	    atlas_h *= 2;
	  }	  
	}else {
	  // NOTE: only overflowing x. go to next row
	  layout_y += row_max_h;
	  layout_x = space_x;
	  row_max_h = 0;
	}
      }
    }

    if(!inserted) {
      fprintf(stderr, "ERROR: font atlas creation failed\n");
      break;
    }    
  }

  // NOTE: build atlas
  U32 atlas_texture = 0;
  glGenTextures(1, &atlas_texture);
  result->atlas_texture_id = atlas_texture;
  result->atlas_width = atlas_w;
  result->atlas_height = atlas_h;

  // TODO: pull out the OpenGL-specific stuff
  glBindTexture(GL_TEXTURE_2D, atlas_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
	       result->atlas_width, result->atlas_height, 0,
	       GL_RED, GL_UNSIGNED_BYTE, 0);
  GlTextureDefaultParams();  

  R32 atlas_inv_w = 1.f/(R32)atlas_w;
  R32 atlas_inv_h = 1.f/(R32)atlas_h;
  glyph_idx = 0;
  for(FontPackNode *pack_node = first_pack_node;
      pack_node;
      pack_node = pack_node->next, ++glyph_idx) {

    U32 x = pack_node->x;
    U32 y = pack_node->y;
    U32 w = pack_node->loose_glyph->width;
    U32 h = pack_node->loose_glyph->height;

    // NOTE: uv data
    PackedGlyph *glyph = result->glyphs + glyph_idx;
    glyph->uv.min.x = x * atlas_inv_w;
    glyph->uv.min.y = y * atlas_inv_h;
    glyph->uv.max.x = (x + w) * atlas_inv_w;
    glyph->uv.max.y = (y + h) * atlas_inv_h;

    // NOTE: copy bitmap data to atlas
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, GL_RED, GL_UNSIGNED_BYTE,
		    pack_node->loose_glyph->bitmap);
  }

  arena_release_scratch(scratch);
  return(result);
}

proc U32
font_glyph_index_from_codepoint(PackedFont *font, U32 codepoint)
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

  return(glyph_index);
}

proc PackedGlyph*
font_glyph_from_codepoint(PackedFont *font, U32 codepoint)
{
  U32 glyph_index = font_glyph_index_from_codepoint(font, codepoint);
  PackedGlyph *result = font->glyphs + glyph_index;
  return(result);
}
