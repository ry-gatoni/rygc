#include <fcntl.h>
#include <unistd.h>
#include <dlfcn.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/syscall.h>

typedef struct Posix_ThreadInfo Posix_ThreadInfo;
struct Posix_ThreadInfo
{
  Posix_ThreadInfo *next;

  Os_ThreadProc *procedure;
  void *data;

  pthread_t handle;
};

typedef struct PosixState
{
  Arena *arena;

  Posix_ThreadInfo *first_thread_info;
  Posix_ThreadInfo *last_thread_info;
  U64 thread_info_count;

  Posix_ThreadInfo *thread_info_freelist;
} PosixState;

global PosixState *posix_state = 0;

// -----------------------------------------------------------------------------
// initialization

proc void posix_init(Arena *arena);

// -----------------------------------------------------------------------------
// system info

proc U64 posix_page_size(void);

// -----------------------------------------------------------------------------
// memory

proc void* posix_mem_reserve(U64 size);
proc B32 posix_mem_commit(void *mem, U64 size);
proc void posix_mem_decommit(void *mem, U64 size);
proc void posix_mem_release(void *mem, U64 size);

// TODO: posix ring buffer?

// -----------------------------------------------------------------------------
// files

proc Os_Handle posix_file_open(String8 path, Os_FileOpenFlags flags);
proc Os_FileAttributes posix_file_attributes(Os_Handle file);
proc Buffer posix_file_read(Arena *arena, Os_Handle file, U64 size);
proc Buffer posix_file_read_from(Arena *arena, Os_Handle file, U64 pos, U64 size);
proc B32 posix_file_write(Buffer file_contents, Os_Handle file);
proc B32 posix_file_write_to(Buffer file_contents, U64 pos, Os_Handle file);

// -----------------------------------------------------------------------------
// threads

proc Os_Handle posix_thread_launch(Os_ThreadProc *procedure, void *data);
proc U64 posix_thread_id(void);

proc void* posix_thread_entry_point(void *param);

// -----------------------------------------------------------------------------
// libraries

proc Os_Handle posix_lib_open(String8 path);
proc void posix_lib_close(Os_Handle lib);
proc void* posix_lib_get_sym(Os_Handle lib, String8 sym_name);

// TODO: posix this image?

// -----------------------------------------------------------------------------
// helpers

proc Os_Handle posix_os_handle_from_file_handle(int fd);
proc int posix_file_handle_from_os_handle(Os_Handle file);

proc Posix_ThreadInfo* posix_alloc_thread_info(void);

proc Os_Handle posix_os_handle_from_thread_handle(pthread_t handle);
proc pthread_t posix_thread_handle_from_os_handle(Os_Handle handle);

proc Os_Handle posix_os_handle_from_lib_handle(void *handle);
proc void* posix_lib_handle_from_os_handle(Os_Handle handle);
