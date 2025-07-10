typedef struct Buffer
{
  U64 size;
  U8 *mem;
} Buffer;

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

proc B32 ring_buf_read_size(U8 *dest, RingBuffer *rb, U64 size);
proc B32 ring_buf_write_size(RingBuffer *rb, U8 *src, U64 size);
#define ring_buf_read_struct(dest, rb, type) ring_buf_read_size((U8 *)dest, rb, sizeof(type))
#define ring_buf_write_struct(rb, src, type) ring_buf_read_size(rb, (U8 *)src, sizeof(type))
#define ring_buf_read_array(dest, rb, type, count) ring_buf_read_size(dest, rb, (count)*sizeof(type))
#define ring_buf_write_array(rb, src, type, count) ring_buf_read_size(rb, src, (count)*sizeof(type))
