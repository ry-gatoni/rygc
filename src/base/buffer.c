// -----------------------------------------------------------------------------
// buffer

proc Buffer
buffer_alloc(Arena *arena, U64 size)
{
  Buffer result = {0};
  result.mem = arena_push_array_z(arena, U8, size);
  result.size = size;

  return(result);
}

proc Buffer
buffer_from_range(U8 *start, U8 *opl)
{
  Buffer result = {0};
  result.mem = start;
  result.size = IntFromPtr(opl - start);
  return(result);
}

proc Buffer
buf_from_str8(String8 str)
{
  Buffer result = {0};
  result.mem = str.string;
  result.size = str.count;
  return(result);
}

// NOTE: I'm making the convention be that the buffer returns null if you try to
// read past the end. It might make more sense to only return null if the buffer
// is *empty*, and hard crash if the buffer has stuff but you try to read
// more. I'm worried about a situation where you're looping until the buffer is
// empty, but you have a bug and keep trying to read more from the buffer than
// it has, thus entering an infinite loop. But maybe there's a situation where
// you actually want it to return null if you try to read past the end and
// there's still stuff in there. I haven't ever wanted that, but who knows?
// - Ry (2026-03-14)
proc U8*
buf_consume_size(Buffer *buf, U64 size)
{
  U8 *result = 0;
  //Assert(size <= buf->size);
  if(size <= buf->size)
  {
    result = buf->mem;
    buf->mem += size;
    buf->size -= size;
  }
  return(result);
}

proc B32
buf_peek_char(Buffer buf, U8 c)
{
  B32 result = 0;
  if(buf.size)
  {
    result = *buf.mem == c;
  }
  return(result);
}

// -----------------------------------------------------------------------------
// push buffer

proc PushBuffer
push_buffer_alloc_ex(Arena *arena, U64 cap, U32 alignment, B32 null_terminate_strings)
{
  PushBuffer result = {0};
  result.mem = arena_push_array_z(arena, U8, cap);
  result.pos = 0;
  result.cap = cap;
  result.alignment = alignment;
  result.null_terminate_strings = null_terminate_strings;
  return(result);
}

proc U8*
buf_push(PushBuffer *buf, U64 size)
{
  U8 *result = 0;
  U64 base = buf->alignment ? AlignPow2(buf->pos, buf->alignment) : buf->pos;
  U64 new_pos = base + size;
  if(new_pos <= buf->cap) {
    result = buf->mem + base;
    buf->pos = new_pos;
  }

  return(result);
}

proc String8
buf_push_str8_copy(PushBuffer *buf, String8 str)
{
  String8 result = {0};
  result.string = buf_push_array(buf, U8, buf->null_terminate_strings ? str.count + 1 : str.count);
  CopySize(result.string, str.string, str.count);
  result.count = str.count;

  return(result);
}

proc Buffer
buf_from_push_buffer(PushBuffer pb)
{
  Buffer result = {0};
  result.size = pb.pos;
  result.mem = pb.mem;
  return(result);
}

proc String8
str8_from_push_buffer(PushBuffer pb)
{
  Assert(pb.pos < pb.cap);
  pb.mem[pb.pos] = 0; // NOTE: null termination

  String8 result = {0};
  result.count = pb.pos;
  result.string = pb.mem;
  return(result);
}

// -----------------------------------------------------------------------------
// ring buffer

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
