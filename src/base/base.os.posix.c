// TODO: opt into or out of default os implementation calling posix functions,
// to reduce duplicated code between implemenations for posix systems (eg linux
// and mac)

// -----------------------------------------------------------------------------
// initialization

proc void
posix_init(Arena *arena)
{
  posix_state = arena_push_struct(arena, PosixState);
  posix_state->arena = arena;
}

// -----------------------------------------------------------------------------
// system info

proc U64
posix_page_size(void)
{
  U64 result = sysconf(_SC_PAGESIZE);
  return(result);
}

// -----------------------------------------------------------------------------
// memory

proc void*
posix_mem_reserve(U64 size)
{
  void *result = mmap(0, size, PROT_NONE, MAP_PRIVATE | MAP_ANON, -1, 0);
  if(result == MAP_FAILED)
  {
    result = 0;
    // TODO: log "ERROR: mmap failed: <ERROR STRING>"
  }
  return(result);
}

proc B32
posix_mem_commit(void *mem, U64 size)
{
  B32 result = 1;
  int success = mprotect(mem, size, PROT_READ | PROT_WRITE);
  if(success == -1)
  {
    result = 0;
    // TODO: log "ERROR: mprotect failed: <ERROR STRING>"
  }
  return(result);
}

proc void
posix_mem_decommit(void *mem, U64 size)
{
  int success = madvise(mem, size, MADV_DONTNEED);
  if(success == 0)
  {
    success = mprotect(mem, size, PROT_NONE);
    if(success == -1)
    {
      // TODO: log "ERROR: mprotect failed: <ERROR STRING>"
    }
  }
  else
  {
    // TODO: log "ERROR: madvise failed: <ERROR STRING>"
  }
}

proc void
posix_mem_release(void *mem, U64 size)
{
  int success = munmap(mem, size);
  if(success == -1)
  {
    // TODO: log "ERROR: munmap failed: <ERROR STRING>"
  }
}

// -----------------------------------------------------------------------------
// files

proc Os_Handle
posix_file_open(String8 path, Os_FileOpenFlags flags)
{
  ArenaTemp scratch = arena_get_scratch(0, 0);
  String8 path_cstr = arena_push_str8_copy(scratch.arena, path);

  int open_flags = 0;
  mode_t mode = 0;
  if((flags & Os_FileOpenFlag_read) &&
     (flags & Os_FileOpenFlag_write))
  {
    open_flags |= O_RDWR;
  }
  else if(flags & Os_FileOpenFlag_read)
  {
    open_flags |= O_RDONLY;
  }
  else if(flags & Os_FileOpenFlag_write)
  {
    open_flags |= O_WRONLY;
    open_flags |= O_CREAT;

    mode |= S_IRUSR;
    mode |= S_IWUSR;
    mode |= S_IRGRP;
    mode |= S_IROTH;
  }

  int handle = open((char*)path_cstr.string, open_flags, mode);
  if(handle == -1)
  {
    // TODO: log error
  }

  Os_Handle result = posix_os_handle_from_file_handle(handle);
  return(result);
}

proc Os_FileAttributes
posix_file_attributes(Os_Handle file)
{
  Os_FileAttributes result = {0};

  int fd = posix_file_handle_from_os_handle(file);

  struct stat attr = {0};
  int fstat_result = fstat(fd, &attr);
  if(fstat_result != -1)
  {
    result.size = attr.st_size;
  }

  return(result);
}

proc Buffer
posix_file_read(Arena *arena, Os_Handle file, U64 size)
{
  U64 arena_pos_pre = arena_pos(arena);
  U8 *mem = arena_push_array(arena, U8, size + 1);

  int fd = posix_file_handle_from_os_handle(file);

  U64 bytes_remaining = size;
  U8 *dest = mem;
  while(bytes_remaining)
  {
    U32 bytes_to_read = Min(bytes_remaining, POSIX_OS_READ_MAX_BYTES);
    int bytes_read = read(fd, dest, bytes_to_read);
    if(bytes_read == -1)
    {
      size = 0;
      mem = 0;
      arena_pop_to(arena, arena_pos_pre);
      // TODO: log error
      break;
    }

    dest += bytes_read;
    bytes_remaining -= bytes_read;
  }
  if(mem)
  {
    mem[size] = 0; // NOTE: null termination
  }

  Buffer result = {0};
  result.size = size;
  result.mem = mem;
  return(result);
}

proc Buffer
posix_file_read_from(Arena *arena, Os_Handle file, U64 pos, U64 size)
{
  U64 arena_pos_pre = arena_pos(arena);
  U8 *mem = arena_push_array(arena, U8, size + 1);

  int fd = posix_file_handle_from_os_handle(file);

  U64 bytes_remaining = size;
  U8 *dest = mem;
  for(;bytes_remaining;)
  {
    U32 bytes_to_read = Min(bytes_remaining, POSIX_OS_READ_MAX_BYTES);
    int bytes_read = pread(fd, dest, bytes_to_read, pos);
    if(bytes_read == -1)
    {
      size = 0;
      mem = 0;
      arena_pop_to(arena, arena_pos_pre);
      // TODO: log error
      break;
    }

    bytes_remaining -= bytes_read;
    dest += bytes_read;
    pos += bytes_read;
  }
  if(mem) mem[size] = 0; // NOTE: null termination

  Buffer result = {0};
  result.size = size;
  result.mem = mem;
  return(result);
}

proc B32
posix_file_write(Buffer file_contents, Os_Handle file)
{
  B32 result = 1;

  int fd = posix_file_handle_from_os_handle(file);

  U64 bytes_remaining = file_contents.size;
  U8 *src = file_contents.mem;
  while(bytes_remaining)
  {
    U32 bytes_to_write = Min(bytes_remaining, POSIX_OS_WRITE_MAX_BYTES);
    int bytes_written = write(fd, src, bytes_to_write);
    if(bytes_written == -1)
    {
      result = 0;
      // TODO: log error
      break;
    }

    bytes_remaining -= bytes_written;
    src += bytes_written;
  }

  return(result);
}

proc B32
posix_file_write_to(Buffer file_contents, U64 pos, Os_Handle file)
{
  B32 result = 1;

  int fd = posix_file_handle_from_os_handle(file);

  U64 bytes_remaining = file_contents.size;
  U8 *src = file_contents.mem;
  for(;bytes_remaining;)
  {
    U32 bytes_to_write = Min(bytes_remaining, POSIX_OS_WRITE_MAX_BYTES);
    int bytes_written = pwrite(fd, src, bytes_to_write, pos);
    if(bytes_written == -1)
    {
      result = 0;
      // TODO: log error
      break;
    }

    bytes_remaining -= bytes_written;
    src += bytes_written;
    pos += bytes_written;
  }

  return(result);
}

// -----------------------------------------------------------------------------
// threads

proc Os_Handle
posix_thread_launch(Os_ThreadProc *procedure, void *data)
{
  Posix_ThreadInfo *thread_info = posix_alloc_thread_info();
  thread_info->procedure = procedure;
  thread_info->data = data;

  pthread_t handle = 0;
  int creation_result = pthread_create(&handle, 0, posix_thread_entry_point, thread_info);
  if(creation_result == 0) thread_info->handle = handle;

  Os_Handle result = posix_os_handle_from_thread_handle(handle);
  return(result);
}

proc void*
posix_thread_entry_point(void *param)
{
  Posix_ThreadInfo *thread_info = param;
  thread_info->procedure(thread_info->data);
  return(0);
}

proc U64
posix_thread_id(void)
{
  pthread_t thread_id = pthread_self();
  U64 result = (U64)thread_id;
  return(result);
}

// -----------------------------------------------------------------------------
// libraries

proc Os_Handle
posix_lib_open(String8 path)
{
  ArenaTemp scratch = arena_get_scratch(0, 0);

  char *path_cstr = arena_push_array(scratch.arena, char, path.count + 1);
  CopyArray(path_cstr, path.string, char, path.count);
  path_cstr[path.count] = 0;

  void *handle = dlopen(path_cstr, RTLD_NOW);
  if(handle == 0)
  {
    // TODO: log error
  }

  arena_release_scratch(scratch);

  Os_Handle result = posix_os_handle_from_lib_handle(handle);
  return(result);
}

proc void
posix_lib_close(Os_Handle lib)
{
  void *handle = posix_lib_handle_from_os_handle(lib);
  dlclose(handle);
}

proc void*
posix_lib_get_sym(Os_Handle lib, String8 sym_name)
{
  ArenaTemp scratch = arena_get_scratch(0, 0);

  void *handle = posix_lib_handle_from_os_handle(lib);

  char *sym_name_cstr = arena_push_array(scratch.arena, char, sym_name.count + 1);
  CopyArray(sym_name_cstr, sym_name.string, char, sym_name.count);
  sym_name_cstr[sym_name.count] = 0;

  void *result = dlsym(handle, sym_name_cstr);

  arena_release_scratch(scratch);
  return(result);
}

// -----------------------------------------------------------------------------
// helpers

proc Os_Handle
posix_os_handle_from_file_handle(int fd)
{
  Os_Handle result = { .handle = PtrFromInt(fd) };
  return(result);
}

proc int
posix_file_handle_from_os_handle(Os_Handle file)
{
  int result = IntFromPtr(file.handle);
  return(result);
}

proc Posix_ThreadInfo*
posix_alloc_thread_info(void)
{
  Posix_ThreadInfo *result = posix_state->thread_info_freelist;
  if(result)
  {
    SLLStackPop(posix_state->thread_info_freelist);
  }
  else
  {
    result = arena_push_struct(posix_state->arena, Posix_ThreadInfo);
  }
  Assert(result);

  SLLQueuePush(posix_state->first_thread_info, posix_state->last_thread_info, result);
  ++posix_state->thread_info_count;

  return(result);
}

proc Os_Handle
posix_os_handle_from_thread_handle(pthread_t handle)
{
  Os_Handle result = { .handle = (void*)(U64)handle };
  return(result);
}

proc pthread_t
posix_thread_handle_from_os_handle(Os_Handle handle)
{
  pthread_t result = (pthread_t)handle.handle;
  return(result);
}

proc Os_Handle
posix_os_handle_from_lib_handle(void *handle)
{
  Os_Handle result = { .handle = handle };
  return(result);
}

proc void*
posix_lib_handle_from_os_handle(Os_Handle handle)
{
  void *result = handle.handle;
  return(result);
}
