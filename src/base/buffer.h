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
proc PushBuffer push_buffer_alloc(Arena *arena, U64 cap);

proc String8 buf_push_str8_copy(PushBuffer *buf, String8 str);
proc U8* buf_push(PushBuffer *buf, U64 size, U64 alignment);
#define buf_push_struct(buf, type) (type*)buf_push(buf, sizeof(type), Min(sizeof(void*), sizeof(type)))
#define buf_push_struct_a(buf, type, alignment) (type*)buf_push(buf, sizeof(type), alignment)
#define buf_push_array(buf, type, count) (type*)buf_push(buf, (count)*sizeof(type), Min(sizeof(void*), sizeof(type)))
#define buf_push_array_a(buf, type, count, alignment) (type*)buf_push(buf, (count)*sizeof(type), alignment)

proc B32 ring_buf_read_size(U8 *dest, RingBuffer *rb, U64 size);
proc B32 ring_buf_write_size(RingBuffer *rb, U8 *src, U64 size);
#define ring_buf_read_struct(dest, rb, type) ring_buf_read_size((U8 *)dest, rb, sizeof(type))
#define ring_buf_write_struct(rb, src, type) ring_buf_read_size(rb, (U8 *)src, sizeof(type))
#define ring_buf_read_array(dest, rb, type, count) ring_buf_read_size(dest, rb, (count)*sizeof(type))
#define ring_buf_write_array(rb, src, type, count) ring_buf_read_size(rb, src, (count)*sizeof(type))
