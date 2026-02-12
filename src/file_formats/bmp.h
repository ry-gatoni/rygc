#define BITMAP_TYPE (0x4D42)

#pragma pack(push, 1)

typedef struct BmpFileHeader
{
  U16 type;
  U32 size;
  U32 reserved; // NOTE: must be 0
  U32 data_offset;
} BmpFileHeader;

typedef struct BmpDataHeader
{
  U32 header_size;
  S32 width;
  S32 height;
  U16 plane_count; // NOTE: must be 1
  U16 bits_per_pixel;
  U32 compression;
  U32 image_size;
  S32 horz_ppm;
  S32 vert_ppm;
  U32 color_count;
  U32 important_color_count;
} BmpDataHeader;

#pragma pack(pop)

#define FILE_HEADER_SIZE (sizeof(BmpFileHeader))
#define DATA_HEADER_SIZE (sizeof(BmpDataHeader))

typedef struct LoadedBmp
{
  S16 width;
  S16 height;
  U32 *pixels;
} LoadedBmp;

// -----------------------------------------------------------------------------
// functions

proc LoadedBmp bmp_alloc(Arena *arena, S32 width, S32 height);

proc LoadedBmp bmp_load(Arena *arena, String8 file_path);

proc void bmp_write(LoadedBmp pixels, String8 file_path);
