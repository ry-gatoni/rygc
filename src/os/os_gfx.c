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
