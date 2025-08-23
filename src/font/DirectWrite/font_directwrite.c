proc LooseFont
font_parse(Arena *arena, String8 font_path, U32 font_size_pt)
{
  HRESULT error = 0;
  
  IDWriteFactory *factory = 0;
  error = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, &IID_IDWriteFactory, (IUnknown**)&factory);

  IDWriteFontFile *font_file = 0;
  {
    ArenaTemp scratch = arena_get_scratch(&arena, 1);
    String16 font_path_w = str16_from_str8(scratch.arena, font_path);
    error = IDWriteFactory_CreateFontFileReference(factory, font_path_w.string, 0, &font_file);
    arena_release_scratch(scratch);
  }

  IDWriteFontFace *font_face = 0;
  error = IDWriteFactory_CreateFontFace(factory, DWRITE_FONT_FACE_TYPE_TRUETYPE, 1, &font_file, 0, DWRITE_FONT_SIMULATIONS_NONE, &font_face);

  IDWriteRenderingParams *default_render_params = 0;
  error = IDWriteFactory_CreateRenderingParams(factory, &default_render_params);

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
  }

  IDWriteGdiInterop *gdi_interop = 0;
  error = IDWriteFactory_GetGdiInterop(factory, &gdi_interop);

  DWRITE_FONT_METRICS font_metrics = {0};
  IDWriteFontFace_GetMetrics(font_face, &font_metrics);

  R32 dpi = 81; // TODO: ???
  R32 pixel_per_em = (R32)font_size_pt*(R32)dpi/72.f;
  R32 pixel_per_design_unit = pixel_per_em/(R32)font_metrics.designUnitsPerEm;

  U32 width = 64;
  U32 height = 64;
  IDWriteBitmapRenderTarget *render_target = 0;
  error = IDWriteGdiInterop_CreateBitmapRenderTarget(gdi_interop, 0, width, height, &render_target);

  HDC dc = IDWriteBitmapRenderTarget_GetMemoryDC(render_target);

  U16 glyph_count = IDWriteFontFace_GetGlyphCount(font_face);
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

    DWRITE_GLYPH_METRICS glyph_metrics = {0};
    error = IDWriteFontFace_GetDesignGlyphMetrics(font_face, &glyph_idx, 1, &glyph_metrics, 0);

    LooseGlyph *glyph = arena_push_struct(arena, LooseGlyph);
    glyph->width = bounding_box.right - bounding_box.left;
    glyph->height = bounding_box.bottom - bounding_box.top;
    glyph->stride = glyph->width;
    glyph->left_bearing = glyph_metrics.leftSideBearing;
    glyph->top_bearing = glyph_metrics.topSideBearing;
    glyph->advance = (U32)(glyph_metrics.advanceWidth*pixel_per_design_unit);
    glyph->bitmap = arena_push_array(arena, U8, glyph->width*glyph->height);

    HBITMAP glyph_bitmap = GetCurrentObject(dc, OBJ_BITMAP);
    DIBSECTION dib = {0};
    GetObject(glyph_bitmap, sizeof(dib), &dib);

    // TODO: blit from the render target bitmap to the glyph bitmap
  }
  
  LooseFont result = {0};
  return(result);
}
