typedef struct Buffer
{
  U64 size;
  U8 *mem;
} Buffer;

typedef struct PushBuffer
{
  U64 cap;
  U64 pos;
  U8 *mem;
} PushBuffer;

typedef struct BufferU32
{
  U64 count;
  U32 *mem;
} BufferU32;

typedef struct BufferR32
{
  U64 count;
  R32 *mem;
} BufferR32;

typedef struct RingBuffer
{
  U64 size;
  U8 *mem;
  
  U64 read_pos;
  U64 write_pos;
  U64 separation;
} RingBuffer;

proc Buffer buffer_alloc(Arena *arena, U64 size);

proc inline U8*
buf_consume_size_(Buffer *buf, U64 size)
{
  U8 *result = 0;
  if(size <= buf->size) {
    result = buf->mem;
    buf->mem += size;
    buf->size -= size;
  }
  return(result);
}
#define buf_consume_struct(buf, type) buf_consume_size_(buf, sizeof(type))
#define buf_consume_array(buf, type, count) buf_consume_size_(buf, (count)*sizeof(type))

proc PushBuffer push_buffer_alloc(Arena *arena, U64 cap);

proc String8 buf_push_str8_copy(PushBuffer *buf, String8 str);
proc U8* buf_push(PushBuffer *buf, U64 size, U64 align);
#define buf_push_struct(buf, type) (type*)buf_push(buf, sizeof(type), 0)
#define buf_push_struct_a(buf, type, align) (type*)buf_push(buf, sizeof(type), align)
#define buf_push_array(buf, type, count) (type*)buf_push(buf, (count)*sizeof(type), 0)
#define buf_push_array_a(buf, type, count, align) (type*)buf_push(buf, (count)*sizeof(type), align)

proc B32 ring_buf_read_size(U8 *dest, RingBuffer *rb, U64 size);
proc B32 ring_buf_write_size(RingBuffer *rb, U8 *src, U64 size);
#define ring_buf_read_struct(dest, rb, type) ring_buf_read_size((U8 *)dest, rb, sizeof(type))
#define ring_buf_write_struct(rb, src, type) ring_buf_read_size(rb, (U8 *)src, sizeof(type))
#define ring_buf_read_array(dest, rb, type, count) ring_buf_read_size(dest, rb, (count)*sizeof(type))
#define ring_buf_write_array(rb, src, type, count) ring_buf_read_size(rb, src, (count)*sizeof(type))
