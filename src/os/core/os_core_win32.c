// NOTE: windows helpers/state management
proc B32
win32_init(void)
{
  Arena *arena = arena_alloc();
  w32_state = arena_push_struct(arena, Win32_State);
  w32_state->arena = arena;
  B32 result = w32_state != 0;
  return(result);
}

proc Win32_ThreadInfo*
win32_alloc_thread_info(void)
{
  Assert(w32_state);
  Win32_ThreadInfo *result = 0;
  if(w32_state->thread_info_freelist) {
    result = w32_state->thread_info_freelist;
    SLLStackPop(w32_state->thread_info_freelist);
  }else {
    result = arena_push_struct(w32_state->arena, Win32_ThreadInfo);
  }
  SLLQueuePush(w32_state->first_thread_info, w32_state->last_thread_info, result);
  ++w32_state->thread_info_count;

  return(result);
}

proc DWORD WINAPI
w32_thread_entry_point(void *param)
{
  Assert(param);
  Win32_ThreadInfo *thread_info = param;
  thread_info->procedure(thread_info->data);
  return(0);
}

// NOTE: os functions
proc B32
os_init(void)
{
  B32 result = win32_init();
  return(result);
}

// NOTE: os memory functions
proc void*
os_mem_reserve(U64 size)
{
  void *result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
  return(result);
}

proc B32
os_mem_commit(void *mem, U64 size)
{
  void *mem_result = VirtualAlloc(mem, size, MEM_COMMIT, PAGE_READWRITE);
  B32 result = mem_result != 0;
  return(result);
}

proc void
os_mem_decommit(void *mem, U64 size)
{
  VirtualFree(mem, size, MEM_DECOMMIT);
}

proc void
os_mem_release(void *mem, U64 size)
{
  VirtualFree(mem, size, MEM_RELEASE);
}

// NOTE: os file functions
proc Os_Handle
os_file_open(String8 path, Os_FileOpenFlags flags)
{
  ArenaTemp scratch = arena_get_scratch(0, 0);
  String16 pathw = str16_from_str8(scratch.arena, path);
  DWORD attr = GetFileAttributesW(pathw.string);
  Unused(attr);

  DWORD access_flags = 0;
  DWORD share_flags = 0;
  DWORD creation_disposition = OPEN_EXISTING;
  if(flags & Os_FileOpenFlag_read) {
    access_flags |= GENERIC_READ;
    share_flags |= FILE_SHARE_READ;
  }
  if(flags & Os_FileOpenFlag_write) {
    access_flags |= GENERIC_WRITE;
    share_flags |= FILE_SHARE_WRITE;
    creation_disposition = CREATE_ALWAYS;
  }

  HANDLE handle = CreateFileW((LPCWSTR)pathw.string, access_flags, share_flags, 0, creation_disposition, FILE_ATTRIBUTE_NORMAL, 0);
  /* HANDLE handle = CreateFileW((LPCWSTR)pathw.string, access_flags, share_flags, 0, creation_disposition, 0, 0); */
  if(handle == INVALID_HANDLE_VALUE) {
    DWORD err = GetLastError();
    Win32LogError(err);
  }
  arena_release_scratch(scratch);
  
  Os_Handle result = {0};
  result.handle = handle;
  return(result);
}

proc Os_FileAttributes
os_file_attributes(Os_Handle file)
{
  HANDLE handle = file.handle;
  LARGE_INTEGER file_size;
  GetFileSizeEx(handle, &file_size);

  Os_FileAttributes result = {0};
  result.size = file_size.QuadPart;
  return(result);
}

proc Buffer
os_file_read(Arena *arena, Os_Handle file, U64 size)
{  
  U8 *contents = arena_push_array(arena, U8, size + 1);
  contents[size] = 0;

  HANDLE handle = file.handle;
#if 0
  {
    LARGE_INTEGER move = {0};
    LARGE_INTEGER new_pointer = {0};
    DWORD set_file_pointer_result = SetFilePointerEx(handle, move, &new_pointer, FILE_CURRENT);
    if(set_file_pointer_result == INVALID_SET_FILE_POINTER) {
      DWORD err = GetLastError();
      Win32LogError(err);
    }
  }
#endif
  
  U64 bytes_to_read = size;
  U8 *contents_at = contents;
  while(bytes_to_read) {
    DWORD bytes_to_read_32 = (DWORD)bytes_to_read;
    DWORD bytes_read = 0;
    B32 read_result = ReadFile(handle, contents_at, bytes_to_read_32, &bytes_read, 0);
    if(!read_result) {
      arena_pop(arena, size + 1);
      contents = 0;
      size = 0;
      DWORD err = GetLastError();
      Win32LogError(err);
      break;
    }
    bytes_to_read -= bytes_read;
    contents_at += bytes_read;
  }

  Buffer result = {0};
  result.size = size;
  result.mem = contents;
  return(result);
}

proc B32
os_file_write(Buffer file_contents, Os_Handle file)
{
  B32 result = 1;
  HANDLE handle = file.handle;
  U64 bytes_to_write = file_contents.size;
  U8 *contents = file_contents.mem;
  DWORD bytes_written = 0;
  while(bytes_to_write) {
    DWORD bytes_to_write_32 = (DWORD)bytes_to_write;
    B32 write_result = WriteFile(handle, contents, bytes_to_write_32, &bytes_written, 0);
    if(!write_result) {
      result = 0;
      break;
    }
    contents += bytes_written;
    bytes_to_write -= bytes_written;
  }
  
  return(result);
}

proc String8
os_read_entire_file(Arena *arena, String8 path)
{  
  Os_Handle file = os_file_open(path, Os_FileOpenFlag_read);
  Os_FileAttributes attr = os_file_attributes(file);
  Buffer buf = os_file_read(arena, file, attr.size);

  String8 result = {0};
  result.string = buf.mem;
  result.count = buf.size;
  return(result);
}

proc B32
os_write_entire_file(String8 file, String8 path)
{
  Os_Handle file_handle = os_file_open(path, Os_FileOpenFlag_write);
  Buffer file_contents = {0};
  file_contents.mem = file.string;
  file_contents.size = file.count;
  B32 result = os_file_write(file_contents, file_handle);
  return(result);
}

// NOTE: os thread functions
proc Os_Handle
os_thread_launch(Os_ThreadProc *procedure, void *data)
{
  Win32_ThreadInfo *thread_info = win32_alloc_thread_info();
  thread_info->procedure = procedure;
  thread_info->data = data;
  thread_info->handle = CreateThread(0, 0, w32_thread_entry_point, thread_info, 0, &thread_info->id); 
  
  Os_Handle result = {0};
  result.handle = thread_info;
  return(result);
}
