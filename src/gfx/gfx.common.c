// -----------------------------------------------------------------------------
// init

proc B32
gfx_init(void)
{
  Arena *arena = arena_alloc();
  gfx_state = arena_push_struct(arena, Gfx_State);
  gfx_state->arena = arena;
  os_ring_buffer_init(&gfx_state->event_buffer, KB(64));

  B32 result;
#if GFX_BACKEND == GFX_BACKEND_WAYLAND
  result = wayland_init(arena);
#elif GFX_BACKEND == GFX_BACKEND_WIN32
  result = win32_init(arena);
#elif GFX_BACKEND == GFX_BACKEND_COCOA
  result = cocoa_init(arena);
#endif
  return result;
}

proc void
gfx_uninit(void)
{
#if GFX_BACKEND == GFX_BACKEND_WAYLAND
  wayland_uninit();
#elif GFX_BACKEND == GFX_BACKEND_WIN32
  win32_uninit();
#elif GFX_BACKEND == GFX_BACKEND_COCOA
  cocoa_uninit();
#endif

  os_ring_buffer_release(&gfx_state->event_buffer);
  arena_release(gfx_state->arena);
  gfx_state = 0;
}

// -----------------------------------------------------------------------------
// helpers

proc Gfx_Event*
gfx__event_new(void)
{
  Os_RingBufferSpan events = os_ring_buffer_write_span(&gfx_state->event_buffer);
  Gfx_Event *result = 0;
  if((U64)(events.end - events.start) >= sizeof(*result))
  { result = (Gfx_Event*)events.start; }
  return result;
}

proc void
gfx__event_push(Gfx_Event *event)
{
  if(event)
  { os_ring_buffer_write_end(&gfx_state->event_buffer, sizeof(*event)); }
}

proc Gfx_Event*
gfx__event_next(void)
{
  Os_RingBufferSpan events = os_ring_buffer_read_span(&gfx_state->event_buffer);
  Gfx_Event *result = 0;
  if((U64)(events.end - events.start) >= sizeof(*result))
  { result = (Gfx_Event*)events.start; }
  return result;
}

proc void
gfx__event_pop(Gfx_Event *event)
{
  if(event)
  { os_ring_buffer_read_end(&gfx_state->event_buffer, sizeof(*event)); }
}

proc void
gfx__events_flush(void)
{
  Os_RingBuffer *rb = &gfx_state->event_buffer;
  Os_RingBufferSpan events = os_ring_buffer_read_span(rb);
  os_ring_buffer_read_end(rb, events.end - events.start);
}

proc Gfx_Event*
gfx__event_list_push_new(Arena *arena, Gfx_EventList *list, Gfx_EventKind kind)
{
  Gfx_Event *event = arena_push_struct_z(arena, Gfx_Event);
  event->kind = kind;
  SLLQueuePush(list->first, list->last, event);
  ++list->count;
  return(event);
}
