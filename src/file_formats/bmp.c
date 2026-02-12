proc LoadedBmp
bmp_alloc(Arena *arena, S32 width, S32 height)
{
  U32 *pixels = arena_push_array(arena, U32, width*height);
  LoadedBmp result = {0};
  result.width = width;
  result.height = height;
  result.pixels = pixels;
  return(result);
}

proc LoadedBmp
bmp_load(Arena *arena, String8 file_path)
{
  ArenaTemp scratch = arena_get_scratch(&arena, 1);

  Buffer file_data = os_read_entire_file(scratch.arena, file_path);
  BmpFileHeader *file_header = buf_consume_struct(&file_data, BmpFileHeader);
  Assert(file_header->type == BITMAP_TYPE);
  Assert(file_header->data_offset == FILE_HEADER_SIZE + DATA_HEADER_SIZE);
  // TODO: handle files with extended headers

  BmpDataHeader *data_header = buf_consume_struct(&file_data, BmpDataHeader);
  Assert(data_header->header_size == sizeof(*data_header));
  S32 width = data_header->width;
  S32 height = data_header->height;
  Assert(data_header->bits_per_pixel == 8*sizeof(U32));
  Assert(data_header->compression == 0); // TODO: support RGB bitmasks, compressed files
  U32 image_size__bytes = data_header->image_size;
  U32 image_size__pixels = image_size__bytes / sizeof(U32);
  U32 *file_pixels = buf_consume_array(&file_data, U32, image_size__pixels);

  // TODO: change row & pixel order/format ?
  U32 *pixels = arena_push_array(arena, U32, image_size__pixels);
  CopyArray(pixels, file_pixels, U32, image_size__pixels);

  arena_release_scratch(scratch);

  // NOTE: storing the pixels in the same order and format as they are in the file
  LoadedBmp result = {0};
  result.width = (S16)width;
  result.height = (S16)height;
  result.pixels = pixels;
  return(result);
}

proc void
bmp_write(LoadedBmp image, String8 file_path)
{
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U32 dest_size = FILE_HEADER_SIZE + DATA_HEADER_SIZE + image.width * image.height * sizeof(*image.pixels);
  PushBuffer buf = push_buffer_alloc(scratch.arena, dest_size);

  BmpFileHeader *file_header = buf_push_struct(&buf, BmpFileHeader);
  file_header->type = BITMAP_TYPE;
  file_header->size = dest_size;
  file_header->reserved = 0;
  file_header->data_offset = FILE_HEADER_SIZE + DATA_HEADER_SIZE;

  BmpDataHeader *data_header = buf_push_struct(&buf, BmpDataHeader);
  data_header->header_size = DATA_HEADER_SIZE;
  data_header->width = image.width;
  data_header->height = image.height;
  data_header->plane_count = 1;
  data_header->bits_per_pixel = 8*sizeof(*image.pixels);
  data_header->compression = 0;
  data_header->image_size = image.width * image.height;
  data_header->horz_ppm = 0;
  data_header->vert_ppm = 0;
  data_header->color_count = 0;
  data_header->important_color_count = 0;

  U32 *pixels = buf_push_array(&buf, U32, image.width*image.height);
  CopyArray(pixels, image.pixels, U32, image.width*image.height);

  Buffer file_data = buf_from_push_buffer(buf);
  os_write_entire_file(file_data, file_path);

  arena_release_scratch(scratch);
}
