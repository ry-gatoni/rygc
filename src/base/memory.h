// NOTE: types
typedef struct Arena Arena;
struct Arena
{
  Arena *current;
  Arena *prev;

  // NOTE: when this arena needs to allocate a new block, these are the sizes it will use
  U64 reserve_size;
  U64 commit_size;
  B32 growable;

  U64 base;
  U64 capacity;
  
  U64 pos;
  U64 commit_pos;
};

typedef struct ArenaTemp
{
  Arena *arena;
  U64 pos;
} ArenaTemp;

#define ARENA_HEADER_SIZE 128
StaticAssert(sizeof(Arena) <= ARENA_HEADER_SIZE, arena_header_size_check);

#define ARENA_DEFAULT_RESERVE_SIZE MB(64)
#define ARENA_DEFAULT_COMMIT_SIZE KB(64)

// NOTE: arena alloc/release
proc Arena* arena_alloc_ex(U64 reserve_size, U64 commit_size, void *backing_buffer, B32 growable);
proc Arena* arena_alloc(void);
proc void arena_release(Arena *arena);

// NOTE: arena push/pop
proc void* arena_push(Arena *arena, U64 size, U64 alginment);
proc void* arena_push_z(Arena *arena, U64 size, U64 alginment);
proc U64 arena_pos(Arena *arena);
proc void arena_pop_to(Arena *arena, U64 pos);
proc void arena_pop(Arena *arena, U64 ammount);
proc void arena_clear(Arena *arena);
#define arena_push_struct(arena, type) (type *)arena_push(arena, sizeof(type), Min(sizeof(type), sizeof(void*)))
#define arena_push_struct_z(arena, type) (type *)arena_push_z(arena, sizeof(type), Min(sizeof(type), sizeof(void*)))
#define arena_push_array(arena, type, count) (type *)arena_push(arena, (count)*sizeof(type), sizeof(void*))
#define arena_push_array_z(arena, type, count) (type *)arena_push_z(arena, (count)*sizeof(type), sizeof(void*))
#define arena_push_array_align(arena, type, count, align) (type *)arena_push(arena, (count)*sizeof(type), align)
#define arena_push_array_align_z(arena, type, count, align) (type *)arena_push_z(arena, (count)*sizeof(type), align)

// NOTE: arena temporary memory
proc ArenaTemp arena_begin_temp(Arena *arena);
proc void arena_end_temp(ArenaTemp temp);

proc ArenaTemp arena_get_scratch(Arena **conflics, U64 count);
#define arena_release_scratch(scratch) arena_end_temp(scratch)
