typedef enum Os_EventKind
{
  Os_EventKind_move,
  Os_EventKind_press,
  Os_EventKind_release,
  Os_EventKind_scroll,  
  Os_EventKind_close,
  
  Os_EventKind_Count,
} Os_EventKind;

typedef enum Os_Key
{
  Os_Key_null = 0,
  
  Os_Key_mouse_left,
  Os_Key_mouse_middle,
  Os_Key_mouse_right,
  Os_Key_mouse_side,
  Os_Key_mouse_extra,
  Os_Key_mouse_forward,
  Os_Key_mouse_back,
  Os_Key_mouse_task,

  Os_Key_joystick_trigger,
  Os_Key_joystick_thumb,
  Os_Key_joystick_thumb2,
  Os_Key_joystick_top,
  Os_Key_joystick_top2,
  Os_Key_joystick_pinkie,
  Os_Key_joystick_base,
  Os_Key_joystick_base2,
  Os_Key_joystick_base3,
  Os_Key_joystick_base4,
  Os_Key_joystick_base5,
  Os_Key_joystick_base6,

  // TODO: keys
  Os_Key_keyboard_backspace,
  
  Os_Key_Count,
} Os_Key;

typedef struct Os_Event Os_Event;
struct Os_Event
{
  Os_Event *next;
  Os_Event *prev;

  Os_EventKind kind;
  Os_Handle window;
  //Value32 data[2];
  V2 pos;
  V2 scroll;
  Os_Key key;
  Os_Key modifiers;
};

/* typedef struct Os_EventNode Os_EventNode; */
/* struct Os_EventNode */
/* { */
/*   Os_EventNode *next; */
/*   Os_Event event; */
/* }; */

typedef struct Os_EventList
{
  Os_Event *first;
  Os_Event *last;
  U64 count;
} Os_EventList;

// NOTE: helpers
//proc void os_event_list_push_ex(Os_EventList *list, Os_Event event, Os_EventNode *node);
//proc void os_event_list_push(Arena *arena, Os_EventList *list, Os_Event event);
proc Os_Event* os_event_list_push_new(Arena *arena, Os_EventList *list, Os_EventKind kind);

// NOTE: interface
proc B32 os_gfx_init(void);

proc Os_Handle os_open_window(String8 name, S32 width, S32 height);
proc void os_close_window(Os_Handle window);

//proc Os_EventList os_events_from_window(Os_Handle window); // TODO: deprecate
proc Os_EventList os_events(Arena *arena);

proc V2S32 os_window_get_dim(Os_Handle window);

// TODO: move to render module
proc void os_window_begin_frame(Os_Handle window);
proc void os_window_end_frame(Os_Handle window);
