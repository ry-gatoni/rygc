proc Buffer
buffer_alloc(Arena *arena, U64 size)
{
  Buffer result = {0};
  result.mem = arena_push_array_z(arena, U8, size);
  result.size = size;

  return(result);
}

proc PushBuffer
push_buffer_alloc(Arena *arena, U64 cap)
{
  PushBuffer result = {0};
  result.mem = arena_push_array_z(arena, U8, cap);
  result.pos = 0;
  result.cap = cap;

  return(result);
}

proc String8
buf_push_str8_copy(PushBuffer *buf, String8 str)
{
  String8 result = {0};
  result.string = buf_push_array(buf, U8, str.count + 1);
  CopySize(result.string, str.string, str.count);
  result.count = str.count;

  return(result);
}

proc U8*
buf_push(PushBuffer *buf, U64 size, U64 alignment)
{
  U8 *result = 0;
  U64 base = AlignPow2(buf->pos, alignment);
  U64 new_pos = base + size;
  if(new_pos < buf->cap) {
    result = buf->mem + base;
    buf->pos = new_pos;
  }
  
  return(result);
}

proc B32
ring_buf_read_size(U8 *dest, RingBuffer *rb, U64 size)
{
  B32 result = size < rb->separation;

  if(result) {
    U8 *src = rb->mem + rb->read_pos;
    U64 size_to_end = Min(size, rb->size - rb->read_pos);
    CopySize(dest, src, size_to_end);
    
    if(size_to_end < size) {
      dest += size_to_end;
      src = rb->mem;
      size -= size_to_end;      
      CopySize(dest, src, size);
    }

    rb->read_pos = (rb->read_pos + size) % rb->size;
    rb->separation -= size;
  }
  
  return(result);
}

proc B32
ring_buf_write_size(RingBuffer *rb, U8 *src, U64 size)
{
  B32 result = rb->separation + size < rb->size;

  if(result) {
    U8 *dest = rb->mem + rb->write_pos;
    U64 size_to_end = Min(size, rb->size - rb->write_pos);
    CopySize(dest, src, size_to_end);
    
    if(size_to_end < size) {
      src += size_to_end;
      dest = rb->mem;
      size -= size_to_end;      
      CopySize(dest, src, size);
    }

    rb->write_pos = (rb->write_pos + size) % rb->size;
    rb->separation += size;
  }

  return(result);
}
