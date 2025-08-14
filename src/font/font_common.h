// NOTE: loose font structures
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
  S32 left_bearing;
  S32 top_bearing;
  U32 advance;
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

typedef struct PackedGlyph
{
  Rect2 rect;
  Rect2 uv;
  R32 advance;
} PackedGlyph;

typedef struct GlTexture
{  
  S32 width;
  S32 height;
} GlTexture;

typedef struct PackedFont
{
  S32 ascender;
  S32 descender;
  U32 line_height;

  U32 glyph_count;
  PackedGlyph *glyphs;

  U32 codepoint_map_count;
  CodepointMapBucket *codepoint_map;

  R_Texture atlas;
} PackedFont;

typedef struct CodepointHashKey
{
  U32 codepoint;
  U64 hash;
} CodepointHashKey;

proc PackedFont* font_pack(Arena *arena, LooseFont *loose_font);
proc U32 font_glyph_index_from_codepoint(PackedFont *font, U32 codepoint);
proc PackedGlyph* font_glyph_from_codepoint(PackedFont *font, U32 codepoint);
