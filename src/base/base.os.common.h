typedef struct Os_Handle
{
  void *handle;
} Os_Handle;

typedef struct Os_RingBuffer
{
  void *mem;
  U64 size;
  Os_Handle handle;
} Os_RingBuffer;

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
// memory (implemented per-os)

proc void* os_mem_reserve(U64 size);
proc B32 os_mem_commit(void *mem, U64 size);
proc void os_mem_decommit(void *mem, U64 size);
proc void os_mem_release(void *mem, U64 size);

proc Os_RingBuffer os_ring_buffer_alloc(U64 min_size);
proc void os_ring_buffer_free(Os_RingBuffer *rb);

// -----------------------------------------------------------------------------
// files (implemented per-os)

proc Os_Handle os_file_open(String8 path, Os_FileOpenFlags flags);
proc Os_FileAttributes os_file_attributes(Os_Handle file);
proc Buffer os_file_read(Arena *arena, Os_Handle file, U64 size);
proc Buffer os_file_read_from(Arena *arena, Os_Handle file, U64 pos, U64 size);
proc B32 os_file_write(Buffer file_contents, Os_Handle file);
proc B32 os_file_write_to(Buffer file_contents, U64 pos, Os_Handle file);

proc String8 os_read_entire_file(Arena *arena, String8 path);
proc B32 os_write_entire_file(String8 file, String8 path);

// -----------------------------------------------------------------------------
// threads (implemented per-os)

proc Os_Handle os_thread_launch(Os_ThreadProc *procedure, void *data);

// -----------------------------------------------------------------------------
// libraries (implemented per-os)

proc void* os_this_image(void);

// -----------------------------------------------------------------------------
// time

proc U64 os_counter(void);
proc U64 os_counter_freq(void);
