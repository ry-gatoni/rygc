// -----------------------------------------------------------------------------
// memory/ring buffer

proc inline U64
os_ring_buffer_used(Os_RingBuffer *rb)
{
  U64 result = rb->write - rb->read;
  return result;
}

proc inline U64
os_ring_buffer_free(Os_RingBuffer *rb)
{
  U64 result = rb->size - os_ring_buffer_used(rb);
  return result;
}

proc inline Os_RingBufferSpan
os_ring_buffer_read_span(Os_RingBuffer *rb)
{
  U64 offset = rb->read & (rb->size - 1);
  U8 *start = (U8*)rb->mem + offset;
  U8 *end = start + os_ring_buffer_used(rb);

  Os_RingBufferSpan result = {0};
  result.start = start;
  result.end = end;
  return result;
}

proc inline void
os_ring_buffer_read_end(Os_RingBuffer *rb, U64 bytes_read)
{
  Assert(bytes_read <= os_ring_buffer_used(rb));
  rb->read += bytes_read;
}

proc inline Os_RingBufferSpan
os_ring_buffer_write_span(Os_RingBuffer *rb)
{
  U64 offset = rb->write & (rb->size - 1);
  U8 *start = (U8*)rb->mem + offset;
  U8 *end = start + os_ring_buffer_free(rb);

  Os_RingBufferSpan result = {0};
  result.start = start;
  result.end = end;
  return result;
}

proc inline void
os_ring_buffer_write_end(Os_RingBuffer *rb, U64 bytes_written)
{
  Assert(bytes_written <= os_ring_buffer_free(rb));
  rb->write += bytes_written;
}
