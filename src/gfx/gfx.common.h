typedef struct Gfx_Handle
{
  void *handle;
} Gfx_Handle;

typedef enum Gfx_EventKind
{
  Gfx_EventKind_move,
  Gfx_EventKind_press,
  Gfx_EventKind_release,
  Gfx_EventKind_scroll,
  Gfx_EventKind_close,

  Gfx_EventKind_Count,
} Gfx_EventKind;

// NOTE: name, display string
#define GFX_KEY_XLIST\
  X(null, "Invalid Key")\
  X(esc, "Esc")\
  X(f1, "F1")\
  X(f2, "F2")\
  X(f3, "F3")\
  X(f4, "F4")\
  X(f5, "F5")\
  X(f6, "F6")\
  X(f7, "F7")\
  X(f8, "F8")\
  X(f9, "F9")\
  X(f10, "F10")\
  X(f11, "F11")\
  X(f12, "F12")\
  X(backtick, "Backtick")\
  X(1, "1")\
  X(2, "2")\
  X(3, "3")\
  X(4, "4")\
  X(5, "5")\
  X(6, "6")\
  X(7, "7")\
  X(8, "8")\
  X(9, "9")\
  X(0, "0")\
  X(hyphen, "Hyphen")\
  X(equals, "Equals")\
  X(backspace, "Backspace")\
  X(tab, "Tab")\
  X(q, "Q")\
  X(w, "W")\
  X(e, "E")\
  X(r, "R")\
  X(t, "T")\
  X(y, "Y")\
  X(u, "U")\
  X(i, "I")\
  X(o, "O")\
  X(p, "P")\
  X(l_bracket, "Left Bracket")\
  X(r_bracket, "Right Bracket")\
  X(backslash, "Backslash")\
  X(capslock, "Caps Lock")\
  X(a, "A")\
  X(s, "S")\
  X(d, "D")\
  X(f, "F")\
  X(g, "G")\
  X(h, "H")\
  X(j, "J")\
  X(k, "K")\
  X(l, "L")\
  X(semicolon, "Semicolon")\
  X(quote, "Quote")\
  X(enter, "Enter")\
  X(shift, "Shift")\
  X(z, "Z")\
  X(x, "X")\
  X(c, "C")\
  X(v, "V")\
  X(b, "B")\
  X(n, "N")\
  X(m, "M")\
  X(comma, "Comma")\
  X(period, "Period")\
  X(slash, "Slash")\
  X(control, "Control")\
  X(super, "Super")\
  X(alt, "Alt")\
  X(space, "Space")\
  X(pause, "Pause")\
  X(insert, "Insert")\
  X(home, "Home")\
  X(page_up, "Page Up")\
  X(delete, "Delete")\
  X(end, "End")\
  X(page_down, "Page Down")\
  X(up, "Up")\
  X(left, "Left")\
  X(down, "Down")\
  X(right, "Right")\
  X(numpad_slash, "Numpad Slash")\
  X(numpad_star, "Numpad Star")\
  X(numpad_minus, "Numpad Minus")\
  X(numpad_plus, "Numpad Plus")\
  X(numpad_enter, "Numpad Enter")\
  X(numpad_dot, "Numpad Dot")\
  X(numpad_0, "Numpad 0")\
  X(numpad_1, "Numpad 1")\
  X(numpad_2, "Numpad 2")\
  X(numpad_3, "Numpad 3")\
  X(numpad_4, "Numpad 4")\
  X(numpad_5, "Numpad 5")\
  X(numpad_6, "Numpad 6")\
  X(numpad_7, "Numpad 7")\
  X(numpad_8, "Numpad 8")\
  X(numpad_9, "Numpad 9")\
  X(mute, "Mute")\
  X(volume_down, "Volume Up")\
  X(volume_up, "Volume Down")\
  X(sleep, "Sleep")\
  X(mouse_left, "Left Mouse Button")\
  X(mouse_middle, "Middle Mouse Button")\
  X(mouse_right, "Right Mouse Button")

typedef enum Gfx_Key
{
#define X(name, str) Glue(Gfx_Key_, name),
  GFX_KEY_XLIST
#undef X
  Gfx_Key_Count,
} Gfx_Key;

global String8 gfx_key_display_strings[Gfx_Key_Count] = {
#define X(name, str) [Glue(Gfx_Key_, name)] = Str8Lit(str),
  GFX_KEY_XLIST
#undef X
};

typedef struct Gfx_Event Gfx_Event;
struct Gfx_Event
{
  /* Gfx_Event *next; */
  /* Gfx_Event *prev; */

  Gfx_EventKind kind;
  Gfx_Handle window;
  //Value32 data[2];
  V2 pos;
  V2 scroll;
  Gfx_Key key; // NOTE: this represents the _physical key_ on the keyboard/mouse that was pressed, _not_ the key symbol unicode codepoint the keypress maps to according to the user's layout
  Gfx_Key modifiers;
};

typedef struct Gfx_EventSpan
{
  Gfx_Event *first;
  Gfx_Event *last;
} Gfx_EventSpan;

typedef struct Gfx_EventIterator
{
  Gfx_Event *event;
} Gfx_EventIterator;

typedef struct Gfx_WindowEventIterator
{
  Gfx_Event *event;
} Gfx_WindowEventIterator;

/* typedef struct Gfx_EventList */
/* { */
/*   Gfx_Event *first; */
/*   Gfx_Event *last; */
/*   U64 count; */
/* } Gfx_EventList; */

typedef struct Gfx_State
{
  Arena *arena;
  Os_RingBuffer event_buffer;
} Gfx_State;

global Gfx_State *gfx_state = 0;

// -----------------------------------------------------------------------------
// init

proc B32 gfx_init(void);
proc void gfx_uninit(void);

// -----------------------------------------------------------------------------
// window

proc Gfx_Handle gfx_window_open(S32 width, S32 height, String8 name);
proc void gfx_window_close(Gfx_Handle window);

proc B32 gfx_windows_are_equal(Gfx_Handle w1, Gfx_Handle w2);

proc V2S32 gfx_window_get_dim(Os_Handle window);

// -----------------------------------------------------------------------------
// events

proc Gfx_EventSpan gfx_events(); // flush event queue and fill with platform events

proc Gfx_EventIterator gfx_event_iterator_start(void);
proc B32 gfx_event_iterator_done(Gfx_EventIterator *it);
proc void gfx_event_iterator_next(Gfx_EventIterator *it);

proc Gfx_WindowEventIterator gfx_window_event_iterator_start(Gfx_Handle window);
proc B32 gfx_window_event_iterator_done(Gfx_WindowEventIterator *it);
proc void gfx_window_event_iterator_next(Gfx_WindowEventIterator *it);

//proc Gfx_EventList gfx_events(Arena *arena);

// -----------------------------------------------------------------------------
// rendering
// TODO: move to render module

proc void gfx_window_begin_frame(Os_Handle window);
proc void gfx_window_end_frame(Os_Handle window);

// -----------------------------------------------------------------------------
// helpers

// events
proc Gfx_Event* gfx__event_new(void);
proc void gfx__event_push(Gfx_Event *event);

proc Gfx_Event* gfx__event_next(void);
proc void gfx__event_pop(Gfx_Event *event);
proc void gfx__events_flush(void);

//proc Gfx_Event* gfx__event_list_push_new(Arena *arena, Gfx_EventList *list, Gfx_EventKind kind);
