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
// window

proc B32
gfx_windows_are_equal(Gfx_Handle w1, Gfx_Handle w2)
{
  B32 result = w1.handle == w2.handle;
  return result;
}

// -----------------------------------------------------------------------------
// events

proc Gfx_EventSpan
gfx_events(void)
{
  // NOTE: flush unread events from buffer
  gfx__events_flush();

  // NOTE: fill buffer
#if GFX_BACKEND == GFX_BACKEND_WAYLAND
  wayland_events();
#elif GFX_BACKEND == GFX_BACKEND_WIN32
  win32_events();
#elif GFX_BACKEND == GFX_BACKEND_COCOA
  cocoa_events();
#endif

  // NOTE: return readable span
  Os_RingBufferSpan rb_span = os_ring_buffer_read_span(&gfx_state->event_buffer);
  U64 span_bytes = rb_span.end - rb_span.start;
  U64 span_events = span_bytes / sizeof(Gfx_Event);

  Gfx_EventSpan result = {0};
  result.first = (Gfx_Event*)rb_span.start;
  result.last = result.first + span_events;
  return result;
}

proc Gfx_EventIterator
gfx_event_iterator_start(void)
{
  gfx_events();
  Gfx_EventIterator result = {0};
  result.event = gfx__event_next();
  return result;
}

proc B32
gfx_event_iterator_done(Gfx_EventIterator *it)
{
  B32 result = it->event == 0;
  return result;
}

proc void
gfx_event_iterator_next(Gfx_EventIterator *it)
{
  gfx__event_pop(it->event);
  it->event = gfx__event_next();
}

proc Gfx_WindowEventIterator
gfx_window_event_iterator_start(Gfx_Handle window)
{
  // TODO: implement
  Assert(0);
  Unused(window);
  return (Gfx_WindowEventIterator){};
}

proc B32
gfx_window_event_iterator_done(Gfx_WindowEventIterator *it)
{
  // TODO: implement
  Assert(0);
  Unused(it);
  return 0;
}

proc void
gfx_window_event_iterator_next(Gfx_WindowEventIterator *it)
{
  // TODO: implement
  Assert(0);
  Unused(it);
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

/* proc Gfx_Event* */
/* gfx__event_list_push_new(Arena *arena, Gfx_EventList *list, Gfx_EventKind kind) */
/* { */
/*   Gfx_Event *event = arena_push_struct_z(arena, Gfx_Event); */
/*   event->kind = kind; */
/*   SLLQueuePush(list->first, list->last, event); */
/*   ++list->count; */
/*   return(event); */
/* } */
