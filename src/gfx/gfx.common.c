proc Os_Event*
os_event_list_push_new(Arena *arena, Os_EventList *list, Os_EventKind kind)
{
  Os_Event *event = arena_push_struct_z(arena, Os_Event);
  event->kind = kind;
  SLLQueuePush(list->first, list->last, event);
  ++list->count;
  return(event);
}
