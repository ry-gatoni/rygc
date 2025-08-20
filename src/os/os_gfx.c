#if 0
proc void
os_event_list_push_ex(Os_EventList *list, Os_Event event, Os_EventNode *node)
{
  node->event = event;
  SLLQueuePush(list->first, list->last, node);
  ++list->count;
}

proc void
os_event_list_push(Arena *arena, Os_EventList *list, Os_Event event)
{
  Os_EventNode *node = arena_push_struct_z(arena, Os_EventNode);
  os_event_list_push_ex(list, event, node);
}
#endif

proc Os_Event*
os_event_list_push_new(Arena *arena, Os_EventList *list, Os_EventKind kind)
{
  Os_Event *event = arena_push_struct_z(arena, Os_Event);
  event->kind = kind;
  SLLQueuePush(list->first, list->last, event);
  ++list->count;
  return(event);
}
