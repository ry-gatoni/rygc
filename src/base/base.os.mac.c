#include "base.os.posix.c"

// -----------------------------------------------------------------------------
// initialization

proc B32
mac_init(void)
{
  B32 result = 1;
  Arena *arena = arena_alloc();
  mac_state = arena_push_struct(arena, Mac_State);
  mac_state->arena = arena;

  // TODO: class ids

  mac_state->cpu_counter_freq = cpu_counter_fixed_freq();
  mac_state->page_size = posix_page_size();

  posix_init(arena);
  return(result);
}

proc B32
os_init(void)
{
  B32 result = mac_init();
  return(result);
}

// -----------------------------------------------------------------------------
// system info

proc U64
os_page_size(void)
{
  U64 result = mac_state->page_size;
  return(result);
}

// -----------------------------------------------------------------------------
// memory

proc void*
os_mem_reserve(U64 size)
{
  void *result = posix_mem_reserve(size);
  return(result);
}

proc B32
os_mem_commit(void *mem, U64 size)
{
  B32 result = posix_mem_commit(mem, size);
  return(result);
}

proc void
os_mem_decommit(void *mem, U64 size)
{
  posix_mem_decommit(mem, size);
}

proc void
os_mem_release(void *mem, U64 size)
{
  posix_mem_release(mem, size);
}

// -----------------------------------------------------------------------------
// files

proc Os_Handle
os_file_open(String8 path, Os_FileOpenFlags flags)
{
  Os_Handle result = posix_file_open(path, flags);
  return(result);
}

proc Os_FileAttributes
os_file_attributes(Os_Handle file)
{
  Os_FileAttributes result = posix_file_attributes(file);
  return(result);
}

proc Buffer
os_file_read(Arena *arena, Os_Handle file, U64 size)
{
  Buffer result = posix_file_read(arena, file, size);
  return(result);
}

proc Buffer
os_file_read_from(Arena *arena, Os_Handle file, U64 pos, U64 size)
{
  Buffer result = posix_file_read_from(arena, file, pos, size);
  return(result);
}

proc B32
os_file_write(Buffer file_contents, Os_Handle file)
{
  B32 result = posix_file_write(file_contents, file);
  return(result);
}

proc B32
os_file_write_to(Buffer file_contents, U64 pos, Os_Handle file)
{
  B32 result = posix_file_write_to(file_contents, pos, file);
  return(result);
}

proc Buffer
os_read_entire_file(Arena *arena, String8 path)
{
  Os_Handle file = os_file_open(path, Os_FileOpenFlag_read);
  Os_FileAttributes attr = os_file_attributes(file);
  Buffer result = os_file_read(arena, file, attr.size);

  return(result);
}

proc B32
os_write_entire_file(Buffer file_contents, String8 path)
{
  Os_Handle file = os_file_open(path, Os_FileOpenFlag_write);
  B32 result = os_file_write(file_contents, file);

  return(result);
}

// -----------------------------------------------------------------------------
// threads

proc Os_Handle
os_thread_launch(Os_ThreadProc *procedure, void *data)
{
  Os_Handle result = posix_thread_launch(procedure, data);
  return(result);
}

proc U64
os_thread_id(void)
{
  U64 result = posix_thread_id();
  return(result);
}

// -----------------------------------------------------------------------------
// libraries

proc Os_Handle
os_lib_open(String8 path)
{
  Os_Handle result = posix_lib_open(path);
  return(result);
}

proc void
os_lib_close(Os_Handle lib)
{
  posix_lib_close(lib);
}

proc void*
os_lib_get_sym(Os_Handle lib, String8 sym_name)
{
  void *result = posix_lib_get_sym(lib, sym_name);
  return(result);
}
