#include <base/base.h>

#include <base/base.c>

// -----------------------------------------------------------------------------
// CoreGraphics types

//#include <CoreGraphics/CoreGraphics.h>

// NOTE: taken from CoreGraphics header CGBase.h
#if defined(__LP64__) && __LP64__
typedef double CGFloat;
#else
typedef float CGFloat;
#endif

typedef struct CGPoint
{
  CGFloat x;
  CGFloat y;
} CGPoint;

typedef struct CGSize
{
  CGFloat width;
  CGFloat height;
} CGSize;

typedef struct CGRect
{
  CGPoint origin;
  CGSize size;
} CGRect;

typedef struct CGContext *CGContextRef;
typedef struct CGDataProvider *CGDataProviderRef;
typedef struct CGImage *CGImageRef;
typedef struct CGColorSpace *CGColorSpaceRef;

// -----------------------------------------------------------------------------
// CoreGraphics constants

#define CG_Enum(name, type)\
  typedef type name;\
  enum

CG_Enum(CGColorRenderingIntent, S32)
{
  kCGRenderingIntentDefault,
  kCGRenderingIntentAbsoluteColorimetric,
  kCGRenderingIntentRelativeColorimetric,
  kCGRenderingIntentPerceptual,
  kCGRenderingIntentSaturation,
};

CG_Enum(CGImageAlphaInfo, U32)
{
  kCGImageAlphaNone,               /* For example, RGB. */
  kCGImageAlphaPremultipliedLast,  /* For example, premultiplied RGBA */
  kCGImageAlphaPremultipliedFirst, /* For example, premultiplied ARGB */
  kCGImageAlphaLast,               /* For example, non-premultiplied RGBA */
  kCGImageAlphaFirst,              /* For example, non-premultiplied ARGB */
  kCGImageAlphaNoneSkipLast,       /* For example, RGBX. */
  kCGImageAlphaNoneSkipFirst,      /* For example, XRGB. */
  kCGImageAlphaOnly                /* No color data, alpha data only */
};

CG_Enum(CGImageByteOrderInfo, U32)
{
  kCGImageByteOrderMask     = 0x7000,
  kCGImageByteOrderDefault  = 0 << 12,
  kCGImageByteOrder16Little = 1 << 12,
  kCGImageByteOrder32Little = 2 << 12,
  kCGImageByteOrder16Big    = 3 << 12,
  kCGImageByteOrder32Big    = 4 << 12,
};

CG_Enum(CGBitmapInfo, U32)
{
  kCGBitmapAlphaInfoMask = 0x1F,

  kCGBitmapFloatInfoMask = 0xF00,
  kCGBitmapFloatComponents = 1 << 8,

  kCGBitmapByteOrderMask     = kCGImageByteOrderMask,
  kCGBitmapByteOrderDefault  = kCGImageByteOrderDefault,
  kCGBitmapByteOrder16Little = kCGImageByteOrder16Little,
  kCGBitmapByteOrder32Little = kCGImageByteOrder32Little,
  kCGBitmapByteOrder16Big    = kCGImageByteOrder16Big,
  kCGBitmapByteOrder32Big    = kCGImageByteOrder32Big,
};

typedef void (*CGDataProviderReleaseDataCallback)(void *info, const void *data, size_t size);

// -----------------------------------------------------------------------------
// CoreGrpahics functions

proc inline CGRect CGRectMake(CGFloat x, CGFloat y, CGFloat width, CGFloat height)
{ return (CGRect){ .origin = { .x = x, .y = y }, .size = { .width = width, .height = height, } }; }

// CGContext
extern CGContextRef CGBitmapContextCreate(void *data, size_t width, size_t height, size_t bits_per_component, size_t bytes_per_row, CGColorSpaceRef color_space, CGBitmapInfo bitmap_info);

extern CGImageRef CGBitmapContextCreateImage(CGContextRef context);

extern void CGContextRelease(CGContextRef c);

// CGImage
extern CGImageRef CGImageCreate(size_t width, size_t height, size_t bits_per_component, size_t bits_per_pixel, size_t bytes_per_row, CGColorSpaceRef color_space, CGBitmapInfo bitmap_info, CGDataProviderRef provider, const CGFloat *decode, bool should_interpolate, CGColorRenderingIntent intent);

extern CGImageRef CGImageCreateWithImageInRect(CGImageRef image, CGRect rect);

extern void CGImageRelease(CGImageRef image);

// CGColorSpace
extern CGColorSpaceRef CGColorSpaceCreateDeviceRGB(void);

extern void CGColorSpaceRelease(CGColorSpaceRef space);

// CGDataProvider
extern CGDataProviderRef CGDataProviderCreateWithData(void *info, const void *data, size_t size, CGDataProviderReleaseDataCallback release_data);

extern void CGDataProviderRelease(CGDataProviderRef data_provider);

// -----------------------------------------------------------------------------
// CoreFoundation types

typedef long CFIndex;
typedef unsigned char Boolean;
typedef unsigned long CFHashCode;

typedef const void *CFTypeRef;
typedef const struct __CFAllocator *CFAllocatorRef;
typedef const struct __CFDictionary *CFDictionaryRef;
typedef const struct __CFString *CFStringRef;

typedef const void* (*CFDictionaryRetainCallback)(CFAllocatorRef allocator, const void *value);
typedef void (*CFDictionaryReleaseCallback)(CFAllocatorRef allocator, const void *value);
typedef CFStringRef (*CFDictionaryCopyDescriptionCallback)(const void *value);
typedef Boolean (*CFDictionaryEqualCallback)(const void *value1, const void *value2);
typedef CFHashCode (*CFDictionaryHashCallback)(const void *value);

typedef struct CFDictionaryKeyCallbacks
{
  CFIndex version;
  CFDictionaryRetainCallback retain;
  CFDictionaryReleaseCallback release;
  CFDictionaryCopyDescriptionCallback copy_description;
  CFDictionaryEqualCallback equal;
  CFDictionaryHashCallback hash;
} CFDictionaryKeyCallbacks;

typedef struct CFDictionaryValueCallbacks
{
  CFIndex version;
  CFDictionaryRetainCallback retain;
  CFDictionaryReleaseCallback release;
  CFDictionaryCopyDescriptionCallback copy_description;
  CFDictionaryEqualCallback equal;
} CFDictionaryValueCallbacks;

// -----------------------------------------------------------------------------
// CoreFoundation constants

extern CFAllocatorRef const kCFAllocatorDefault;

extern const CFDictionaryKeyCallbacks kCFTypeDictionaryKeyCallBacks;
extern const CFDictionaryValueCallbacks kCFTypeDictionaryValueCallBacks;

// -----------------------------------------------------------------------------
// CoreFoundation functions

extern CFDictionaryRef CFDictionaryCreate(CFAllocatorRef allocator, const void **keys, const void **values, CFIndex num_values, const CFDictionaryKeyCallbacks *key_callbacks, const CFDictionaryValueCallbacks *value_callbacks);

extern void CFRelease(CFTypeRef cf);

// -----------------------------------------------------------------------------
// CoreVideo types

typedef U32 FourCharCode;
typedef FourCharCode OSType;

typedef struct __CVBuffer *CVBufferRef;
typedef CVBufferRef CVImageBufferRef;
typedef CVImageBufferRef CVPixelBufferRef;

typedef S32 CVReturn;
typedef U64 CVOptionFlags;

// -----------------------------------------------------------------------------
// CoreVideo constants

#define CV_Enum(name, type)\
  typedef type name;\
  enum

CV_Enum(CVPixelFormatType, OSType)
{
  kCVPixelFormatType_1Monochrome    = 0x00000001, /* 1 bit indexed */
  kCVPixelFormatType_2Indexed       = 0x00000002, /* 2 bit indexed */
  kCVPixelFormatType_4Indexed       = 0x00000004, /* 4 bit indexed */
  kCVPixelFormatType_8Indexed       = 0x00000008, /* 8 bit indexed */
  kCVPixelFormatType_1IndexedGray_WhiteIsZero = 0x00000021, /* 1 bit indexed gray, white is zero */
  kCVPixelFormatType_2IndexedGray_WhiteIsZero = 0x00000022, /* 2 bit indexed gray, white is zero */
  kCVPixelFormatType_4IndexedGray_WhiteIsZero = 0x00000024, /* 4 bit indexed gray, white is zero */
  kCVPixelFormatType_8IndexedGray_WhiteIsZero = 0x00000028, /* 8 bit indexed gray, white is zero */
  kCVPixelFormatType_16BE555        = 0x00000010, /* 16 bit BE RGB 555 */
  kCVPixelFormatType_16LE555        = 'L555',     /* 16 bit LE RGB 555 */
  kCVPixelFormatType_16LE5551       = '5551',     /* 16 bit LE RGB 5551 */
  kCVPixelFormatType_16BE565        = 'B565',     /* 16 bit BE RGB 565 */
  kCVPixelFormatType_16LE565        = 'L565',     /* 16 bit LE RGB 565 */
  kCVPixelFormatType_24RGB          = 0x00000018, /* 24 bit RGB */
  kCVPixelFormatType_24BGR          = '24BG',     /* 24 bit BGR */
  kCVPixelFormatType_32ARGB         = 0x00000020, /* 32 bit ARGB */
  kCVPixelFormatType_32BGRA         = 'BGRA',     /* 32 bit BGRA */
  kCVPixelFormatType_32ABGR         = 'ABGR',     /* 32 bit ABGR */
  kCVPixelFormatType_32RGBA         = 'RGBA',     /* 32 bit RGBA */
  kCVPixelFormatType_64ARGB         = 'b64a',     /* 64 bit ARGB, 16-bit big-endian samples */
  kCVPixelFormatType_64RGBALE       = 'l64r',     /* 64 bit RGBA, 16-bit little-endian full-range (0-65535) samples */
  kCVPixelFormatType_48RGB          = 'b48r',     /* 48 bit RGB, 16-bit big-endian samples */
  kCVPixelFormatType_32AlphaGray    = 'b32a',     /* 32 bit AlphaGray, 16-bit big-endian samples, black is zero */
  kCVPixelFormatType_16Gray         = 'b16g',     /* 16 bit Grayscale, 16-bit big-endian samples, black is zero */
  kCVPixelFormatType_30RGB          = 'R10k',     /* 30 bit RGB, 10-bit big-endian samples, 2 unused padding bits (at least significant end). */
  kCVPixelFormatType_30RGB_r210     = 'r210',     /* 30 bit RGB, 10-bit big-endian samples, 2 unused padding bits (at most significant end), video-range (64-940). */
  kCVPixelFormatType_422YpCbCr8     = '2vuy',     /* Component Y'CbCr 8-bit 4:2:2, ordered Cb Y'0 Cr Y'1 */
  kCVPixelFormatType_4444YpCbCrA8   = 'v408',     /* Component Y'CbCrA 8-bit 4:4:4:4, ordered Cb Y' Cr A */
  kCVPixelFormatType_4444YpCbCrA8R  = 'r408',     /* Component Y'CbCrA 8-bit 4:4:4:4, rendering format. full range alpha, zero biased YUV, ordered A Y' Cb Cr */
  kCVPixelFormatType_4444AYpCbCr8   = 'y408',     /* Component Y'CbCrA 8-bit 4:4:4:4, ordered A Y' Cb Cr, full range alpha, video range Y'CbCr. */
  kCVPixelFormatType_4444AYpCbCr16  = 'y416',     /* Component Y'CbCrA 16-bit 4:4:4:4, ordered A Y' Cb Cr, full range alpha, video range Y'CbCr, 16-bit little-endian samples. */
  kCVPixelFormatType_4444AYpCbCrFloat    = 'r4fl',  /* Component AY'CbCr single precision floating-point 4:4:4:4 */
  kCVPixelFormatType_444YpCbCr8     = 'v308',     /* Component Y'CbCr 8-bit 4:4:4, ordered Cr Y' Cb, video range Y'CbCr */
  kCVPixelFormatType_422YpCbCr16    = 'v216',     /* Component Y'CbCr 10,12,14,16-bit 4:2:2 */
  kCVPixelFormatType_422YpCbCr10    = 'v210',     /* Component Y'CbCr 10-bit 4:2:2 */
  kCVPixelFormatType_444YpCbCr10    = 'v410',     /* Component Y'CbCr 10-bit 4:4:4 */
  kCVPixelFormatType_420YpCbCr8Planar = 'y420',   /* Planar Component Y'CbCr 8-bit 4:2:0.  baseAddr points to a big-endian CVPlanarPixelBufferInfo_YCbCrPlanar struct */
  kCVPixelFormatType_420YpCbCr8PlanarFullRange    = 'f420',   /* Planar Component Y'CbCr 8-bit 4:2:0, full range.  baseAddr points to a big-endian CVPlanarPixelBufferInfo_YCbCrPlanar struct */
  kCVPixelFormatType_422YpCbCr_4A_8BiPlanar = 'a2vy', /* First plane: Video-range Component Y'CbCr 8-bit 4:2:2, ordered Cb Y'0 Cr Y'1; second plane: alpha 8-bit 0-255 */
  kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange = '420v', /* Bi-Planar Component Y'CbCr 8-bit 4:2:0, video-range (luma=[16,235] chroma=[16,240]).  baseAddr points to a big-endian CVPlanarPixelBufferInfo_YCbCrBiPlanar struct */
  kCVPixelFormatType_420YpCbCr8BiPlanarFullRange  = '420f', /* Bi-Planar Component Y'CbCr 8-bit 4:2:0, full-range (luma=[0,255] chroma=[1,255]).  baseAddr points to a big-endian CVPlanarPixelBufferInfo_YCbCrBiPlanar struct */
  kCVPixelFormatType_422YpCbCr8BiPlanarVideoRange = '422v', /* Bi-Planar Component Y'CbCr 8-bit 4:2:2, video-range (luma=[16,235] chroma=[16,240]).  baseAddr points to a big-endian CVPlanarPixelBufferInfo_YCbCrBiPlanar struct */
  kCVPixelFormatType_422YpCbCr8BiPlanarFullRange  = '422f', /* Bi-Planar Component Y'CbCr 8-bit 4:2:2, full-range (luma=[0,255] chroma=[1,255]).  baseAddr points to a big-endian CVPlanarPixelBufferInfo_YCbCrBiPlanar struct */
  kCVPixelFormatType_444YpCbCr8BiPlanarVideoRange = '444v', /* Bi-Planar Component Y'CbCr 8-bit 4:4:4, video-range (luma=[16,235] chroma=[16,240]).  baseAddr points to a big-endian CVPlanarPixelBufferInfo_YCbCrBiPlanar struct */
  kCVPixelFormatType_444YpCbCr8BiPlanarFullRange  = '444f', /* Bi-Planar Component Y'CbCr 8-bit 4:4:4, full-range (luma=[0,255] chroma=[1,255]).  baseAddr points to a big-endian CVPlanarPixelBufferInfo_YCbCrBiPlanar struct */
  kCVPixelFormatType_422YpCbCr8_yuvs = 'yuvs',     /* Component Y'CbCr 8-bit 4:2:2, ordered Y'0 Cb Y'1 Cr */
  kCVPixelFormatType_422YpCbCr8FullRange = 'yuvf', /* Component Y'CbCr 8-bit 4:2:2, full range, ordered Y'0 Cb Y'1 Cr */
  kCVPixelFormatType_OneComponent8  = 'L008',     /* 8 bit one component, black is zero */
  kCVPixelFormatType_TwoComponent8  = '2C08',     /* 8 bit two component, black is zero */
  kCVPixelFormatType_30RGBLEPackedWideGamut = 'w30r', /* little-endian RGB101010, 2 MSB are ignored, wide-gamut (384-895) */
  kCVPixelFormatType_ARGB2101010LEPacked = 'l10r',     /* little-endian ARGB2101010 full-range ARGB */
  kCVPixelFormatType_40ARGBLEWideGamut      = 'w40a', /* little-endian ARGB10101010, each 10 bits in the MSBs of 16bits, wide-gamut (384-895, including alpha) */
  kCVPixelFormatType_40ARGBLEWideGamutPremultiplied = 'w40m', /* little-endian ARGB10101010, each 10 bits in the MSBs of 16bits, wide-gamut (384-895, including alpha). Alpha premultiplied */
  kCVPixelFormatType_OneComponent10      = 'L010',     /* 10 bit little-endian one component, stored as 10 MSBs of 16 bits, black is zero */
  kCVPixelFormatType_OneComponent12      = 'L012',     /* 12 bit little-endian one component, stored as 12 MSBs of 16 bits, black is zero */
  kCVPixelFormatType_OneComponent16      = 'L016',     /* 16 bit little-endian one component, black is zero */
  kCVPixelFormatType_TwoComponent16      = '2C16',     /* 16 bit little-endian two component, black is zero */
  kCVPixelFormatType_OneComponent16Half  = 'L00h',     /* 16 bit one component IEEE half-precision float, 16-bit little-endian samples */
  kCVPixelFormatType_OneComponent32Float = 'L00f',     /* 32 bit one component IEEE float, 32-bit little-endian samples */
  kCVPixelFormatType_TwoComponent16Half  = '2C0h',     /* 16 bit two component IEEE half-precision float, 16-bit little-endian samples */
  kCVPixelFormatType_TwoComponent32Float = '2C0f',     /* 32 bit two component IEEE float, 32-bit little-endian samples */
  kCVPixelFormatType_64RGBAHalf          = 'RGhA',     /* 64 bit RGBA IEEE half-precision float, 16-bit little-endian samples */
  kCVPixelFormatType_128RGBAFloat        = 'RGfA',     /* 128 bit RGBA IEEE float, 32-bit little-endian samples */
  kCVPixelFormatType_14Bayer_GRBG        = 'grb4',     /* Bayer 14-bit Little-Endian, packed in 16-bits, ordered G R G R... alternating with B G B G... */
  kCVPixelFormatType_14Bayer_RGGB        = 'rgg4',     /* Bayer 14-bit Little-Endian, packed in 16-bits, ordered R G R G... alternating with G B G B... */
  kCVPixelFormatType_14Bayer_BGGR        = 'bgg4',     /* Bayer 14-bit Little-Endian, packed in 16-bits, ordered B G B G... alternating with G R G R... */
  kCVPixelFormatType_14Bayer_GBRG        = 'gbr4',     /* Bayer 14-bit Little-Endian, packed in 16-bits, ordered G B G B... alternating with R G R G... */
  kCVPixelFormatType_DisparityFloat16	 = 'hdis',     /* IEEE754-2008 binary16 (half float), describing the normalized shift when comparing two images. Units are 1/meters: ( pixelShift / (pixelFocalLength * baselineInMeters) ) */
  kCVPixelFormatType_DisparityFloat32	 = 'fdis',     /* IEEE754-2008 binary32 float, describing the normalized shift when comparing two images. Units are 1/meters: ( pixelShift / (pixelFocalLength * baselineInMeters) ) */
  kCVPixelFormatType_DepthFloat16	     = 'hdep',     /* IEEE754-2008 binary16 (half float), describing the depth (distance to an object) in meters */
  kCVPixelFormatType_DepthFloat32	     = 'fdep',     /* IEEE754-2008 binary32 float, describing the depth (distance to an object) in meters */
  kCVPixelFormatType_420YpCbCr10BiPlanarVideoRange = 'x420', /* 2 plane YCbCr10 4:2:0, each 10 bits in the MSBs of 16bits, video-range (luma=[64,940] chroma=[64,960]) */
  kCVPixelFormatType_422YpCbCr10BiPlanarVideoRange = 'x422', /* 2 plane YCbCr10 4:2:2, each 10 bits in the MSBs of 16bits, video-range (luma=[64,940] chroma=[64,960]) */
  kCVPixelFormatType_444YpCbCr10BiPlanarVideoRange = 'x444', /* 2 plane YCbCr10 4:4:4, each 10 bits in the MSBs of 16bits, video-range (luma=[64,940] chroma=[64,960]) */
  kCVPixelFormatType_420YpCbCr10BiPlanarFullRange  = 'xf20', /* 2 plane YCbCr10 4:2:0, each 10 bits in the MSBs of 16bits, full-range (Y range 0-1023) */
  kCVPixelFormatType_422YpCbCr10BiPlanarFullRange  = 'xf22', /* 2 plane YCbCr10 4:2:2, each 10 bits in the MSBs of 16bits, full-range (Y range 0-1023) */
  kCVPixelFormatType_444YpCbCr10BiPlanarFullRange  = 'xf44', /* 2 plane YCbCr10 4:4:4, each 10 bits in the MSBs of 16bits, full-range (Y range 0-1023) */
  kCVPixelFormatType_420YpCbCr8VideoRange_8A_TriPlanar   = 'v0a8', /* first and second planes as per 420YpCbCr8BiPlanarVideoRange (420v), alpha 8 bits in third plane full-range.  No CVPlanarPixelBufferInfo struct. */
  kCVPixelFormatType_16VersatileBayer    = 'bp16',   /* Single plane Bayer 16-bit little-endian sensor element ("sensel") samples from full-size decoding of ProRes RAW images; Bayer pattern (sensel ordering) and other raw conversion information is described via buffer attachments */
  kCVPixelFormatType_64RGBA_DownscaledProResRAW    = 'bp64',   /* Single plane 64-bit RGBA (16-bit little-endian samples) from downscaled decoding of ProRes RAW images; components--which may not be co-sited with one another--are sensel values and require raw conversion, information for which is described via buffer attachments */
  kCVPixelFormatType_422YpCbCr16BiPlanarVideoRange       = 'sv22', /* 2 plane YCbCr16 4:2:2, video-range (luma=[4096,60160] chroma=[4096,61440]) */
  kCVPixelFormatType_444YpCbCr16BiPlanarVideoRange       = 'sv44', /* 2 plane YCbCr16 4:4:4, video-range (luma=[4096,60160] chroma=[4096,61440]) */
  kCVPixelFormatType_444YpCbCr16VideoRange_16A_TriPlanar = 's4as', /* 3 plane video-range YCbCr16 4:4:4 with 16-bit full-range alpha (luma=[4096,60160] chroma=[4096,61440] alpha=[0,65535]).  No CVPlanarPixelBufferInfo struct. */
};

CV_Enum(CVPixelBufferLockFlags, CVOptionFlags)
{
  kCVPixelBufferLock_ReadWrite,
  kCVPixelBufferLock_ReadOnly,
};

extern CFStringRef const kCVPixelBufferPixelFormatTypeKey;
extern CFStringRef const kCVPixelBufferMemoryAllocatorKey;
extern CFStringRef const kCVPixelBufferWidthKey;
extern CFStringRef const kCVPixelBufferHeightKey;
extern CFStringRef const kCVPixelBufferCGBitmapContextCompatibilityKey;
extern CFStringRef const kCVPixelBufferCGImageCompatibilityKey;
extern CFStringRef const kCVPixelBufferOpenGLCompatibilityKey;
extern CFStringRef const kCVPixelBufferIOSurfacePropertiesKey;
extern CFStringRef const kCVPixelBufferOpenGLESCompatibilityKey;
extern CFStringRef const kCVPixelBufferMetalCompatibilityKey;
extern CFStringRef const kCVPixelBufferOpenGLTexturCacheCompatibilityKey;
extern CFStringRef const kCVPixelBufferOpenGLESTextureCacheCompatibilityKey;

// -----------------------------------------------------------------------------
// CoreVideo functions

extern CVReturn CVPixelBufferCreate(CFAllocatorRef allocator, size_t width, size_t height, OSType pixel_format_type, CFDictionaryRef pixel_buffer_attributes, CVPixelBufferRef *pixel_buffer_out);

extern void* CVPixelBufferGetBaseAddress(CVPixelBufferRef pixel_buffer);
extern size_t CVPixelBufferGetBytesPerRow(CVPixelBufferRef pixel_buffer);

extern CVReturn CVPixelBufferLockBaseAddress(CVPixelBufferRef pixel_buffer, CVPixelBufferLockFlags lock_flags);
extern CVReturn CVPixelBufferUnlockBaseAddress(CVPixelBufferRef pixel_buffer, CVPixelBufferLockFlags lock_flags);

extern void CVPixelBufferRelease(CVPixelBufferRef texture);

// -----------------------------------------------------------------------------
// AppKit types

typedef void NSApplication;
typedef void NSWindow;
typedef void NSView;
typedef void NSEvent;
typedef void NSString;
typedef void NSDate;

typedef NSString *NSRunLoopMode;
global NSRunLoopMode NSDefaultRunLoopMode;
global NSRunLoopMode NSRunLoopCommonModes;

typedef double   NSTimeInterval;

typedef CGPoint NSPoint;
typedef CGSize  NSSize;
typedef CGRect  NSRect;

// -----------------------------------------------------------------------------
// CoreAnimation types

typedef void CALayer;
typedef NSString *CALayerContentsGravity;

// -----------------------------------------------------------------------------
// CoreAnimation constants

extern CALayerContentsGravity const kCAGravityCenter;
extern CALayerContentsGravity const kCAGravityTop;
extern CALayerContentsGravity const kCAGravityBottom;
extern CALayerContentsGravity const kCAGravityLeft;
extern CALayerContentsGravity const kCAGravityRight;
extern CALayerContentsGravity const kCAGravityTopLeft;
extern CALayerContentsGravity const kCAGravityTopRight;
extern CALayerContentsGravity const kCAGravityBottomLeft;
extern CALayerContentsGravity const kCAGravityBottomRight;
extern CALayerContentsGravity const kCAGravityResize;
extern CALayerContentsGravity const kCAGravityResizeAspect;
extern CALayerContentsGravity const kCAGravityResizeAspectFill;

// -----------------------------------------------------------------------------
// CoreAnimation functions

// CALayer
proc inline id
CALayer_contents(CALayer *layer)
{
  id nsid = layer;
  SEL nssel = mac_state->sels[MacSelector_contents];
  return ((id (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline void
CALayer_setContents(CALayer *layer, id contents)
{
  id nsid = layer;
  SEL nssel = mac_state->sels[MacSelector_setContents];
  return ((void (*)(id, SEL, id))objc_msgSend)(nsid, nssel, contents);
}

proc inline CGRect
CALayer_contentsRect(CALayer *layer)
{
  id nsid = layer;
  SEL nssel = mac_state->sels[MacSelector_contentsRect];
  return ((CGRect (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline void
CALayer_setContentsRect(CALayer *layer, CGRect contents_rect)
{
  id nsid = layer;
  SEL nssel = mac_state->sels[MacSelector_setContentsRect];
  return ((void (*)(id, SEL, CGRect))objc_msgSend)(nsid, nssel, contents_rect);
}

proc inline CALayerContentsGravity
CALayer_contentsGravity(CALayer *layer)
{
  id nsid = layer;
  SEL nssel = mac_state->sels[MacSelector_contentsGravity];
  return ((NSString* (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline void
CALayer_setContentsGravity(CALayer *layer, CALayerContentsGravity contents_gravity)
{
  id nsid = layer;
  SEL nssel = mac_state->sels[MacSelector_setContentsGravity];
  return ((void (*)(id, SEL, NSString*))objc_msgSend)(nsid, nssel, contents_gravity);
}

proc inline BOOL
CALayer_needsDisplay(CALayer *layer)
{
  id nsid = layer;
  SEL nssel = mac_state->sels[MacSelector_needsDisplay];
  return ((BOOL (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline void
CALayer_setNeedsDisplay(CALayer *layer)
{
  id nsid = layer;
  SEL nssel = mac_state->sels[MacSelector_setNeedsDisplay];
  return ((void (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

// CATransaction
proc inline void
CATransaction_begin(void)
{
  Class nsclass = mac_state->classes[MacClass_CATransaction];
  id nsid = (id)nsclass;
  SEL nssel = mac_state->sels[MacSelector_begin];
  return ((void (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline void
CATransaction_commit(void)
{
  Class nsclass = mac_state->classes[MacClass_CATransaction];
  id nsid = (id)nsclass;
  SEL nssel = mac_state->sels[MacSelector_commit];
  return ((void (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline void
CATransaction_flush(void)
{
  Class nsclass = mac_state->classes[MacClass_CATransaction];
  id nsid = (id)nsclass;
  SEL nssel = mac_state->sels[MacSelector_flush];
  return ((void (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline BOOL
CATransaction_disableActions(void)
{
  Class nsclass = mac_state->classes[MacClass_CATransaction];
  id nsid = (id)nsclass;
  SEL nssel = mac_state->sels[MacSelector_disableActions];
  return ((BOOL (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline void
CATransaction_setDisableActions(BOOL disable_actions)
{
  Class nsclass = mac_state->classes[MacClass_CATransaction];
  id nsid = (id)nsclass;
  SEL nssel = mac_state->sels[MacSelector_setDisableActions];
  return ((void (*)(id, SEL, BOOL))objc_msgSend)(nsid, nssel, disable_actions);
}

// -----------------------------------------------------------------------------
// AppKit constants

#define NS_Enum(name, type)\
  typedef type name;\
  enum

NS_Enum(NSApplicationActivationPolicy, NSInteger)
{
  NSApplicationActivationPolicyRegular,
  NSApplicationActivationPolicyAccessory,
  NSApplicationActivationPolicyProhibited,
};

// TODO: API_AVAILABLE macros
NS_Enum(NSWindowStyleMask, NSUInteger)
{
  NSWindowStyleMaskBorderless             = 0,
  NSWindowStyleMaskTitled                 = 1 <<  0,
  NSWindowStyleMaskClosable               = 1 <<  1,
  NSWindowStyleMaskMiniaturizable         = 1 <<  2,
  NSWindowStyleMaskResizable              = 1 <<  3,
  NSWindowStyleMaskTexturedBackground     = 1 <<  8, /* NOTE: deprecated */
  NSWindowStyleMaskUnifiedTitleAndToolbar = 1 << 12,
  NSWindowStyleMaskFullScreen             = 1 << 14,
  NSWindowStyleMaskFullSizeContentView    = 1 << 15,
  NSWindowStyleMaskUtilityWindow          = 1 <<  4,
  NSWindowStyleMaskDocModalWindow         = 1 <<  6,
  NSWindowStyleMaskNonactivatingPanel     = 1 <<  7,
  NSWindowStyleMaskHUDWindow              = 1 << 13,
};

NS_Enum(NSBackingStoreType, NSUInteger)
{
  NSBackingStoreRetained, 	 /* NOTE: deprecated */
  NSBackingStoreNonretained,     /* NOTE: deprecated */
  NSBackingStoreBuffered,
};

// TODO: generate strings for each event type
NS_Enum(NSEventType, NSUInteger)
{
  NSEventTypeNone                       =  0,

  NSEventTypeLeftMouseDown              =  1,
  NSEventTypeLeftMouseUp                =  2,

  NSEventTypeRightMouseDown             =  3,
  NSEventTypeRightMouseUp               =  4,

  NSEventTypeMouseMoved                 =  5,

  NSEventTypeLeftMouseDragged           =  6,
  NSEventTypeRightMouseDragged          =  7,

  NSEventTypeMouseEntered               =  8,
  NSEventTypeMouseExited                =  9,

  NSEventTypeKeyDown                    = 10,
  NSEventTypeKeyUp                      = 11,

  NSEventTypeFlagsChanged               = 12,

  NSEventTypeAppKitDefined              = 13,
  NSEventTypeSystemDefined              = 14,
  NSEventTypeApplicationDefined         = 15,

  NSEventTypePeriodic                   = 16,

  NSEventTypeCursorUpdate               = 17,

  NSEventTypeScrollWheel                = 22,

  NSEventTypeTabletPoint                = 23,
  NSEventTypeTabletProximity            = 24,

  NSEventTypeOtherMouseDown             = 25,
  NSEventTypeOtherMouseUp               = 26,
  NSEventTypeOtherMouseDragged          = 27,
};

NS_Enum(NSEventMask, U64)
{
  NSEventMaskAny                        = U64_MAX,
  NSEventMaskLeftMouseDown              = 1 << NSEventTypeLeftMouseDown,
  NSEventMaskLeftMouseDragged           = 1 << NSEventTypeLeftMouseDragged,
  NSEventMaskLeftMouseUp                = 1 << NSEventTypeLeftMouseUp,

  NSEventMaskRightMouseDown             = 1 << NSEventTypeRightMouseDown,
  NSEventMaskRightMouseDragged          = 1 << NSEventTypeRightMouseDragged,
  NSEventMaskRightMouseUp               = 1 << NSEventTypeRightMouseUp,

  NSEventMaskOtherMouseDown             = 1 << NSEventTypeOtherMouseDown,
  NSEventMaskOtherMouseDragged          = 1 << NSEventTypeOtherMouseDragged,
  NSEventMaskOtherMouseUp               = 1 << NSEventTypeOtherMouseUp,

  NSEventMaskMouseEntered               = 1 << NSEventTypeMouseEntered,
  NSEventMaskMouseMoved                 = 1 << NSEventTypeMouseMoved,
  NSEventMaskMouseExited                = 1 << NSEventTypeMouseExited,

  NSEventMaskKeyDown                    = 1 << NSEventTypeKeyDown,
  NSEventMaskKeyUp                      = 1 << NSEventTypeKeyUp,

  NSEventMaskScrollWheel                = 1 << NSEventTypeScrollWheel,

  NSEventMaskFlagsChanged               = 1 << NSEventTypeFlagsChanged,

  NSEventMaskAppKitDefined              = 1 << NSEventTypeAppKitDefined,
  NSEventMaskSystemDefined              = 1 << NSEventTypeSystemDefined,
  NSEventMaskApplicationDefined         = 1 << NSEventTypeApplicationDefined,

  NSEventMaskPeriodic                   = 1 << NSEventTypePeriodic,

  NSEventMaskCursorUpdate               = 1 << NSEventTypeCursorUpdate,

  NSEventMaskTabletPoint                = 1 << NSEventTypeTabletPoint,
  NSEventMaskTabletProximity            = 1 << NSEventTypeTabletProximity,
};

NS_Enum(NSEventSubtype, short)
{
  // NSEventTypeAppKitDefined
  NSEventSubtypeWindowExposed = 0,
  NSEventSubtypeApplicationActivated = 1,
  NSEventSubtypeApplicationDeactivated = 2,
  NSEventSubtypeWindowMoved = 4,
  NSEventSubtypeScreenChanged = 8,

  // NSEventTypeSystemDefined
  NSEventSubtypePowerOff = 1,

  // mouse events
  NSEventSubtypeMouseEvent = 0,
  NSEventSubtypeTabletPoint = 1,
  NSEventSubtypeTabletProximity = 2,
  NSEventSubtypeTouch = 3,
};

NS_Enum(NSEventModifierFlags, NSUInteger)
{
  NSEventModifierFlagCapsLock                   = 1 << 16,
  NSEventModifierFlagShift                      = 1 << 17,
  NSEventModifierFlagControl                    = 1 << 18,
  NSEventModifierFlagOption                     = 1 << 19,
  NSEventModifierFlagCommand                    = 1 << 20,
  NSEventModifierFlagNumericPad                 = 1 << 21,
  NSEventModifierFlagHelp                       = 1 << 22,
  NSEventModifierFlagFunction                   = 1 << 23,
  NSEventModifierFlagDeviceIndependentFlagsMask = 0xFFFF0000UL,
};

NS_Enum(NSEventPhase, NSUInteger)
{
  NSEventPhaseNone              = 0,
  NSEventPhaseBegan             = 1 << 0,
  NSEventPhaseStationary        = 1 << 1,
  NSEventPhaseChanged           = 1 << 2,
  NSEventPhaseEnded             = 1 << 3,
  NSEventPhaseCancelled         = 1 << 4,
  NSEventPhaseMayBegin          = 1 << 5,
};

NS_Enum(NSViewLayerContentsRedrawPolicy, NSInteger)
{
  NSViewLayerContentsRedrawNever,
  NSViewLayerContentsRedrawOnSetNeedsDisplay,
  NSViewLayerContentsRedrawDuringViewResize,
  NSViewLayerContentsRedrawBeforeViewResize,
  NSViewLayerContentsRedrawCrossfade,
};

// TODO: how to generate all this?
// TODO: make sure floating point returns are handled correctly for each abi
// -----------------------------------------------------------------------------
// AppKit functions

#define objc_add_method(class, method, impl, sigstr)\
  class_addMethod(mac_state->classes[Glue(MacClass_, class)], mac_state->sels[Glue(MacSelector_, method)], (IMP)impl, sigstr)

proc inline id
NSAlloc(Class nsclass)
{
  id nsid = (id)nsclass;
  SEL nssel = mac_state->sels[MacSelector_alloc];
  return ((id (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

// -----------------------------------------------------------------------------
// NSApplication

proc inline NSApplication*
NSApplication_sharedApplication(void)
{
  id nsid = (id)mac_state->classes[MacClass_NSApplication];
  SEL nssel = mac_state->sels[MacSelector_sharedApplication];
  return ((id (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline BOOL
NSApplication_setActivationPolicy(NSApplication *app, NSApplicationActivationPolicy activation_policy)
{
  id nsid = app;
  SEL nssel = mac_state->sels[MacSelector_setActivationPolicy];
  return ((BOOL (*)(id, SEL, NSInteger))objc_msgSend)(nsid, nssel, activation_policy);
}

proc inline void
NSApplication_activate(NSApplication *app)
{
  id nsid = app;
  SEL nssel = mac_state->sels[MacSelector_activate];
  return ((void (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline void
NSApplication_finishLaunching(NSApplication *app)
{
  id nsid = app;
  SEL nssel = mac_state->sels[MacSelector_finishLaunching];
  return ((void (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline NSEvent*
NSApplication_nextEventMatchingMask(NSApplication *app, NSEventMask mask, NSDate *expiration, NSRunLoopMode mode, BOOL dequeue)
{
  id nsid = app;
  SEL nssel = mac_state->sels[MacSelector_nextEventMatchingMask];
  return ((id (*)(id, SEL, U64, id, id, BOOL))objc_msgSend)(nsid, nssel, mask, expiration, mode, dequeue);
}

proc inline void
NSApplication_sendEvent(NSApplication *app, NSEvent *event)
{
  id nsid = app;
  SEL nssel = mac_state->sels[MacSelector_sendEvent];
  return ((void (*)(id, SEL, id))objc_msgSend)(nsid, nssel, event);
}

proc inline void
NSApplication_updateWindows(NSApplication *app)
{
  id nsid = app;
  SEL nssel = mac_state->sels[MacSelector_updateWindows];
  return ((void (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

// -----------------------------------------------------------------------------
// NSWindow

proc inline NSWindow*
NSWindow_initWithContentRect(NSRect content_rect, NSWindowStyleMask style_mask, NSBackingStoreType backing, BOOL defer)
{
  Class nsclass = mac_state->classes[MacClass_NSWindow];
  id nsid = NSAlloc(nsclass);
  SEL nssel = mac_state->sels[MacSelector_initWithContentRect];
  return ((id (*)(id, SEL, NSRect, NSUInteger, NSUInteger, BOOL))objc_msgSend)(nsid, nssel, content_rect, style_mask, backing, defer);
}

proc inline NSView*
NSWindow_contentView(NSWindow *window)
{
  id nsid = window;
  SEL nssel = mac_state->sels[MacSelector_contentView];
  return ((id (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline void
NSWindow_setContentView(NSWindow *window, NSView *view)
{
  id nsid = window;
  SEL nssel = mac_state->sels[MacSelector_setContentView];
  return ((void (*)(id, SEL, id))objc_msgSend)(nsid, nssel, view);
}

proc inline void
NSWindow_makeKeyAndOrderFront(NSWindow *window, id sender)
{
  id nsid = window;
  SEL nssel = mac_state->sels[MacSelector_makeKeyAndOrderFront];
  return ((void (*)(id, SEL, id))objc_msgSend)(nsid, nssel, sender);
}

proc inline void
NSWindow_setIsVisible(NSWindow *window, BOOL flag)
{
  id nsid = window;
  SEL nssel = mac_state->sels[MacSelector_setIsVisible];
  return ((void (*)(id, SEL, BOOL))objc_msgSend)(nsid, nssel, flag);
}

// -----------------------------------------------------------------------------
// NSView

proc inline NSView*
NSView_initWithFrame(NSRect frame)
{
  Class nsclass = mac_state->classes[MacClass_NSView];
  id nsid = NSAlloc(nsclass);
  SEL nssel = mac_state->sels[MacSelector_initWithFrame];
  return ((id (*)(id, SEL, NSRect))objc_msgSend)(nsid, nssel, frame);
}

proc inline NSRect
NSView_bounds(NSView *view)
{
  id nsid = view;
  SEL nssel = mac_state->sels[MacSelector_bounds];
  return ((NSRect (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline BOOL
NSView_wantsLayer(NSView *view)
{
  id nsid = view;
  SEL nssel = mac_state->sels[MacSelector_wantsLayer];
  return ((BOOL (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline void
NSView_setWantsLayer(NSView *view, BOOL wants_layer)
{
  id nsid = view;
  SEL nssel = mac_state->sels[MacSelector_setWantsLayer];
  return ((void (*)(id, SEL, BOOL))objc_msgSend)(nsid, nssel, wants_layer);
}

proc inline CALayer*
NSView_layer(NSView *view)
{
  id nsid = view;
  SEL nssel = mac_state->sels[MacSelector_layer];
  return ((id (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline void
NSView_setLayer(NSView *view, CALayer *layer)
{
  id nsid = view;
  SEL nssel = mac_state->sels[MacSelector_setLayer];
  return ((void (*)(id, SEL, id))objc_msgSend)(nsid, nssel, layer);
}

proc inline NSViewLayerContentsRedrawPolicy
NSView_layerContentsRedrawPolicy(NSView *view)
{
  id nsid = view;
  SEL nssel = mac_state->sels[MacSelector_layerContentsRedrawPolicy];
  return ((NSInteger (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline void
NSView_setLayerContentsRedrawPolicy(NSView *view, NSViewLayerContentsRedrawPolicy policy)
{
  id nsid = view;
  SEL nssel = mac_state->sels[MacSelector_setLayerContentsRedrawPolicy];
  return ((void (*)(id, SEL, NSInteger))objc_msgSend)(nsid, nssel, policy);
}

// -----------------------------------------------------------------------------
// NSEvent

proc inline NSEventType
NSEvent_type(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_type];
  return ((NSUInteger (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline NSEventSubtype
NSEvent_subtype(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_subtype];
  return ((short (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline NSPoint
NSEvent_locationInWindow(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_locationInWindow];
  return ((NSPoint (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline NSTimeInterval
NSEvent_timestamp(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_timestamp];
  return ((double (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline NSWindow*
NSEvent_window(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_window];
  return ((id (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline NSInteger
NSEvent_windowNumber(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_windowNumber];
  return ((NSInteger (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

// TODO: event ref

proc inline NSEventModifierFlags
NSEvent_modifierFlags(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_modifierFlags];
  return ((NSUInteger (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

// TODO: global modifier flags?

// TODO: characters, or characters igoring modifiers?

proc inline U16
NSEvent_keyCode(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_keyCode];
  return ((U16 (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline NSTimeInterval
NSEvent_keyRepeatDelay(void)
{
  Class nsclass = mac_state->classes[MacClass_NSEvent];
  id nsid = (id)nsclass;
  SEL nssel = mac_state->sels[MacSelector_keyRepeatDelay];
  return ((double (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline NSTimeInterval
NSEvent_keyRepeatInterval(void)
{
  Class nsclass = mac_state->classes[MacClass_NSEvent];
  id nsid = (id)nsclass;
  SEL nssel = mac_state->sels[MacSelector_keyRepeatInterval];
  return ((double (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline NSUInteger
NSEvent_pressedMouseButtons(void)
{
  Class nsclass = mac_state->classes[MacClass_NSEvent];
  id nsid = (id)nsclass;
  SEL nssel = mac_state->sels[MacSelector_pressedMouseButtons];
  return ((NSUInteger (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline NSTimeInterval
NSEvent_doubleClickInterval(void)
{
  Class nsclass = mac_state->classes[MacClass_NSEvent];
  id nsid = (id)nsclass;
  SEL nssel = mac_state->sels[MacSelector_doubleClickInterval];
  return ((double (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline NSPoint
NSEvent_mouseLocation(void)
{
  Class nsclass = mac_state->classes[MacClass_NSEvent];
  id nsid = (id)nsclass;
  SEL nssel = mac_state->sels[MacSelector_mouseLocation];
  return ((NSPoint (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline NSInteger
NSEvent_buttonNumber(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_buttonNumber];
  return ((NSInteger (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline NSInteger
NSEvent_clickCount(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_clickCount];
  return ((NSInteger (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline CGFloat
NSEvent_deltaX(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_deltaX];
  return ((CGFloat (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline CGFloat
NSEvent_deltaY(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_deltaY];
  return ((CGFloat (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline CGFloat
NSEvent_deltaZ(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_deltaZ];
  return ((CGFloat (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline BOOL
NSEvent_hasPreciseScrollingDeltas(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_hasPreciseScrollingDeltas];
  return ((BOOL (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline CGFloat
NSEvent_scrollingDeltaX(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_scrollingDeltaX];
  return ((CGFloat (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline CGFloat
NSEvent_scrollingDeltaY(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_scrollingDeltaY];
  return ((CGFloat (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline NSEventPhase
NSEvent_momentumPhase(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_momentumPhase];
  return ((NSUInteger (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

proc inline BOOL
NSEvent_isDirectionInvertedFromDevice(NSEvent *event)
{
  id nsid = event;
  SEL nssel = mac_state->sels[MacSelector_isDirectionInvertedFromDevice];
  return ((BOOL (*)(id, SEL))objc_msgSend)(nsid, nssel);
}

// -----------------------------------------------------------------------------
// NSString

proc inline NSString*
NSString_stringWithUTF8String(const char *cstr)
{
  Class nsclass = mac_state->classes[MacClass_NSString];
  id nsid = (id)nsclass;
  SEL nssel = mac_state->sels[MacSelector_stringWithUTF8String];
  return ((id (*)(id, SEL, const char*))objc_msgSend)(nsid, nssel, cstr);
}

// -----------------------------------------------------------------------------
// main

typedef struct Cocoa_PixelBuffer
{
  S32 pixels_width;
  S32 pixels_height;

  CVPixelBufferRef buf;
} Cocoa_PixelBuffer;

global Cocoa_PixelBuffer *backbuffer  = 0;
global Cocoa_PixelBuffer *frontbuffer = 0;

proc void
render(NSView *view, NSRect frame_bounds)
{
  CALayer *layer = NSView_layer(view);
  CALayer_setContents(layer, (id)backbuffer->buf);
  CGFloat norm_w = ClampToRange(frame_bounds.size.width / (CGFloat)backbuffer->pixels_width, 0.0, 1.0);
  CGFloat norm_h = ClampToRange(frame_bounds.size.height / (CGFloat)backbuffer->pixels_height, 0.0, 1.0);
  CGFloat norm_y = ClampToRange(1.0 - norm_h, 0.0, 1.0);

  CGRect contents_rect = CGRectMake(0, norm_y, norm_w, norm_h);
  CALayer_setContentsRect(layer, contents_rect);
  //CALayer_setNeedsDisplay(layer);

  // NOTE: swap buffers
  Cocoa_PixelBuffer *temp = frontbuffer;
  frontbuffer = backbuffer;
  backbuffer = temp;
}

global B32 running = 1;

proc B32
on_window_close(id self, NSWindow *sender)
{
  Unused(self);
  Unused(sender);

  running = 0;
  return(1);
}

proc NSSize
on_window_resize(id self, NSWindow *sender, NSSize frame_size)
{
  Unused(self);
  Unused(sender);

  return(frame_size);
}

typedef struct BoxState
{
  V2 p;
  V2 v;
  V2 dim;
} BoxState;

proc void
update_and_draw(BoxState *box, U32 *pixels, U32 pixel_stride, V2S32 frame_dim)
{
  // clear background
  {
    S32 frame_w = frame_dim.width;
    S32 frame_h = frame_dim.height;
    U8 *dest = (U8*)pixels;
    for(S32 row_idx = 0; row_idx < frame_h; ++row_idx)
    {
      U32 *row = (U32*)(dest + row_idx*pixel_stride);
      for(S32 col_idx = 0; col_idx < frame_w; ++col_idx)
      {
	*row++ = 0xFF080C1C;
      }
    }
  }

  // update box
  if(0 > box->p.x + box->v.x || box->p.x + box->dim.x + box->v.x > frame_dim.width)
  {
    box->v.x = -box->v.x;
  }
  if(0 > box->p.y + box->v.y || box->p.y + box->dim.y + box->v.y > frame_dim.height)
  {
    box->v.y = -box->v.y;
  }

  box->p.x += box->v.x;
  box->p.y += box->v.y;

  // draw box
  {
    S32 box_x = (S32)box->p.x;
    S32 box_y = (S32)box->p.y;
    S32 box_w = (S32)box->dim.x;
    S32 box_h = (S32)box->dim.y;
    U8 *dest = (U8*)pixels + pixel_stride*box_y + box_x*sizeof(*pixels);
    for(S32 row_idx = 0; row_idx < box_h; ++row_idx)
    {
      U32 *row = (U32*)(dest + pixel_stride*row_idx);
      for(S32 col_idx = 0; col_idx < box_w; ++col_idx)
      {
	*row++ = 0xFFFF0000;
      }
    }
  }
}

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  int result = 0;

  if(!os_init())
  { result = 1; goto end; }

  NSDefaultRunLoopMode = NSString_stringWithUTF8String("kCFRunLoopDefaultMode");
  NSRunLoopCommonModes = NSString_stringWithUTF8String("kCFRunLoopCommonModes");

  // init app
  NSApplication *app = 0;
  if((app = NSApplication_sharedApplication()) == 0)
  { result = 1; goto end; }
  if(!NSApplication_setActivationPolicy(app, NSApplicationActivationPolicyRegular))
  { result = 1; goto end; }

  // allocate backbuffer
  Arena *arena = arena_alloc();
  // TODO: get actual screen dim
  S32 screen_width = 1920;
  S32 screen_height = 1080;
  CFAllocatorRef pbuf_allocator = kCFAllocatorDefault;
  OSType pbuf_fmt = kCVPixelFormatType_32BGRA;
  CFStringRef pbuf_attr_iosurface_key = kCVPixelBufferIOSurfacePropertiesKey;
  CFDictionaryRef pbuf_attr_iosurface_val = CFDictionaryCreate(pbuf_allocator, 0, 0, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
  const void *pbuf_attr_keys[] = { pbuf_attr_iosurface_key, };
  const void *pbuf_attr_vals[] = { pbuf_attr_iosurface_val, };
  CFDictionaryRef pbuf_attr = CFDictionaryCreate(pbuf_allocator, pbuf_attr_keys, pbuf_attr_vals, 1, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
  backbuffer = arena_push_struct(arena, Cocoa_PixelBuffer);
  CVPixelBufferCreate(pbuf_allocator, screen_width, screen_height, pbuf_fmt, pbuf_attr, &backbuffer->buf);
  backbuffer->pixels_width = screen_width;
  backbuffer->pixels_height = screen_height;
  //backbuffer->pixels = CVPixelBufferGetBaseAddress(backbuffer->buf);

  frontbuffer = arena_push_struct(arena, Cocoa_PixelBuffer);
  CVPixelBufferCreate(pbuf_allocator, screen_width, screen_height, pbuf_fmt, pbuf_attr, &frontbuffer->buf);
  frontbuffer->pixels_width = screen_width;
  frontbuffer->pixels_height = screen_height;
  //frontbuffer->pixels = CVPixelBufferGetBaseAddress(frontbuffer->buf);

  CFRelease(pbuf_attr_iosurface_val);
  CFRelease(pbuf_attr);
  // NOTE: clear color
  /* { */
  /*   U32 color = 0x080C1CFF; */
  /*   U32 *dest = backbuffer_pixels; */
  /*   for(S32 idx = 0; idx < 2*screen_width*screen_height; ++idx) */
  /*   { */
  /*     *dest++ = color; */
  /*   } */
  /* } */

  // open window
  if(!objc_add_method(NSObject, windowShouldClose, on_window_close, 0))
  { result = 1; goto end; }

  if(!objc_add_method(NSObject, windowWillResize, on_window_resize, "{NSSize=ff}@:{NSSize=ff}"))
  { result = 1; goto end; }

  NSWindow *window = 0;
  NSRect window_rect = {{200, 200}, {200, 200}};
  NSWindowStyleMask window_style_mask = (NSWindowStyleMaskClosable|
					 NSWindowStyleMaskMiniaturizable|
					 NSWindowStyleMaskTitled|
					 NSWindowStyleMaskResizable);
  NSBackingStoreType window_backing = NSBackingStoreBuffered;
  if((window = NSWindow_initWithContentRect(window_rect, window_style_mask, window_backing, false)) == 0)
  { result = 1; goto end; }

  NSView *view = NSWindow_contentView(window);
  NSView_setWantsLayer(view, true);
  //NSView_setLayerContentsRedrawPolicy(view, NSViewLayerContentsRedrawDuringViewResize);
  //NSView_setLayerContentsRedrawPolicy(view, NSViewLayerContentsRedrawNever);

  CALayer *layer = NSView_layer(view);
  CALayer_setContentsGravity(layer, kCAGravityBottomLeft);

  NSApplication_activate(app);
  NSWindow_makeKeyAndOrderFront(window, 0);
  NSWindow_setIsVisible(window, true);

  NSApplication_finishLaunching(app);

  BoxState box = {0};
  box.p = v2(20, 40);
  box.v = v2(0.01f, 0.03f);
  box.dim = v2(20, 20);
  // TODO: more event handling
  // TODO: smooth resize
  while(running)
  {
    // NOTE: get events
    NSEvent *e = 0;
    NSEventMask mask = NSEventMaskAny;
    //NSRunLoopMode mode = NSString_stringWithUTF8String("kCFRunLoopDefaultMode");
    NSRunLoopMode mode = NSDefaultRunLoopMode;
    //NSRunLoopMode mode = NSModalPanelRunLoopMode;
    //NSRunLoopMode mode = NSEventTrackingRunLoopMode;
    while((e = NSApplication_nextEventMatchingMask(app, mask, 0, mode, true)) != 0)
    {
      NSEventType event_type = NSEvent_type(e);
      //NSEventSubtype event_subtype = NSEvent_subtype(e);
      NSPoint event_pos = NSEvent_locationInWindow(e);
      NSEventModifierFlags event_mods = NSEvent_modifierFlags(e);
      if(event_type)
      {
	printf("default event:\n"
	       "  type: %lu\n"
	       //"  subtype: %hd\n"
	       "  pos: (%.2f, %.2f)\n"
	       "  mods: %lu\n",
	       event_type,
	       //event_subtype,
	       event_pos.x, event_pos.y,
	       event_mods);
      }

      NSApplication_sendEvent(app, e);
    }

    /* mode = NSRunLoopCommonModes; */
    /* while((e = NSApplication_nextEventMatchingMask(app, mask, 0, mode, true)) != 0) */
    /* { */
    /*   NSEventType event_type = NSEvent_type(e); */
    /*   //NSEventSubtype event_subtype = NSEvent_subtype(e); */
    /*   NSPoint event_pos = NSEvent_locationInWindow(e); */
    /*   NSEventModifierFlags event_mods = NSEvent_modifierFlags(e); */
    /*   if(event_type) */
    /*   { */
    /* 	printf("common event:\n" */
    /* 	       "  type: %lu\n" */
    /* 	       //"  subtype: %hd\n" */
    /* 	       "  pos: (%.2f, %.2f)\n" */
    /* 	       "  mods: %lu\n", */
    /* 	       event_type, */
    /* 	       //event_subtype, */
    /* 	       event_pos.x, event_pos.y, */
    /* 	       event_mods); */
    /*   } */

    /*   NSApplication_sendEvent(app, e); */
    /* } */

    NSApplication_updateWindows(app);

    CGRect frame_bounds = NSView_bounds(view);
    V2S32 frame_dim = v2s32(frame_bounds.size.width, frame_bounds.size.height);

    CVPixelBufferLockBaseAddress(backbuffer->buf, 0);
    U32 *backbuffer_pixels = CVPixelBufferGetBaseAddress(backbuffer->buf);
    U32 pixel_stride = CVPixelBufferGetBytesPerRow(backbuffer->buf);
    update_and_draw(&box, backbuffer_pixels, pixel_stride, frame_dim);
    CVPixelBufferUnlockBaseAddress(backbuffer->buf, 0);

    render(view, frame_bounds);
  }

end:
  // TODO: proper cleanup
  return(result);
}
