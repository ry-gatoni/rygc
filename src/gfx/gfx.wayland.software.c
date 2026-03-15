proc B32
wayland_software__allocate_shared_memory(WaylandSoftware_Window *window, U64 size)
{
  Wayland_Window *wayland_window = window->wayland_window;

  B32 result = 0;
  int shared_memory_handle = -1;
  void *shared_memory = MAP_FAILED;
  Wayland_Id *wl_shm_pool = 0;

  ArenaTemp scratch = arena_get_scratch(0, 0);

  // TODO: use memfd_create() on linux instead of creating a file by hand?
  char *filename = "fhgjkaspytqwczvxbteqiomzxcvalwxpm";
  shared_memory_handle = shm_open(filename, O_RDWR | O_EXCL | O_CREAT, S_IRUSR | S_IWUSR);
  if(shared_memory_handle == -1) goto wayland_allocate_shared_memory_failure;

  int shm_unlink_result = shm_unlink(filename);
  if(shm_unlink_result == -1) goto wayland_allocate_shared_memory_failure;
  int ftruncate_result = ftruncate(shared_memory_handle, size);
  if(ftruncate_result == -1) goto wayland_allocate_shared_memory_failure;
  shared_memory = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_handle, 0);
  if(shared_memory == MAP_FAILED) goto wayland_allocate_shared_memory_failure;
  window->shared_memory = shared_memory;
  window->shared_memory_size = size;

  wl_shm_pool = wayland__new_id(wayland_window);
  B32 shm_create_pool_result =
    wl_shm_create_pool(wayland_state->wl_shm->id, wl_shm_pool->id, shared_memory_handle, size);
  if(!shm_create_pool_result) goto wayland_allocate_shared_memory_failure;
  window->wl_shm_pool = wl_shm_pool;
  DLLPushBack(wayland_state->first_wl_shm_pool, wayland_state->last_wl_shm_pool, wl_shm_pool);
  ++wayland_state->wl_shm_pool_count;

  result = 1;

  if(!result){
wayland_allocate_shared_memory_failure:
    if(shared_memory_handle != -1)
    {
      close(shared_memory_handle);
    }
    if(shared_memory != MAP_FAILED)
    {
      munmap(shared_memory, size);
      window->shared_memory = 0;
      window->shared_memory_size = 0;
    }
    if(wl_shm_pool)
    {
      wayland__release_id(wl_shm_pool);
    }
  }

  arena_release_scratch(scratch);
  return(result);
}

proc B32
wayland_software__create_shm_buffer(WaylandSoftware_Window *window)
{
  B32 result = 1;

  Wayland_Window *wayland_window = window->wayland_window;

  S32 stride = wayland_window->width*sizeof(U32);
  U32 format = 0x34324241; // NOTE: ABGR 32-bit little-endian // TODO: generate enums

  //Assert(!window->buffer_id[window->backbuffer_index]);
  Wayland_Id *buffer = wayland__new_id(wayland_window);
  if(wl_shm_pool_create_buffer(window->wl_shm_pool->id, buffer->id,
                               wayland_window->backbuffer_index*window->shared_memory_size/2,
                               wayland_window->width, wayland_window->height, stride, format))
  {
    wayland_window->buffers[wayland_window->backbuffer_index] = buffer;
    DLLPushBack(wayland_state->first_wl_buffer, wayland_state->last_wl_buffer, buffer);
    ++wayland_state->wl_buffer_count;
  }
  else
  {
    // NOTE: buffer creation failed
    result = 0;
  }

  return(result);
}

proc WaylandSoftware_Window*
wayland_software__window_create(Arena *arena)
{
  WaylandSoftware_Window *result = wayland_software_state->freelist;
  if(result)
  {
    SLLStackPop_N(wayland_software_state->freelist, next_free);
  }
  else
  {
    result = arena_push_struct(arena, WaylandSoftware_Window);
  }
  Assert(result);
  return(result);
}

proc void
wayland_software__window_destroy(WaylandSoftware_Window *window)
{
  munmap(window->shared_memory, window->shared_memory_size);
  SLLStackPush_N(wayland_software_state->freelist, window, next_free);
}

proc WaylandSoftware_Window*
wayland_software__window_get(Wayland_Window *window)
{
  WaylandSoftware_Window *result = window->backends[Wayland_Backend_software];
  return(result);
}

proc U32*
wayland_software__window_get_pixels(WaylandSoftware_Window *window)
{
  Wayland_Window *wayland_window = window->wayland_window;
  U32 *result = (U32*)((U8*)window->shared_memory +
		       wayland_window->backbuffer_index*window->shared_memory_size/2);
  return(result);
}

proc WaylandSoftware_Window*
wayland_software_window_open(Wayland_Window *window)
{
  Arena *arena = wayland_state->arena;
  U64 arena_pre_pos = arena_pos(arena);
  WaylandSoftware_Window *software_window = wayland_software__window_create(arena);
  software_window->wayland_window = window;

  U64 shared_memory_size = 1920*1080*sizeof(U32)*2;
  B32 result = wayland_software__allocate_shared_memory(software_window, shared_memory_size);
  if(!result)
  {
    arena_pop_to(arena, arena_pre_pos);
    software_window = 0;
  }

  return(software_window);
}

proc void
wayland_software_window_close(WaylandSoftware_Window *window)
{
  wl_shm_pool_destroy(window->wl_shm_pool->id);
  wayland__release_id(window->wl_shm_pool);

  wayland_software__window_destroy(window);
}

proc B32
wayland_software_init(void)
{
  wayland_software_state = arena_push_struct(wayland_state->arena, WaylandSoftware_State);
  B32 result = wayland_software_state != 0;
  if(result)
  {
    Wayland_BackendApi *software_backend = wayland_state->backends + Wayland_Backend_software;
    software_backend->create_buffer = wayland_software_window_create_buffer;
    software_backend->begin_frame = wayland_software_window_begin;
    software_backend->end_frame = wayland_software_window_end;
  }
  return(result);
}

proc void
wayland_software_uninit(void)
{
  ZeroStruct(wayland_state->backends + Wayland_Backend_software);
}

proc B32
wayland_software_window_create_buffer(Wayland_Window *window)
{
  WaylandSoftware_Window *software_window = wayland_software__window_get(window);
  B32 result = wayland_software__create_shm_buffer(software_window);
  return(result);
}

proc B32
wayland_software_window_begin(Wayland_Window *window)
{
  Unused(window);
  return(1);
}

proc B32
wayland_software_window_end(Wayland_Window *window)
{
  Unused(window);
  return(1);
}
