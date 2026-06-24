typedef struct Os_Handle
{
  void *handle;
} Os_Handle;

/* typedef struct Os_RingBuffer */
/* { */
/*   void *mem; */
/*   U64 size; */
/*   Os_Handle handle; */
/* } Os_RingBuffer; */
typedef struct Os_RingBuffer
{
  void *mem;
  U64 size;
  U64 read;
  U64 write;
} Os_RingBuffer;

typedef struct Os_RingBufferSpan
{
  U8 *start;
  U8 *end;
} Os_RingBufferSpan;

typedef enum Os_FileOpenFlags
{
  Os_FileOpenFlag_read  = (1 << 0),
  Os_FileOpenFlag_write = (1 << 1),
} Os_FileOpenFlags;

typedef struct Os_FileAttributes
{
  U64 size;
} Os_FileAttributes;

#define OsThreadProc(name) void (name)(void *data);
typedef OsThreadProc(Os_ThreadProc);

// -----------------------------------------------------------------------------
// initialization (implemented per-os)

proc B32 os_init(void);

// -----------------------------------------------------------------------------
// system info (implemented per-os)

proc U64 os_page_size(void);

// -----------------------------------------------------------------------------
// memory (implemented per-os)

proc void* os_mem_reserve(U64 size);
proc B32 os_mem_commit(void *mem, U64 size);
proc void os_mem_decommit(void *mem, U64 size);
proc void os_mem_release(void *mem, U64 size);

proc void os_ring_buffer_init(Os_RingBuffer *rb, U64 min_size);
proc void os_ring_buffer_release(Os_RingBuffer *rb);

proc inline U64 os_ring_buffer_used(Os_RingBuffer *rb); // NOTE: number of bytes available for reading
proc inline U64 os_ring_buffer_free(Os_RingBuffer *rb); // NOTE: number of bytes available for writing

proc inline Os_RingBufferSpan os_ring_buffer_read_span(Os_RingBuffer *rb);
proc inline void os_ring_buffer_read_end(Os_RingBuffer *rb, U64 bytes_read);
proc inline Os_RingBufferSpan os_ring_buffer_write_span(Os_RingBuffer *rb);
proc inline void os_ring_buffer_write_end(Os_RingBuffer *rb, U64 bytes_written);

// -----------------------------------------------------------------------------
// files (implemented per-os)

proc Os_Handle os_file_open(String8 path, Os_FileOpenFlags flags);
proc Os_FileAttributes os_file_attributes(Os_Handle file);
proc Buffer os_file_read(Arena *arena, Os_Handle file, U64 size);
proc Buffer os_file_read_from(Arena *arena, Os_Handle file, U64 pos, U64 size);
proc B32 os_file_write(Buffer file_contents, Os_Handle file);
proc B32 os_file_write_to(Buffer file_contents, U64 pos, Os_Handle file);

proc Buffer os_read_entire_file(Arena *arena, String8 path);
proc B32 os_write_entire_file(Buffer file_contents, String8 path);

// -----------------------------------------------------------------------------
// threads (implemented per-os)

proc Os_Handle os_thread_launch(Os_ThreadProc *procedure, void *data);
proc U64 os_thread_id(void);

// -----------------------------------------------------------------------------
// libraries (implemented per-os)

proc void* os_this_image(void);

proc Os_Handle os_lib_open(String8 path);
proc void os_lib_close(Os_Handle lib);
proc void* os_lib_get_sym(Os_Handle lib, String8 sym_name);

// -----------------------------------------------------------------------------
// time

proc U64 os_counter(void);
proc U64 os_counter_freq(void);
