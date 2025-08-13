typedef struct Os_Handle
{
  void *handle;
} Os_Handle;

typedef enum Os_EventKind
{
  Os_EventKind_move,
  Os_EventKind_press,
  Os_EventKind_release,
  Os_EventKind_scroll,  
  Os_EventKind_close,
  
  Os_EventKind_Count,
} Os_EventKind;

typedef struct Os_Event
{
  Os_EventKind kind;
  Value32 data[2];
} Os_Event;

typedef struct Os_EventNode Os_EventNode;
struct Os_EventNode
{
  Os_EventNode *next;
  Os_Event event;
};

typedef struct Os_EventList
{
  Os_EventNode *first;
  Os_EventNode *last;
  U64 count;
} Os_EventList;

// NOTE: helpers
proc void os_event_list_push_ex(Os_EventList *list, Os_Event event, Os_EventNode *node);
proc void os_event_list_push(Arena *arena, Os_EventList *list, Os_Event event);

// NOTE: interface
proc B32 os_gfx_init(void);

proc Os_Handle os_open_window(String8 name, S32 width, S32 height);
proc void os_close_window(Os_Handle window);

proc Os_EventList os_events_from_window(Os_Handle window);

proc V2S32 os_window_get_dim(Os_Handle window);

// TODO: move to render module
proc void os_window_begin_frame(Os_Handle window);
proc void os_window_end_frame(Os_Handle window);
