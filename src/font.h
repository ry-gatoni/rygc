
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

// NOTE: packed font structures
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

  U32 codepoint_map_count;
  CodepointMapBucket *codepoint_map;
} LoadedFont;

typedef struct CodepointHashKey
{
  U32 codepoint;
  U64 hash;
} CodepointHashKey;

proc LoadedFont font_pack(Arena *arena, LooseFont *loose_font);
