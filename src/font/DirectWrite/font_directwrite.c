proc LooseFont
font_parse(Arena *arena, String8 font_path, U32 font_size_pt)
{
  LooseFont result = {0};
  HRESULT error = 0;
  U64 arena_restore = arena_pos(arena);

#define Win32LogError(error) do { char *msg; FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM, 0, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&msg, 0, 0); OutputDebugStringA(msg); } while(0)
#define DWCheck(run) if(error != S_OK) { Win32LogError(error); error = S_OK; arena_pop_to(arena, arena_restore); run; goto end; }
#define DWCheckPtr(ptr, run) if((ptr == 0) || (error != S_OK)) { Win32LogError(error); error = S_OK; arena_pop_to(arena, arena_restore); run; goto end; }

  IDWriteFactory *factory = 0;
  error = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, &IID_IDWriteFactory, (IUnknown**)&factory);
  DWCheckPtr(factory, Assert(!"factory"));

  IDWriteFontFile *font_file = 0;
  {
    ArenaTemp scratch = arena_get_scratch(&arena, 1);
    String16 font_path_w = str16_from_str8(scratch.arena, font_path);
    error = IDWriteFactory_CreateFontFileReference(factory, font_path_w.string, 0, &font_file);
    arena_release_scratch(scratch);
    DWCheckPtr(font_file, Assert(!"font_file"));
  }

  IDWriteFontFace *font_face = 0;
  error = IDWriteFactory_CreateFontFace(factory, DWRITE_FONT_FACE_TYPE_TRUETYPE, 1, &font_file, 0, DWRITE_FONT_SIMULATIONS_NONE, &font_face);
  DWCheckPtr(font_face, Assert(!"font_face"));

  IDWriteRenderingParams *default_render_params = 0;
  error = IDWriteFactory_CreateRenderingParams(factory, &default_render_params);
  DWCheckPtr(default_render_params, Assert(!"default_render_params"));

  IDWriteRenderingParams *render_params = 0;
  {
    R32 gamma = 1.f;
    error =
      IDWriteFactory_CreateCustomRenderingParams
      (
       factory,
       gamma,
       IDWriteRenderingParams_GetEnhancedContrast(default_render_params),
       IDWriteRenderingParams_GetClearTypeLevel(default_render_params),
       IDWriteRenderingParams_GetPixelGeometry(default_render_params),
       DWRITE_RENDERING_MODE_NATURAL,
       &render_params
      );
    DWCheckPtr(render_params, Assert(!"render_params"));
  }

  IDWriteGdiInterop *gdi_interop = 0;
  error = IDWriteFactory_GetGdiInterop(factory, &gdi_interop);
  DWCheckPtr(gdi_interop, Assert(!"gdi_interop"));

  DWRITE_FONT_METRICS font_metrics = {0};
  IDWriteFontFace_GetMetrics(font_face, &font_metrics);

  R32 dpi = 81; // TODO: ???
  R32 pixel_per_em = (R32)font_size_pt*(R32)dpi/72.f;
  R32 pixel_per_design_unit = pixel_per_em/(R32)font_metrics.designUnitsPerEm;
  U16 ascender = font_metrics.ascent;
  U16 descender = font_metrics.descent;
  S16 line_height = font_metrics.lineGap;

  S32 width = 64;
  S32 height = 64;
  IDWriteBitmapRenderTarget *render_target = 0;
  error = IDWriteGdiInterop_CreateBitmapRenderTarget(gdi_interop, 0, width, height, &render_target);
  DWCheckPtr(render_target, Assert(!"render_target"));

  HDC dc = IDWriteBitmapRenderTarget_GetMemoryDC(render_target);
  // NOTE: clear render target
  {
    HGDIOBJ original = SelectObject(dc, GetStockObject(DC_PEN));
    SetDCPenColor(dc, RGB(0, 0, 0));
    SelectObject(dc, GetStockObject(DC_BRUSH));
    SetDCBrushColor(dc, RGB(0, 0, 0));
    Rectangle(dc, 0, 0, width, height);
    SelectObject(dc, original);
  }

  CodepointIndex *first_cp_index = 0;
  CodepointIndex *last_cp_index = 0;
  LooseGlyph *first_glyph = 0;
  LooseGlyph *last_glyph = 0;
  U16 glyph_count = IDWriteFontFace_GetGlyphCount(font_face);
  RangeU32 glyph_idx_rng = rangeu32(0, glyph_count);
  for(U16 glyph_idx = 0; glyph_idx < glyph_count; ++glyph_idx) {   

    DWRITE_GLYPH_RUN glyph_run = {0};
    glyph_run.fontFace = font_face;
    glyph_run.fontEmSize = pixel_per_em;
    glyph_run.glyphCount = 1;
    glyph_run.glyphIndices = &glyph_idx;

    RECT bounding_box = {0};
    error = IDWriteBitmapRenderTarget_DrawGlyphRun(render_target, 0, 0, DWRITE_MEASURING_MODE_NATURAL,
						   &glyph_run, render_params,
						   RGB(255, 255, 255), &bounding_box);
    DWCheck(Assert(!"not good"));
    if(bounding_box.top < 0 || bounding_box.left < 0) continue;
    Assert(bounding_box.right <= width && bounding_box.bottom <= height);

    DWRITE_GLYPH_METRICS glyph_metrics = {0};
    error = IDWriteFontFace_GetDesignGlyphMetrics(font_face, &glyph_idx, 1, &glyph_metrics, 0);
    DWCheck(Assert(!"not good"));

    LooseGlyph *glyph = arena_push_struct(arena, LooseGlyph);
    glyph->width = bounding_box.right - bounding_box.left;
    glyph->height = bounding_box.bottom - bounding_box.top;
    glyph->stride = glyph->width;
    glyph->left_bearing = glyph_metrics.leftSideBearing;
    glyph->top_bearing = glyph_metrics.topSideBearing;
    glyph->advance = (U32)(glyph_metrics.advanceWidth*pixel_per_design_unit);
    glyph->bitmap = arena_push_array_z(arena, U8, glyph->width*glyph->height);

    HBITMAP glyph_bitmap = GetCurrentObject(dc, OBJ_BITMAP);
    DIBSECTION dib = {0};
    GetObject(glyph_bitmap, sizeof(dib), &dib);

    // TODO: blit from the render target bitmap to the glyph bitmap
    U16 src_pixel_size = dib.dsBm.bmBitsPixel/8;
    U32 src_stride = dib.dsBm.bmWidthBytes;
    U8 *src = (U8*)dib.dsBm.bmBits + bounding_box.top*src_stride + bounding_box.left*src_pixel_size;
    U8 *dest = glyph->bitmap;
    for(S32 y = 0; y < glyph->height; ++y) {

      U8 *src_pixel = src + y*src_stride;
      U8 *dest_pixel = dest + y*glyph->stride;
      for(S32 x = 0; x < glyph->width; ++x) {
	*dest_pixel = *src_pixel;
	dest_pixel += 1;
	src_pixel += src_pixel_size;
      }
    }

    SLLQueuePush(first_glyph, last_glyph, glyph);

    // NOTE: clear render target
    {
      HGDIOBJ original = SelectObject(dc, GetStockObject(DC_PEN));
      SetDCPenColor(dc, RGB(0, 0, 0));
      SelectObject(dc, GetStockObject(DC_BRUSH));
      SetDCBrushColor(dc, RGB(0, 0, 0));
      Rectangle(dc, bounding_box.left, bounding_box.top, bounding_box.right, bounding_box.bottom);
      SelectObject(dc, original);
    }
  }
  
  result.first_cp_index = first_cp_index;
  result.last_cp_index = last_cp_index;
  result.first_glyph = first_glyph;
  result.last_glyph = last_glyph;
  result.glyph_idx_rng = glyph_idx_rng;
  result.ascender = ascender;
  result.descender = descender;
  result.line_height = line_height;
 end:
  return(result);
}
