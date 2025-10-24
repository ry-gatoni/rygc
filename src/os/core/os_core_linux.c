proc B32
os_init(void)
{
  B32 result = linux_init();
  return(result);
}

// NOTE: memory
proc void *
os_mem_reserve(U64 size)
{
  void *result = mmap(0, size, PROT_NONE, MAP_PRIVATE | MAP_ANON, -1, 0);
  if(result == MAP_FAILED) {
    result = 0;
    // TODO: log "ERROR: mmap failed: <ERROR STRING>"
  }
  
  return(result);
}

proc B32
os_mem_commit(void *mem, U64 size)
{
  B32 result = 1;
  int success = mprotect(mem, size, PROT_READ | PROT_WRITE);
  if(success == -1) {
      result = 0;
      // TODO: log "ERROR: mprotect failed: <ERROR STRING>"
    }

  return(result);
}

proc void
os_mem_decommit(void *mem, U64 size)
{  
  int success = madvise(mem, size, MADV_DONTNEED);
  if(success == 0) {
    success = mprotect(mem, size, PROT_NONE);
    if(success == -1) {
      // TODO: log "ERROR: mprotect failed: <ERROR STRING>"
    }
  }
  else {
    // TODO: log "ERROR: madvise failed: <ERROR STRING>"
  } 
}

proc void
os_mem_release(void *mem, U64 size)
{
  int success = munmap(mem, size);
  if(success == -1) {
    // TODO: log "ERROR: munmap failed: <ERROR STRING>"
  }
}

// NOTE: files
#define LINUX_MAX_BYTES_TO_READ (0x7FFFF000ULL)
#define LINUX_MAX_BYTES_TO_WRITE LINUX_MAX_BYTES_TO_READ

proc Os_Handle
os_file_open(String8 path, Os_FileOpenFlags flags)
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
  if(handle == -1) {
    // TODO: log error
  }
  
  Os_Handle result = {0};
  result.handle = PtrFromInt(handle);
  return(result);
}

proc Os_FileAttributes
os_file_attributes(Os_Handle file)
{
  Os_FileAttributes result = {0};

  int handle = IntFromPtr(file.handle);
  struct stat attr = {0};
  int fstat_result = fstat(handle, &attr);
  if(fstat_result != -1) {
    result.size = attr.st_size;
  }
  return(result);
}

proc Buffer
os_file_read(Arena *arena, Os_Handle file, U64 size)
{
  U64 arena_pos_pre = arena_pos(arena);
  U8 *mem = arena_push_array(arena, U8, size + 1);

  int handle = IntFromPtr(file.handle);
  U64 bytes_remaining = size;
  U8 *dest = mem;
  while(bytes_remaining) {
    U32 bytes_to_read = Min(bytes_remaining, LINUX_MAX_BYTES_TO_READ);
    int bytes_read = read(handle, dest, bytes_to_read);
    if(bytes_read == -1) {
      size = 0;
      mem = 0;
      arena_pop_to(arena, arena_pos_pre);
      // TODO: log error
      break;
    }

    dest += bytes_read;
    bytes_remaining -= bytes_read;
  }
  if(mem) {
    mem[size] = 0; // NOTE: null termination
  }
  
  Buffer result = {0};
  result.size = size;
  result.mem = mem;
  return(result);
}

proc B32
os_file_write(Buffer file_contents, Os_Handle file)
{
  B32 result = 1;
  int handle = IntFromPtr(file.handle);
  U64 bytes_remaining = file_contents.size;
  U8 *src = file_contents.mem;
  while(bytes_remaining) {
    U32 bytes_to_write = Min(bytes_remaining, LINUX_MAX_BYTES_TO_WRITE);
    int bytes_written = write(handle, src, bytes_to_write);
    if(bytes_written == -1) {
      result = 0;
      // TODO: log error
      break;
    }

    bytes_remaining -= bytes_written;
    src += bytes_written;
  }
  return(result);
}

proc String8
os_read_entire_file(Arena *arena, String8 path)
{
  ArenaTemp scratch = arena_get_scratch(&arena, 1);

  String8 result = {0};
  String8 path_cstr = arena_push_str8_copy(scratch.arena, path);
  int handle = open((char *)path_cstr.string, O_RDONLY);
  if(handle != - 1) {
    struct stat file_status = {0};
    int fstat_result = fstat(handle, &file_status);
    if(fstat_result != -1) {
      U64 arena_pre_pos = arena_pos(arena);
      result.count = file_status.st_size;
      result.string = arena_push_array(arena, U8, result.count);
      
      U64 bytes_remaining = result.count;
      U8 *dest = result.string;
      while(bytes_remaining) {
	U32 bytes_to_read = Min(bytes_remaining, LINUX_MAX_BYTES_TO_READ);
	int bytes_read = read(handle, dest, bytes_to_read);
	if(bytes_read == -1) {
	  // TODO: log "ERROR: read failed: <ERROR STRING>"
	  result.string = 0;
	  result.count = 0;
	  arena_pop_to(arena, arena_pre_pos);
	  break;
	}
	
	dest += bytes_read;
	bytes_remaining -= bytes_read;
      }
    }
    else {
      // TODO: log "ERROR: fstat failed: <ERROR STRING>"
    }
      
    close(handle);
  }
  else {
    // TODO: log "ERROR: open failed: <ERROR STRING>"
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
os_write_entire_file(String8 file, String8 path)
{
  B32 result = 1;

  ArenaTemp scratch = arena_get_scratch(0, 0);

  String8 path_cstr = arena_push_str8_copy(scratch.arena, path);
  int handle = open((char *)path_cstr.string, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if(handle != -1) {
    U8 *src = file.string;
    U64 bytes_remaining = file.count;
    while(bytes_remaining) {
      U32 bytes_to_write = Min(bytes_remaining, LINUX_MAX_BYTES_TO_WRITE);
      int bytes_written = write(handle, src, bytes_to_write);
      if(bytes_written == -1) {
	result = 0;
	break;
	// TODO: log "ERROR: write failed: <ERROR STRING>"
      }

      bytes_remaining -= bytes_written;
      src += bytes_written;
    }
  }
  else {
    result = 0;
    // TODO: log "ERROR: open failed: <ERROR STRING>"
  }    

  arena_release_scratch(scratch);
  return(result);
}

proc Os_Handle
os_thread_launch(Os_ThreadProc *procedure, void *data)
{
  Linux_ThreadInfo *thread_info = linux_alloc_thread_info();
  thread_info->procedure = procedure;
  thread_info->data = data;

  pthread_t handle = 0;
  int creation_result = pthread_create(&handle, 0, linux_thread_entry_point, thread_info);
  if(creation_result == 0) {
    thread_info->handle = handle;
  }

  Os_Handle result = {0};
  result.handle = PtrFromInt(handle);
  return(result);
}

// NOTE: intrinsics
proc U64
cpu_get_cycle_count_fixed(void)
{
  U64 result = 0;
#if CPU_X86 || CPU_X64
  struct timeval t;
  gettimeofday(&t, 0);
  result = Million(1)*t.tv_sec + t.tv_usec;
#else // TODO: arm
#  error not implemented for this architecture
#endif
  return(result);
}

proc U64
cpu_get_cycle_counter_freq(void)
{
  U64 result = 0;
#if CPU_X86 || CPU_X64
  result = Million(1);
#else // TODO: arm
#  error not implemented for this architecture
#endif
  return(result);
}

proc U64
cpu_get_cycle_count(void)
{
  U64 result = 0;
#if CPU_X86 || CPU_X64
  result = __rdtsc();    
#else // TODO: arm
#  error not implemented for this architecture
#endif
  return(result);
}

// NOTE: linux functions
proc B32
linux_init(void)
{
  B32 result = 0;
  Arena *arena = arena_alloc();
  linux_state = arena_push_struct(arena, Linux_State);
  if(linux_state) {
    linux_state->arena = arena;
    result = 1;
  }
  return(result);
}

proc Linux_ThreadInfo*
linux_alloc_thread_info(void)
{
  Linux_ThreadInfo *result = 0;
  if(linux_state) {
    
    result = linux_state->thread_info_freelist;
    if(result) {
      SLLStackPop(linux_state->thread_info_freelist);
    }else {
      result = arena_push_struct(linux_state->arena, Linux_ThreadInfo);
    }
    
    Assert(result);
    SLLQueuePush(linux_state->first_thread_info, linux_state->last_thread_info, result);
    ++linux_state->thread_info_count;
  }
  return(result);
}

proc void*
linux_thread_entry_point(void *param)
{
  Linux_ThreadInfo *thread_info = (Linux_ThreadInfo*)param;
  thread_info->procedure(thread_info->data);
  return(0);
}
