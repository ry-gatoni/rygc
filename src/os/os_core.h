// NOTE: os memory functions
proc void* os_mem_reserve(U64 size);
proc B32 os_mem_commit(void *mem, U64 size);
proc void os_mem_decommit(void *mem, U64 size);
proc void os_mem_release(void *mem, U64 size);

// NOTE: os file functions
proc String8 os_read_entire_file(Arena *arena, String8 path);
proc B32 os_write_entire_file(String8 file, String8 path);
