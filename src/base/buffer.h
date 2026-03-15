typedef struct PushBuffer
{
  U64 cap;
  U64 pos;
  U8 *mem;

  U32 alignment;              // NOTE: per-push alignment
  B32 null_terminate_strings; // NOTE: if we null-terminate every pushed string or not
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

// -----------------------------------------------------------------------------
// buffer

// -----------------------------------------------------------------------------
// constructors

proc Buffer buffer_alloc(Arena *arena, U64 size);
proc Buffer buffer_from_range(U8 *start, U8 *opl);
proc Buffer buf_from_str8(String8 str);

// -----------------------------------------------------------------------------
// consume

#define buf_consume_struct(buf, type) (type*)buf_consume_size(buf, sizeof(type))
#define buf_consume_array(buf, type, count) (type*)buf_consume_size(buf, (count)*sizeof(type))
proc U8* buf_consume_size(Buffer *buf, U64 size);

// -----------------------------------------------------------------------------
// peek

proc B32 buf_peek_char(Buffer buf, U8 c);

// -----------------------------------------------------------------------------
// push buffer

#define push_buffer_alloc(arena, cap) push_buffer_alloc_ex(arena, cap, 0, 0)
proc PushBuffer push_buffer_alloc_ex(Arena *arena, U64 cap, U32 alignment, B32 null_terminate_strings);

#define buf_push_struct(buf, type) (type*)buf_push(buf, sizeof(type))
#define buf_push_array(buf, type, count) (type*)buf_push(buf, (count)*sizeof(type))
proc U8* buf_push(PushBuffer *buf, U64 size);
proc String8 buf_push_str8_copy(PushBuffer *buf, String8 str);

proc Buffer buf_from_push_buffer(PushBuffer pb);
proc String8 str8_from_push_buffer(PushBuffer pb);

// -----------------------------------------------------------------------------
// ring buffer

proc B32 ring_buf_read_size(U8 *dest, RingBuffer *rb, U64 size);
proc B32 ring_buf_write_size(RingBuffer *rb, U8 *src, U64 size);
#define ring_buf_read_struct(dest, rb, type) ring_buf_read_size((U8 *)dest, rb, sizeof(type))
#define ring_buf_write_struct(rb, src, type) ring_buf_read_size(rb, (U8 *)src, sizeof(type))
#define ring_buf_read_array(dest, rb, type, count) ring_buf_read_size(dest, rb, (count)*sizeof(type))
#define ring_buf_write_array(rb, src, type, count) ring_buf_read_size(rb, src, (count)*sizeof(type))
