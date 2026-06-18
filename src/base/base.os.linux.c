// -----------------------------------------------------------------------------
// initialization

proc B32
linux_init(void)
{
  B32 result = 0;
  Arena *arena = arena_alloc();
  linux_state = arena_push_struct(arena, Linux_State);
  if(linux_state)
  {
    linux_state->arena = arena;
    linux_state->page_size = posix_pagesize();

    posix_init(arena);

    result = 1;
  }
  return(result);
}

proc B32
os_init(void)
{
  B32 result = linux_init();
  return(result);
}

// -----------------------------------------------------------------------------
// system info

proc U64
os_page_size(void)
{
  U64 result = linux_state->page_size;
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

proc Os_RingBuffer
os_ring_buffer_alloc(U64 min_size)
{
  int fd = -1;
  void *map_base = MAP_FAILED;
  void *map_lo = MAP_FAILED;
  void *map_hi = MAP_FAILED;
  Os_RingBuffer result = {0};

  U64 size = AlignPow2(min_size, linux_state->page_size);
  fd = syscall(__NR_memfd_create, "RYGC-MAGIC-RING-BUFFER", FD_CLOEXEC);
  if(fd == -1) { goto os_ring_buffer_alloc_failure; }
  if(ftruncate(fd, size) == -1) { goto os_ring_buffer_alloc_failure; }

  map_base = mmap(0, 2*size, PROT_NONE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
  if(map_base == MAP_FAILED) { goto os_ring_buffer_alloc_failure; }

  map_lo = mmap((U8*)map_base + 0*size, size, PROT_READ|PROT_WRITE, MAP_FIXED|MAP_SHARED, fd, 0);
  if(map_lo == MAP_FAILED) { goto os_ring_buffer_alloc_failure; }
  map_hi = mmap((U8*)map_base + 1*size, size, PROT_READ|PROT_WRITE, MAP_FIXED|MAP_SHARED, fd, 0);
  if(map_hi == MAP_FAILED) { goto os_ring_buffer_alloc_failure; }

  close(fd);

  result.mem = map_base;
  result.size = size;
  return(result);

os_ring_buffer_alloc_failure:
  if(fd != -1) { close(fd); }
  if(map_base != MAP_FAILED) munmap(map_base, 2*size);
  return(result);
}

proc void
os_ring_buffer_free(Os_RingBuffer *rb)
{
  munmap(rb->mem, 2*rb->size);
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

proc void*
os_this_image(void)
{
  void *result = 0;
  Dl_info info = {0};
  extern int main(int, char **);
  int success = dladdr((void*)main, &info);
  if(success != 0)
  {
    result = info.dli_fbase;
  }
  else
  {
    // TODO: log error
  }
  return(result);
}

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

// -----------------------------------------------------------------------------
// time

proc U64
os_counter(void)
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
os_counter_freq(void)
{
  U64 result = 0;
#if CPU_X86 || CPU_X64
  result = Million(1);
#else // TODO: arm
#  error not implemented for this architecture
#endif
  return(result);
}

// -----------------------------------------------------------------------------
// intrinsics

proc U64
cpu_counter(void)
{
  U64 result = 0;
#if CPU_X86 || CPU_X64
  result = __rdtsc();
#elif CPU_ARM || CPU_ARM64
  __asm__ __volatile__("mrs %0, cntvct_el0" : "=r"(result));
#else
#  error not implemented for this architecture
#endif
  return(result);
}

proc U64
cpu_counter_freq(void)
{
  U64 result = 0;
#if CPU_X86 || CPU_X64
  if(linux_state)
  {
    if(linux_state->cpu_counter_freq)
    {
      result = linux_state->cpu_counter_freq;
    }
  }

  if(result == 0)
  {
    // TODO: it sucks having to estimate the frequency on x86. is there a better way?
    U64 wait_ms = 100;
    U64 os_freq = os_counter_freq();
    U64 os_wait = os_freq * wait_ms / 1000;

    U64 cpu_start = cpu_counter();
    U64 os_start = os_counter();
    U64 os_end = 0;
    U64 os_elapsed = 0;
    while(os_elapsed < os_wait)
    {
      os_end = os_counter();
      os_elapsed = os_end - os_start;
    }
    U64 cpu_end = cpu_counter();
    U64 cpu_elapsed = cpu_end - cpu_start;

    result = os_freq * cpu_elapsed / os_elapsed;
    if(linux_state) linux_state->cpu_counter_freq = result;
  }
#elif CPU_ARM || CPU_ARM64
  __asm__ __volatile__("mrs %0, cntfrq_el0" : "=r"(result));
#else
#  error not implemented for this architecture
#endif
  return(result);
}
