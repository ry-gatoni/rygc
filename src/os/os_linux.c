#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

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
