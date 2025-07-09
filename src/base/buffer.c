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
