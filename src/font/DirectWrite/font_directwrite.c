// NOTE: internal helpers
#define Win32LogError(error) do { char *msg; FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM, 0, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&msg, 0, 0); OutputDebugStringA(msg); } while(0)
proc B32
dwrite_init(void)
{
  B32 result = 0;
  Arena *arena = arena_alloc();
  dwrite_state = arena_push_struct(arena, DWriteState);
  if(dwrite_state) {
    result = 1;    
    dwrite_state->arena = arena;

    HRESULT error = 0;
#define DWInitCheckPtr(ptr, run) if(((ptr) == 0) || ((error) != S_OK)) { Win32LogError(error); dwrite_state = 0; result = 0; arena_release(arena); run; goto dwrite_init_end; }
    
    // NOTE: create factory
    error = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, &IID_IDWriteFactory1, (IUnknown**)&dwrite_state->factory);
    if(error == S_OK) {
      dwrite_state->factory_version = 1;
    }else {
      error = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, &IID_IDWriteFactory, (IUnknown**)&dwrite_state->factory);
      DWInitCheckPtr(dwrite_state->factory, Assert(!"factory"));
    }

    // NOTE: create render params
    R32 gamma = 1.f;
    error = IDWriteFactory_CreateRenderingParams(dwrite_state->factory, &dwrite_state->default_render_params);
    DWInitCheckPtr(dwrite_state->default_render_params, Assert(!"default_render_params"));
    error =
      IDWriteFactory_CreateCustomRenderingParams
      (
       dwrite_state->factory,
       gamma,
       IDWriteRenderingParams_GetEnhancedContrast(dwrite_state->default_render_params),
       IDWriteRenderingParams_GetClearTypeLevel(dwrite_state->default_render_params),
       IDWriteRenderingParams_GetPixelGeometry(dwrite_state->default_render_params),
       DWRITE_RENDERING_MODE_NATURAL,
       &dwrite_state->render_params
      );
    DWInitCheckPtr(dwrite_state->render_params, Assert(!"render_params"));

    // NOTE: get gdi interop
    error = IDWriteFactory_GetGdiInterop(dwrite_state->factory, &dwrite_state->gdi_interop);
    DWInitCheckPtr(dwrite_state->gdi_interop, Assert(!"gdi_interop"));
  }
 dwrite_init_end:
  return(result);
}

proc DWriteFont
dwrite_open_font(String8 font_path)
{
  HRESULT error = 0;
  IDWriteFontFile *font_file = 0;
  IDWriteFontFace *font_face = 0;

  // NOTE: open file
  {
    ArenaTemp scratch = arena_get_scratch(0, 0);
    String16 font_path_w = str16_from_str8(scratch.arena, font_path);
    error =
      IDWriteFactory_CreateFontFileReference(dwrite_state->factory, font_path_w.string, 0, &font_file);
    if(error != S_OK) {
      Win32LogError(error);
    }
    arena_release_scratch(scratch);
  }

  // NOTE: create font face from file
  if(font_file) {
    error = IDWriteFactory_CreateFontFace(dwrite_state->factory, DWRITE_FONT_FACE_TYPE_TRUETYPE, 1, &font_file, 0, DWRITE_FONT_SIMULATIONS_NONE, &font_face);
    if(error != S_OK) {
      Win32LogError(error);
    }
  }
  
  DWriteFont result = {0};
  result.font_file = font_file;
  result.font_face = font_face;
  return(result);
}

proc void
dwrite_font_metrics(DWriteFont *font, U32 font_size_pt)
{
  DWRITE_FONT_METRICS font_metrics = {0};
  if(font->font_face) {
    IDWriteFontFace_GetMetrics(font->font_face, &font_metrics);
  }

  R32 dpi = 81; // TODO: ???
  R32 pixel_per_em = (R32)font_size_pt*(R32)dpi/72.f;
  R32 pixel_per_design_unit = pixel_per_em/(R32)font_metrics.designUnitsPerEm;
  font->metrics.pixel_per_em = pixel_per_em;
  font->metrics.pixel_per_design_unit = pixel_per_design_unit;
  font->metrics.ascender = font_metrics.ascent;
  font->metrics.descender = font_metrics.descent;
  font->metrics.line_height = font_metrics.lineGap;
}

proc RangeU32
dwrite_font_glyph_map(Arena *arena, DWriteFont *font,
		      CodepointIndex **first, CodepointIndex **last)
{
  HRESULT error = 0;
  CodepointIndex *first_cp_idx = 0;
  CodepointIndex *last_cp_idx = 0;
  U16 glyph_count = 0;
  if(font->font_face) {
    glyph_count = IDWriteFontFace_GetGlyphCount(font->font_face);

    // NOTE: codepoint -> glyph index map    
    if(dwrite_state->factory_version == 1) {
      U32 unicode_rng_count = 0;
      error = IDWriteFontFace1_GetUnicodeRanges((IDWriteFontFace1*)font->font_face, 0, 0, &unicode_rng_count);
      // TODO why doesn't this return OK?
      //if(error == S_OK)
      {
	DWRITE_UNICODE_RANGE *unicode_rngs =
	  arena_push_array(arena, DWRITE_UNICODE_RANGE, unicode_rng_count);
	error =
	  IDWriteFontFace1_GetUnicodeRanges((IDWriteFontFace1*)font->font_face, unicode_rng_count, unicode_rngs, &unicode_rng_count);
	if(error == S_OK) {	  
	  for(U32 unicode_rng_idx = 0; unicode_rng_idx < unicode_rng_count; ++unicode_rng_idx) {
	    DWRITE_UNICODE_RANGE *unicode_rng = unicode_rngs + unicode_rng_idx;
	    for(U32 cp = unicode_rng->first; cp < unicode_rng->last; ++cp) {
	      U16 glyph_idx = 0;	              
	      error = IDWriteFontFace_GetGlyphIndices(font->font_face, &cp, 1, &glyph_idx);
	      if(error == S_OK && glyph_idx != 0) {
		CodepointIndex *cp_idx = arena_push_struct(arena, CodepointIndex);
		cp_idx->codepoint = cp;
		cp_idx->index = glyph_idx;
		SLLQueuePush(first_cp_idx, last_cp_idx, cp_idx);
	      }else {
		// TODO: log error
	      }
	    }
	  }
	}
      }
    }else {
      // TODO: what to do when we can't get the unicode ranges??
    }
  }
  
  *first = first_cp_idx;
  *last = last_cp_idx;
  RangeU32 result = rangeu32(0, glyph_count);
  return(result);
}

proc void
dwrite_rasterize_glyphs(Arena *arena, DWriteFont *font, RangeU32 range,
			LooseGlyph **first, LooseGlyph **last)
{
  HRESULT error = 0;
  LooseGlyph *first_glyph = 0;
  LooseGlyph *last_glyph = 0;

  S32 width = 128;
  S32 height = 128;
  R32 baseline_x = 64.f;
  R32 baseline_y = 64.f;
  IDWriteBitmapRenderTarget *render_target = 0;
  error = IDWriteGdiInterop_CreateBitmapRenderTarget(dwrite_state->gdi_interop, 0, width, height, &render_target);
  Assert(error == S_OK);
  Assert(render_target);
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

  if(font->font_face) {
    for(U32 glyph_idx = range.min; glyph_idx < range.max; ++glyph_idx) {

      DWRITE_GLYPH_RUN glyph_run = {0};
      glyph_run.fontFace = font->font_face;
      glyph_run.fontEmSize = font->metrics.pixel_per_em;
      glyph_run.glyphCount = 1;
      glyph_run.glyphIndices = &(U16)glyph_idx;

      RECT bounding_box = {0};
      error =
	IDWriteBitmapRenderTarget_DrawGlyphRun(render_target, baseline_x, baseline_y,
					       DWRITE_MEASURING_MODE_NATURAL, &glyph_run,
					       dwrite_state->render_params, RGB(255, 255, 255),
					       &bounding_box);      
      Assert(error == S_OK);
      //if(bounding_box.top < 0 || bounding_box.left < 0) continue;
      Assert(bounding_box.left >= 0 && bounding_box.top >= 0);
      Assert(bounding_box.right <= width && bounding_box.bottom <= height);

      DWRITE_GLYPH_METRICS glyph_metrics = {0};
      error =
	IDWriteFontFace_GetDesignGlyphMetrics(font->font_face, &(U16)glyph_idx, 1, &glyph_metrics, 0);
      Assert(error == S_OK);

      LooseGlyph *glyph = arena_push_struct(arena, LooseGlyph);
      glyph->width = bounding_box.right - bounding_box.left;
      glyph->height = bounding_box.bottom - bounding_box.top;
      glyph->stride = glyph->width;
      glyph->left_bearing = bounding_box.left - (S32)baseline_x;//glyph_metrics.leftSideBearing;
      glyph->top_bearing = bounding_box.top - (S32)baseline_y;//glyph_metrics.topSideBearing;
      glyph->advance = (U32)(glyph_metrics.advanceWidth*font->metrics.pixel_per_design_unit);
      glyph->bitmap = arena_push_array_z(arena, U8, glyph->width*glyph->height);

      // NOTE: copy pixels from the render target into the glyph bitmap
      HBITMAP glyph_bitmap = GetCurrentObject(dc, OBJ_BITMAP);
      DIBSECTION dib = {0};
      GetObject(glyph_bitmap, sizeof(dib), &dib);

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
    }
  }
  
  *first = first_glyph;
  *last = last_glyph;
}

// NOTE: implementations
proc LooseFont
font_parse(Arena *arena, String8 font_path, U32 font_size_pt)
{
#if 1
  U64 arena_restore = arena_pos(arena);

  CodepointIndex *first_cp_idx = 0;
  CodepointIndex *last_cp_idx = 0;
  LooseGlyph *first_glyph = 0;
  LooseGlyph *last_glyph = 0;
  RangeU32 glyph_idx_rng = rangeu32_invalid();
  S32 ascender = 0;
  S32 descender = 0;
  U32 line_height = 0;
  
  if(dwrite_init()) {
    DWriteFont font = dwrite_open_font(font_path);
    dwrite_font_metrics(&font, font_size_pt);

    RangeU32 range = dwrite_font_glyph_map(arena, &font, &first_cp_idx, &last_cp_idx);
    if(first_cp_idx == 0) {
      goto font_parse_end;
    }
    
    dwrite_rasterize_glyphs(arena, &font, range, &first_glyph, &last_glyph);
    if(first_glyph == 0) {
      first_cp_idx = 0;
      last_cp_idx = 0;
      arena_pop_to(arena, arena_restore);
      goto font_parse_end;
    }

    glyph_idx_rng = range;
    ascender = font.metrics.ascender;
    descender = font.metrics.descender;
    line_height = font.metrics.line_height;
  }
  
 font_parse_end:  
  LooseFont result = {0};
  result.first_cp_index = first_cp_idx;
  result.last_cp_index = last_cp_idx;
  result.first_glyph = first_glyph;
  result.last_glyph = last_glyph;
  result.glyph_idx_rng = glyph_idx_rng;
  result.ascender = ascender;
  result.descender = descender;
  result.line_height = line_height;
  return(result);
#else
#define DWCheck(run) if(error != S_OK) { Win32LogError(error); error = S_OK; arena_pop_to(arena, arena_restore); run; goto end; }
#define DWCheckPtr(ptr, run) if((ptr == 0) || (error != S_OK)) { Win32LogError(error); error = S_OK; arena_pop_to(arena, arena_restore); run; goto end; }
  LooseFont result = {0};
  HRESULT error = 0;
  U64 arena_restore = arena_pos(arena);

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

  S32 width = 128;
  S32 height = 128;
  S32 baseline_x = 64;
  S32 baseline_y = 64;
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

  // TODO: get glyph codepoints
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
    error = IDWriteBitmapRenderTarget_DrawGlyphRun(render_target, baseline_x, baseline_y,
						   DWRITE_MEASURING_MODE_NATURAL, &glyph_run,
						   render_params, RGB(255, 255, 255), &bounding_box);
    DWCheck(Assert(!"not good"));
    //if(bounding_box.top < 0 || bounding_box.left < 0) continue;
    Assert(bounding_box.left >= 0 && bounding_box.top >= 0);
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
  #endif
}
