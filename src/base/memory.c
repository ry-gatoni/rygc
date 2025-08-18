proc Arena*
arena_alloc_ex(U64 reserve_size, U64 commit_size, void *backing_buffer, B32 growable)
{
  Arena *result = 0;
  // TODO: replace alignment with actual os page size
  reserve_size = AlignPow2(reserve_size, KB(4));
  commit_size = AlignPow2(commit_size, KB(4));

  void *base = backing_buffer;
  if(!base) {
    base = os_mem_reserve(reserve_size);
    if(!os_mem_commit(base, commit_size)) {
      base = 0;
    }
  }
  if(base) {
    AsanPoison(base, reserve_size);
    AsanUnpoison(base, ARENA_HEADER_SIZE);
    result = (Arena *)base;
    result->current = result;
    result->prev = 0;
    result->reserve_size = reserve_size;
    result->commit_size = commit_size;
    result->growable = growable;
    result->base = 0;
    result->capacity = reserve_size;
    result->pos = ARENA_HEADER_SIZE;
    result->commit_pos = commit_size;
  }  

  
  return(result);
}

proc Arena*
arena_alloc(void)
{
  Arena *result = arena_alloc_ex(ARENA_DEFAULT_RESERVE_SIZE, ARENA_DEFAULT_COMMIT_SIZE, 0, 1);
  return(result);
}

proc void
arena_release(Arena *arena)
{
  for(Arena *current = arena->current; current; current = current->prev) {
    // TODO: address sanitizer memory unpoisoning
    os_mem_release(current, current->capacity);
  }
}

proc void*
arena_push(Arena *arena, U64 size, U64 alignment)
{
  void *result = 0;
  Arena *current = arena->current;

  U64 aligned_pos = AlignPow2(current->pos, alignment);
  U64 new_pos = aligned_pos + size;

  // NOTE: if the current block can't handle the allocation, push a new block that can
  if(new_pos > current->capacity && current->growable) {
    // NOTE: try previous reserve/commit sizes; if insufficient, use enough to fit
    U64 reserve_size = current->reserve_size;
    U64 commit_size = current->commit_size;
    if(size + ARENA_HEADER_SIZE > reserve_size) {
      reserve_size = AlignPow2(size + ARENA_HEADER_SIZE, alignment);
      commit_size = reserve_size;
    }
    
    Arena *new_block = arena_alloc_ex(reserve_size, commit_size, 0, 1);
    Assert(new_block);
    new_block->base = current->base + current->pos;
    SLLStackPush_N(current, new_block, prev);
    current = new_block;
    aligned_pos = AlignPow2(current->pos, alignment);
    new_pos = aligned_pos + size;
  }

  // NOTE: ensure there are enough pages commited to handle the allocation
  if(new_pos > current->commit_pos) {
    // TODO: should we guarantee that the arena commit size is always a power of 2?
    U64 new_commit_pos = Min(Align(new_pos, current->commit_size), current->capacity);
    U64 commit_size = new_commit_pos - current->commit_pos;
    void *commit = (U8 *)current + current->commit_pos;
    B32 commit_result = os_mem_commit(commit, commit_size);
    Assert(commit_result);
    current->commit_pos = new_commit_pos;
  }

  // NOTE: increment the pos
  Assert(new_pos <= arena->capacity);  
  Assert(new_pos <= arena->commit_pos);
  result = (U8 *)current + aligned_pos;
  current->pos = new_pos;
  AsanUnpoison(result, size);
  
  return(result);
}

proc void*
arena_push_z(Arena *arena, U64 size, U64 alignment)
{
  void *result = arena_push(arena, size, alignment);
  ZeroSize(result, size);
  return(result);
}

proc U64
arena_pos(Arena *arena)
{
  Arena *current = arena->current;
  U64 result = current->base + current->pos;
  return(result);
}

proc void
arena_pop_to(Arena *arena, U64 pos)
{
  pos = Max(pos, ARENA_HEADER_SIZE);
  Arena *current = arena->current;
  // NOTE: pop chunks beyond the target position
  for(Arena *prev = 0; current->base >= pos; current = prev)
    {
      prev = current->prev;
      os_mem_release(current, current->capacity);
    }

  arena->current = current;
  U64 new_pos = pos - current->base;
  Assert(new_pos <= current->pos);
  AsanPoison((U8 *)current + new_pos, current->pos - new_pos);
  current->pos = new_pos;
}

proc void
arena_pop(Arena *arena, U64 ammount)
{
  U64 pos = arena_pos(arena);
  Assert(ammount <= pos);
  U64 new_pos = pos - ammount;
  arena_pop_to(arena, new_pos);
}

proc void
arena_clear(Arena *arena)
{
  arena_pop_to(arena, 0);
}

proc ArenaTemp
arena_begin_temp(Arena *arena)
{
  ArenaTemp result = {arena, arena_pos(arena)};
  return(result);
}

proc void
arena_end_temp(ArenaTemp temp)
{
  arena_pop_to(temp.arena, temp.pos);
}

#define ARENA_SCRATCH_POOL_COUNT 2
thread_var Arena *m__scratch_pool[ARENA_SCRATCH_POOL_COUNT] = {0};

proc ArenaTemp
arena_get_scratch(Arena **conflicts, U64 count)
{
  // NOTE: init
  if(m__scratch_pool[0] == 0) {
    Arena **scratch_slot = m__scratch_pool;
    for(U64 i = 0; i < ARENA_SCRATCH_POOL_COUNT; ++i, ++scratch_slot) {
      *scratch_slot = arena_alloc();
    }
  }

  // NOTE: get non-conflicting arena
  ArenaTemp result = {0};
  Arena **scratch_slot = m__scratch_pool;
  for(U64 i = 0; i < ARENA_SCRATCH_POOL_COUNT; ++i, ++scratch_slot) {
    B32 is_non_conflict = 1;
    Arena **conflict_ptr = conflicts;
    for(U64 j = 0; j < count; ++j, ++conflict_ptr) {
      if(*scratch_slot == *conflict_ptr) {
	is_non_conflict = 0;
	break;
      }
    }
    
    if(is_non_conflict) {
      result = arena_begin_temp(*scratch_slot);
      break;
    }
  }

  return(result);
}
