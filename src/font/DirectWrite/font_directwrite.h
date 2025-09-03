//#include <dwrite_1.h>
// NOTE: pieces taken and modified from windows' dwrite.h and dcommon.h, for interoperability with C
/* #include <combaseapi.h> */
/* #include <dcommon.h> */
#include <initguid.h>
#pragma comment(lib, "dwrite.lib")

// NOTE: defines
#ifndef DWRITE_EXPORT
#  define DWRITE_EXPORT __declspec(dllimport) WINAPI
#endif

#define DWRITE_METHOD(ret, interface, name, ...) \
  static inline ret Glue(Glue(interface, _), name)(interface *this_, __VA_ARGS__)
#define DWRITE_METHOD_VOID(ret, interface, name) \
  static inline ret Glue(Glue(interface, _), name)(interface *this_)

DEFINE_GUID(IID_IDWriteFactory,  0xb859ee5a, 0xd838, 0x4b5b, 0xa2, 0xe8, 0x1a, 0xdc, 0x7d, 0x93, 0xdb, 0x48);
DEFINE_GUID(IID_IDWriteFactory1, 0x30572f99, 0xdac6, 0x41db, 0xa1, 0x6e, 0x04, 0x86, 0x30, 0x7e, 0x60, 0x6a);

// NOTE: interfaces
typedef struct IDWriteFactory			{ struct { void *tbl[]; } *v; } IDWriteFactory;
typedef struct IDWriteFontFile			{ struct { void *tbl[]; } *v; } IDWriteFontFile;
typedef struct IDWriteFontFace			{ struct { void *tbl[]; } *v; } IDWriteFontFace;
typedef struct IDWriteFontFace1			{ struct { void *tbl[]; } *v; } IDWriteFontFace1;
typedef struct IDWriteRenderingParams		{ struct { void *tbl[]; } *v; } IDWriteRenderingParams;
typedef struct IDWriteGdiInterop		{ struct { void *tbl[]; } *v; } IDWriteGdiInterop;
typedef struct IDWriteBitmapRenderTarget	{ struct { void *tbl[]; } *v; } IDWriteBitmapRenderTarget;

// NOTE: types
typedef enum DWRITE_FACTORY_TYPE
{
  DWRITE_FACTORY_TYPE_SHARED,
  DWRITE_FACTORY_TYPE_ISOLATED
} DWRITE_FACTORY_TYPE;

typedef enum DWRITE_FONT_FACE_TYPE
{
  DWRITE_FONT_FACE_TYPE_CFF,
  DWRITE_FONT_FACE_TYPE_TRUETYPE,
  DWRITE_FONT_FACE_TYPE_OPENTYPE_COLLECTION,
  DWRITE_FONT_FACE_TYPE_TYPE1,
  DWRITE_FONT_FACE_TYPE_VECTOR,
  DWRITE_FONT_FACE_TYPE_BITMAP,
  DWRITE_FONT_FACE_TYPE_UNKNOWN,
  DWRITE_FONT_FACE_TYPE_RAW_CFF,
  DWRITE_FONT_FACE_TYPE_TRUETYPE_COLLECTION = DWRITE_FONT_FACE_TYPE_OPENTYPE_COLLECTION,
} DWRITE_FONT_FACE_TYPE;

typedef enum DWRITE_FONT_SIMULATIONS
{
  DWRITE_FONT_SIMULATIONS_NONE    = 0x0000,
  DWRITE_FONT_SIMULATIONS_BOLD    = 0x0001,
  DWRITE_FONT_SIMULATIONS_OBLIQUE = 0x0002,
} DWRITE_FONT_SIMULATIONS;

typedef enum DWRITE_PIXEL_GEOMETRY
{
  DWRITE_PIXEL_GEOMETRY_FLAT,
  DWRITE_PIXEL_GEOMETRY_RGB,
  DWRITE_PIXEL_GEOMETRY_BGR,
} DWRITE_PIXEL_GEOMETRY;

typedef enum DWRITE_RENDERING_MODE
{
  DWRITE_RENDERING_MODE_DEFAULT,
  DWRITE_RENDERING_MODE_ALIASED,
  DWRITE_RENDERING_MODE_GDI_CLASSIC,
  DWRITE_RENDERING_MODE_GDI_NATURAL,
  DWRITE_RENDERING_MODE_NATURAL,
  DWRITE_RENDERING_MODE_NATURAL_SYMMETRIC,
  DWRITE_RENDERING_MODE_OUTLINE,
  DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC         = DWRITE_RENDERING_MODE_GDI_CLASSIC,
  DWRITE_RENDERING_MODE_CLEARTYPE_GDI_NATURAL         = DWRITE_RENDERING_MODE_GDI_NATURAL,
  DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL             = DWRITE_RENDERING_MODE_NATURAL,
  DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC   = DWRITE_RENDERING_MODE_NATURAL_SYMMETRIC,
} DWRITE_RENDERING_MODE;

typedef enum DWRITE_MEASURING_MODE
{
  DWRITE_MEASURING_MODE_NATURAL,
  DWRITE_MEASURING_MODE_GDI_CLASSIC,
  DWRITE_MEASURING_MODE_GDI_NATURAL,
} DWRITE_MEASURING_MODE;

typedef struct DWRITE_FONT_METRICS
{
  UINT16 designUnitsPerEm;
  UINT16 ascent;
  UINT16 descent;
  INT16 lineGap;
  UINT16 capHeight;
  UINT16 xHeight;
  INT16 underlinePosition;
  UINT16 underlineThickness;
  INT16 strikethroughPosition;
  UINT16 strikethroughThickness;
} DWRITE_FONT_METRICS;

typedef struct DWRITE_GLYPH_OFFSET
{
  FLOAT advanceOffset;
  FLOAT ascenderOffset;
} DWRITE_GLYPH_OFFSET;

typedef struct DWRITE_GLYPH_RUN
{
  IDWriteFontFace* fontFace;
  FLOAT fontEmSize;
  UINT32 glyphCount;
  UINT16 const* glyphIndices;
  FLOAT const* glyphAdvances;
  DWRITE_GLYPH_OFFSET const* glyphOffsets;
  BOOL isSideways;
  UINT32 bidiLevel;
} DWRITE_GLYPH_RUN;

typedef struct DWRITE_GLYPH_METRICS
{
  INT32 leftSideBearing;
  UINT32 advanceWidth;
  INT32 rightSideBearing;
  INT32 topSideBearing;
  UINT32 advanceHeight;
  INT32 bottomSideBearing;
  INT32 verticalOriginY;
} DWRITE_GLYPH_METRICS;

typedef struct DWRITE_UNICODE_RANGE
{
  UINT32 first;
  UINT32 last;
} DWRITE_UNICODE_RANGE;

// NOTE: functions
EXTERN_C HRESULT DWRITE_EXPORT DWriteCreateFactory(DWRITE_FACTORY_TYPE factoryType, REFIID iid, IUnknown **factory);
/* EXTERN_C HRESULT DWRITE_EXPORT DWriteCreateFactory(DWRITE_FACTORY_TYPE factoryType, const GUID *iid, IUnknown **factory); */

// NOTE: methods
DWRITE_METHOD(HRESULT, IDWriteFactory, CreateFontFileReference,
	      WCHAR const* filePath, FILETIME const* lastWriteTime, IDWriteFontFile** fontFile)
{
  return(((HRESULT (WINAPI*)(IDWriteFactory*, WCHAR const*, FILETIME const*, IDWriteFontFile**))this_->v->tbl[7])(this_, filePath, lastWriteTime, fontFile));
}
DWRITE_METHOD(HRESULT, IDWriteFactory, CreateFontFace,
	      DWRITE_FONT_FACE_TYPE fontFaceType, UINT32 numberOfFiles,
	      IDWriteFontFile* const* fontFiles, UINT32 faceIndex,
	      DWRITE_FONT_SIMULATIONS fontFaceSimulationFlags, IDWriteFontFace** fontFace)
{
  return(((HRESULT (WINAPI*)(IDWriteFactory*, DWRITE_FONT_FACE_TYPE, UINT32, IDWriteFontFile*const*, UINT32, DWRITE_FONT_SIMULATIONS, IDWriteFontFace**))this_->v->tbl[9])(this_, fontFaceType, numberOfFiles, fontFiles, faceIndex, fontFaceSimulationFlags, fontFace));
}
DWRITE_METHOD(HRESULT, IDWriteFactory, CreateRenderingParams,
	      IDWriteRenderingParams** renderingParams)
{
    return(((HRESULT (WINAPI*)(IDWriteFactory*, IDWriteRenderingParams**))this_->v->tbl[10])(this_, renderingParams));
}
DWRITE_METHOD(HRESULT, IDWriteFactory, CreateCustomRenderingParams,
	      FLOAT gamma, FLOAT enhancedContrast, FLOAT clearTypeLevel,
	      DWRITE_PIXEL_GEOMETRY pixelGeometry, DWRITE_RENDERING_MODE renderingMode,
	      IDWriteRenderingParams** renderingParams)
{
  return(((HRESULT (WINAPI*)(IDWriteFactory*, FLOAT, FLOAT, FLOAT, DWRITE_PIXEL_GEOMETRY, DWRITE_RENDERING_MODE, IDWriteRenderingParams**))this_->v->tbl[12])(this_, gamma, enhancedContrast, clearTypeLevel, pixelGeometry, renderingMode, renderingParams));
}
DWRITE_METHOD(HRESULT, IDWriteFactory, GetGdiInterop,
	      IDWriteGdiInterop** gdiInterop)
{
  return(((HRESULT (WINAPI*)(IDWriteFactory*, IDWriteGdiInterop**))this_->v->tbl[17])(this_, gdiInterop));
}

DWRITE_METHOD_VOID(FLOAT, IDWriteRenderingParams, GetEnhancedContrast)
{
  return(((FLOAT (WINAPI*)(IDWriteRenderingParams*))this_->v->tbl[4])(this_));
}
DWRITE_METHOD_VOID(FLOAT, IDWriteRenderingParams, GetClearTypeLevel)
{
  return(((FLOAT (WINAPI*)(IDWriteRenderingParams*))this_->v->tbl[5])(this_));
}
DWRITE_METHOD_VOID(DWRITE_PIXEL_GEOMETRY, IDWriteRenderingParams, GetPixelGeometry)
{
  return(((DWRITE_PIXEL_GEOMETRY (WINAPI*)(IDWriteRenderingParams*))this_->v->tbl[6])(this_));
}

DWRITE_METHOD(void, IDWriteFontFace, GetMetrics,
	      DWRITE_FONT_METRICS* fontFaceMetrics)
{
  ((void (WINAPI*)(IDWriteFontFace*, DWRITE_FONT_METRICS*))this_->v->tbl[8])(this_, fontFaceMetrics);
}
DWRITE_METHOD(HRESULT, IDWriteFontFace, GetDesignGlyphMetrics,
	      UINT16 const* glyphIndices, UINT32 glyphCount, DWRITE_GLYPH_METRICS* glyphMetrics,
	      BOOL isSideways)
{
  return(((HRESULT (WINAPI*)(IDWriteFontFace*, UINT16 const*, UINT32, DWRITE_GLYPH_METRICS*, BOOL))this_->v->tbl[10])(this_, glyphIndices, glyphCount, glyphMetrics, isSideways));
}
// TODO: I don't know why the macro isn't working here
/* DWRITE_METHOD(HRESULT, IDWriteFontFace, GetGlyphIndices, */
/* 	      const UINT32* codePoints, UINT32 codePointCount, UINT16* glyphIndices) */
static inline HRESULT IDWriteFontFace_GetGlyphIndices(IDWriteFontFace* this_, const UINT32* codePoints, UINT32 codePointCount, UINT16* glyphIndices)
{
  return(((HRESULT (WINAPI*)(IDWriteFontFace*, const UINT32*, UINT32, UINT16*))this_->v->tbl[11])(this_, codePoints, codePointCount, glyphIndices));
}
DWRITE_METHOD_VOID(UINT16, IDWriteFontFace, GetGlyphCount)
{
   return(((UINT16 (WINAPI*)(IDWriteFontFace*))this_->v->tbl[9])(this_));
}
DWRITE_METHOD(HRESULT, IDWriteFontFace1, GetUnicodeRanges,
	      UINT32 maxRangeCount, DWRITE_UNICODE_RANGE* unicodeRanges, UINT32* actualRangeCount)
{
  return(((HRESULT (WINAPI*)(IDWriteFontFace1*, UINT32, DWRITE_UNICODE_RANGE*, UINT32*))this_->v->tbl[21])(this_, maxRangeCount, unicodeRanges, actualRangeCount));
}

DWRITE_METHOD(HRESULT, IDWriteGdiInterop, CreateBitmapRenderTarget,
	      HDC hdc, UINT32 width, UINT32 height, IDWriteBitmapRenderTarget** renderTarget)
{
  return(((HRESULT (WINAPI*)(IDWriteGdiInterop*, HDC, UINT32, UINT32, IDWriteBitmapRenderTarget**))this_->v->tbl[7])(this_, hdc, width, height, renderTarget));
}

DWRITE_METHOD(HRESULT, IDWriteBitmapRenderTarget, DrawGlyphRun,
	      FLOAT baselineOriginX, FLOAT baselineOriginY, DWRITE_MEASURING_MODE measuringMode,
	      DWRITE_GLYPH_RUN const* glyphRun, IDWriteRenderingParams* renderingParams,
	      COLORREF textColor, RECT* blackBoxRect)
{
  return(((HRESULT (WINAPI*)(IDWriteBitmapRenderTarget*, FLOAT, FLOAT, DWRITE_MEASURING_MODE, DWRITE_GLYPH_RUN const*, IDWriteRenderingParams*, COLORREF, RECT*))this_->v->tbl[3])(this_, baselineOriginX, baselineOriginY, measuringMode, glyphRun, renderingParams, textColor, blackBoxRect));
}
DWRITE_METHOD_VOID(HDC, IDWriteBitmapRenderTarget, GetMemoryDC)
{
  return(((HDC (WINAPI*)(IDWriteBitmapRenderTarget*))this_->v->tbl[4])(this_));
}

// NOTE: internal structures
typedef struct DWriteFontMetrics
{
  R32 pixel_per_em;
  R32 pixel_per_design_unit;

  S32 ascender;
  S32 descender;
  U32 line_height;
} DWriteFontMetrics;

typedef struct DWriteFont
{
  IDWriteFontFile *font_file;
  IDWriteFontFace *font_face;
  DWriteFontMetrics metrics;
} DWriteFont;

typedef struct DWriteState
{
  Arena *arena;

  U32 factory_version;
  IDWriteFactory *factory;
  IDWriteRenderingParams *default_render_params;
  IDWriteRenderingParams *render_params;
  IDWriteGdiInterop *gdi_interop;
} DWriteState;

global DWriteState *dwrite_state = 0;

// NOTE: internal functions
proc B32 dwrite_init(void);

proc DWriteFont dwrite_open_font(String8 font_path);
proc void dwrite_font_metrics(DWriteFont *font, U32 font_size_pt);
proc RangeU32 dwrite_font_glyph_map(Arena *arena, DWriteFont *font, CodepointIndex **first, CodepointIndex **last);
proc void dwrite_rasterize_glyphs(Arena *arena, DWriteFont *font, RangeU32 range, LooseGlyph **first, LooseGlyph **last);

// TODO: pull out into font_common (align with freetype)
proc LooseFont font_parse(Arena *arena, String8 font_path, U32 font_size_pt);
