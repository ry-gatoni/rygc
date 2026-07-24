#pragma pack(push, 0)
typedef struct CmapTableHeader
{
  U16 version;
  U16 subtable_count;
} CmapTableHeader;

typedef struct CmapSubtable
{
  U16 platform_id;
  U16 platform_specific_id;
  U32 offset;
} CmapSubtable;


#pragma pack(pop)

typedef struct UnicodeRange
{
  U32 start_cp;
  U32 end_cp;
} UnicodeRange;

typedef enum CharSetBitmapParserStatus
{
  CharSetBitmapParserStatus_end_of_block,
  CharSetBitmapParserStatus_start_of_range,
  CharSetBitmapParserStatus_end_of_range,
} CharSetBitmapParserStatus;

typedef struct CharSetBitmapParser CharSetBitmapParser;
#define CHAR_SET_BITMAP_PARSER_PROC(name) CharSetBitmapParserStatus (name)(CharSetBitmapParser *self)
typedef CHAR_SET_BITMAP_PARSER_PROC(CharSetBitmapParserProc);
struct CharSetBitmapParser
{
  U32 plane_idx;
  U16 byte_idx;
  U16 bit_idx;
  UnicodeRange *current_range;
  const U8 *block_bytes;
  CharSetBitmapParserProc *parse;
};

proc CHAR_SET_BITMAP_PARSER_PROC(char_set_bitmap_parse_extending_range);
proc CHAR_SET_BITMAP_PARSER_PROC(char_set_bitmap_parse_searching_for_range_start);

proc void
char_set_bitmap_parser_init(CharSetBitmapParser *parser, const U8 *block_bytes, UnicodeRange *current_range)
{
  parser->plane_idx = 0;
  parser->byte_idx = 0;
  parser->bit_idx = 0;
  parser->current_range = current_range;
  parser->block_bytes = block_bytes;
  //parser->parse = char_set_bitmap_parse_extending_range;
  parser->parse = char_set_bitmap_parse_searching_for_range_start;
}

proc CharSetBitmapParserStatus
char_set_bitmap_parse_extending_range(CharSetBitmapParser *self)
{
  // TODO: how to deduplicate?
  if(self->bit_idx)
  {
    U8 byte = self->block_bytes[self->byte_idx];
    for(; self->bit_idx < 8; ++self->bit_idx)
    {
      U8 mask = 0x01 << self->bit_idx;
      if(!(byte & mask))
      {
	self->current_range->end_cp = (self->plane_idx << 16) + 8*self->byte_idx + self->bit_idx;
	++self->bit_idx;
	goto end_of_range;
      }
    }
    self->bit_idx = 0;
    ++self->byte_idx;
  }

  for(; self->byte_idx < KB(8); ++self->byte_idx)
  {
    U8 byte = self->block_bytes[self->byte_idx];
    if(byte == 0xFF)
    {
      self->current_range->end_cp += 8;
    }
    else if(byte & 0xFF)
    {
      for(; self->bit_idx < 8; ++self->bit_idx)
      {
	U8 mask = 0x01 << self->bit_idx;
	if(!(byte & mask))
	{
	  self->current_range->end_cp = (self->plane_idx << 16) + 8*self->byte_idx + self->bit_idx;
	  ++self->bit_idx;
	  goto end_of_range;
	}
      }
      self->bit_idx = 0;
    }
    else
    {
      self->current_range->end_cp = (self->plane_idx << 16) + 8*self->byte_idx;
      ++self->byte_idx;
      goto end_of_range;
    }
  }
  self->byte_idx = 0;

  return CharSetBitmapParserStatus_end_of_block;

end_of_range:
  self->parse = char_set_bitmap_parse_searching_for_range_start;
  return CharSetBitmapParserStatus_end_of_range;
}

proc CharSetBitmapParserStatus
char_set_bitmap_parse_searching_for_range_start(CharSetBitmapParser *self)
{
  // TODO: how to deduplicate?
  if(self->bit_idx)
  {
    U8 byte = self->block_bytes[self->byte_idx];
    for(; self->bit_idx < 8; ++self->bit_idx)
    {
      U8 mask = 0x01 << self->bit_idx;
      if(byte & mask)
      {
	self->current_range->start_cp = (self->plane_idx << 16) + 8*self->byte_idx + self->bit_idx;
	self->current_range->end_cp = self->current_range->start_cp;
	++self->bit_idx;
	goto start_of_range;
      }
    }
    self->bit_idx = 0;
    ++self->byte_idx;
  }

  for(; self->byte_idx < KB(8); ++self->byte_idx)
  {
    U8 byte = self->block_bytes[self->byte_idx];
    if(byte == 0xFF)
    {
      self->current_range->start_cp = (self->plane_idx << 16) + 8*self->byte_idx;
      self->current_range->end_cp = self->current_range->start_cp + 8;
      ++self->byte_idx;
      goto start_of_range;
    }
    else if(byte & 0xFF)
    {
      for(; self->bit_idx < 8; ++self->bit_idx)
      {
	U8 mask = 0x01 << self->bit_idx;
	if(byte & mask)
	{
	  self->current_range->start_cp = (self->plane_idx << 16) + 8*self->byte_idx + self->bit_idx;
	  self->current_range->end_cp = self->current_range->start_cp;
	  ++self->bit_idx;
	  goto start_of_range;
	}
      }
    }
  }
  self->byte_idx = 0;

  return CharSetBitmapParserStatus_end_of_block;

start_of_range:
  self->parse = char_set_bitmap_parse_extending_range;
  return CharSetBitmapParserStatus_start_of_range;
}

proc LooseFont
font_parse(Arena *arena, String8 font_path, U32 font_size_pt)
{
  ArenaTemp scratch = arena_get_scratch(&arena, 1);

  CFArrayRef font_descs;
  {
    CFAllocatorRef def_alloc = kCFAllocatorDefault;
    CFStringRef cf_font_path = CFStringCreateWithCString(def_alloc, (const char*)font_path.string, kCFStringEncodingUTF8);
    CFURLRef font_url = CFURLCreateWithFileSystemPath(def_alloc, cf_font_path, kCFURLPOSIXPathStyle, 0);
    font_descs = CTFontManagerCreateFontDescriptorsFromURL(font_url);
    CFRelease(font_url);
    CFRelease(cf_font_path);
  }
  // TODO: check errors

  CTFontRef ct_font;
  {
    CFIndex font_desc_count = CFArrayGetCount(font_descs);
    Assert(font_desc_count == 1);
    CTFontDescriptorRef font_desc = CFArrayGetValueAtIndex(font_descs, 0);
    ct_font = CTFontCreateWithFontDescriptor(font_desc, (CGFloat)font_size_pt, 0);
    CFRelease(font_descs);
  }
  // TODO: check errors

  CGFloat ascent = CTFontGetAscent(ct_font);
  CGFloat descent = CTFontGetDescent(ct_font);
  CGFloat line_height = CTFontGetLeading(ct_font);
  CFIndex glyph_count = CTFontGetGlyphCount(ct_font);

  UnicodeRange *unicode_ranges = arena_push_array(scratch.arena, UnicodeRange, glyph_count); // upper bound
  unicode_ranges[0].start_cp = unicode_ranges[0].end_cp = 0;
  U32 unicode_range_count = 0;

  CFCharacterSetRef cf_char_set = CTFontCopyCharacterSet(ct_font);
  CFDataRef char_set_bitmap = CFCharacterSetCreateBitmapRepresentation(kCFAllocatorDefault, cf_char_set);
  CFIndex char_set_bitmap_size = CFDataGetLength(char_set_bitmap);
  const U8 *char_set_bitmap_bytes = CFDataGetBytePtr(char_set_bitmap);
  // SCHEMA:
  // 8 kb blocks, each representing 64 kb unicode planes (bit set if codepoint supported).
  // first block is always the BMP. each subsequent block has a 1-byte before it indicating the plane index.
  const U32 char_set_bitmap_block_size = KB(8);
  U32 char_set_block_count = char_set_bitmap_size / char_set_bitmap_block_size;

  U32 blocks_parsed = 0;
  U32 codepoint_count = 0;
  CharSetBitmapParser parser = {0};
  char_set_bitmap_parser_init(&parser, char_set_bitmap_bytes, &unicode_ranges[0]);
  while(blocks_parsed < char_set_block_count)
  {
    CharSetBitmapParserStatus status = parser.parse(&parser);
    if(status == CharSetBitmapParserStatus_end_of_block)
    {
      parser.block_bytes += char_set_bitmap_block_size;
      parser.plane_idx = *parser.block_bytes++;
      parser.byte_idx = 0;
      parser.bit_idx = 0;
      ++blocks_parsed;
      if(blocks_parsed == char_set_block_count && parser.parse == char_set_bitmap_parse_extending_range)
      {
	Assert(parser.current_range->start_cp != 0);
	Assert(parser.current_range->end_cp != 0);
	U32 range_codepoint_count = parser.current_range->end_cp - parser.current_range->start_cp;
	codepoint_count += range_codepoint_count;
	parser.current_range = &unicode_ranges[unicode_range_count];
      }
    }
    else if(status == CharSetBitmapParserStatus_end_of_range)
    {
      Assert(parser.current_range->start_cp != 0);
      Assert(parser.current_range->end_cp != 0);
      U32 range_codepoint_count = parser.current_range->end_cp - parser.current_range->start_cp;
      codepoint_count += range_codepoint_count;
      parser.current_range = &unicode_ranges[unicode_range_count];
    }
    else if(status == CharSetBitmapParserStatus_start_of_range)
    {
      ++unicode_range_count;
    }
  }

  // TODO: map codepoints to glyph indices, build loose structure
  // NOTE: we have codepoints for non-visible control characters; those will
  // probably map to the nil glyph (index 0). we then have to make sure not to
  // pollute the atlas/codepoint-glyph map with nil glyphs and control
  // codepoints when we bake the font

  U32 cp_glyph_count = 0;
  CFCharacterSetRef control_chars = CFCharacterSetGetPredefined(kCFCharacterSetControl);
  CGGlyph *glyphs = arena_push_array(scratch.arena, CGGlyph, codepoint_count);
  UniChar *characters = arena_push_array(scratch.arena, UniChar, codepoint_count);
  for(U32 range_idx = 0; range_idx < unicode_range_count; ++range_idx)
  {
    UnicodeRange range = unicode_ranges[range_idx];
    U32 range_count = range.end_cp - range.start_cp;
    U32 valid_count_in_range = 0;
    for(U32 char_idx = 0; char_idx < range_count; ++char_idx)
    {
      UniChar unichar = range.start_cp + char_idx;
      if(!CFCharacterSetIsCharacterMember(control_chars, unichar))
      { characters[cp_glyph_count + valid_count_in_range++] = unichar; }
    }
    if(!CTFontGetGlyphsForCharacters(ct_font, characters + cp_glyph_count, glyphs + cp_glyph_count, valid_count_in_range))
    { Assert(0); }
    cp_glyph_count += valid_count_in_range;
  }

  CGRect *glyph_rects = arena_push_array(scratch.arena, CGRect, cp_glyph_count);
  CGRect bounding_rect = CTFontGetBoundingRectsForGlyphs(ct_font, 0, glyphs, glyph_rects, cp_glyph_count);

  CGSize *advances = arena_push_array(scratch.arena, CGSize, cp_glyph_count);
  CTFontGetAdvancesForGlyphs(ct_font, 0, glyphs, advances, cp_glyph_count);

  /* CGSize *vertical_translations = arena_push_array(scratch.arena, CGSize, codepoint_count); */
  /* CTFontGetVerticalTranslationsForGlyphs(ct_font, glyphs, vertical_translations, codepoint_count); */

  CGPoint *positions = arena_push_array(scratch.arena, CGPoint, cp_glyph_count);
  for(U32 glyph_idx = 0; glyph_idx < cp_glyph_count; ++glyph_idx)
  {
    CGPoint base_pos = {glyph_idx * bounding_rect.size.width, 0};
    CGRect glyph_rect = glyph_rects[glyph_idx];
    CGPoint glyph_pos = {base_pos.x - glyph_rect.origin.x, -glyph_rect.origin.y};
    positions[glyph_idx] = glyph_pos;
  }

  U32 bitmap_width = ((U32)bounding_rect.size.width + 1)*cp_glyph_count;
  U32 bitmap_height = ((U32)bounding_rect.size.height + 1);
  U32 *bitmap = arena_push_array(arena, U32, bitmap_width*bitmap_height);

  U32 bitmap_bytes_per_row = sizeof(*bitmap)*bitmap_width;
  CGColorSpaceRef color_space = CGColorSpaceCreateDeviceRGB();
  CGContextRef cg_ctxt = CGBitmapContextCreate(bitmap, bitmap_width, bitmap_height, 8, bitmap_bytes_per_row,
					       color_space, kCGImageAlphaPremultipliedLast);
  CGContextSetRGBFillColor(cg_ctxt, 1, 1, 1, 1);
  CTFontDrawGlyphs(ct_font, glyphs, positions, cp_glyph_count, cg_ctxt);
  CGContextRelease(cg_ctxt);
  CGColorSpaceRelease(color_space);

  RangeU32 glyph_idx_rng = range_u32_invalid();
  CodepointIndex *first_cp_idx, *last_cp_idx; first_cp_idx = last_cp_idx = 0;
  LooseGlyph *first_loose_glyph, *last_loose_glyph; first_loose_glyph = last_loose_glyph = 0;
  for(U32 cp_idx = 0; cp_idx < cp_glyph_count; ++cp_idx)
  {
    U32 cp = characters[cp_idx];
    U32 glyph_idx = glyphs[cp_idx];
    CGRect glyph_rect = glyph_rects[cp_idx];
    U32 advance = (U32)advances[cp_idx].width;
    //CGSize vertical_translation = vertical_translations[cp_idx];
    CodepointIndex *new_cp_idx = arena_push_struct(arena, CodepointIndex);
    new_cp_idx->codepoint = cp;
    new_cp_idx->index = glyph_idx;
    SLLQueuePush(first_cp_idx, last_cp_idx, new_cp_idx);

    LooseGlyph *loose_glyph = arena_push_struct(arena, LooseGlyph);
    loose_glyph->width = glyph_rect.size.width;
    loose_glyph->height = glyph_rect.size.height;
    loose_glyph->stride = bitmap_bytes_per_row;
    loose_glyph->left_bearing = glyph_rect.origin.x; // ?
    loose_glyph->top_bearing = glyph_rect.origin.y + glyph_rect.size.height; // ?
    loose_glyph->advance = advance;
    loose_glyph->bitmap = 0; // TODO: packing code should really handle the stride, instead of requiring per-glyph bitmaps
    if(loose_glyph->width && loose_glyph->height)
    {
      loose_glyph->bitmap = arena_push_array_z(arena, U32, loose_glyph->width*loose_glyph->height);

      U32 *dest_row = loose_glyph->bitmap;
      U32 *src_row = bitmap + (U32)(cp_idx*bounding_rect.size.width) + bitmap_width*(bitmap_height - 1);
      for(S32 row_idx = 0; row_idx < loose_glyph->height; ++row_idx)
      {
	CopyArray(dest_row, src_row, U32, loose_glyph->width);
	dest_row += loose_glyph->width;
	src_row -= bitmap_width;
      }
    }
    SLLQueuePush(first_loose_glyph, last_loose_glyph, loose_glyph);

    glyph_idx_rng.min = Min(glyph_idx_rng.min, glyph_idx);
    glyph_idx_rng.max = Max(glyph_idx_rng.max, glyph_idx);
  }

  arena_release_scratch(scratch);
  CFRelease(ct_font);

  LooseFont result = {0};
  result.first_cp_index = first_cp_idx;
  result.last_cp_index = last_cp_idx;
  result.first_glyph = first_loose_glyph;
  result.last_glyph = last_loose_glyph;
  result.glyph_idx_rng = glyph_idx_rng;
  result.ascender = ascent;
  result.descender = descent;
  result.line_height = line_height;
  return result;
}
