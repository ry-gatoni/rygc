typedef enum Os_EventKind
{
  Os_EventKind_move,
  Os_EventKind_press,
  Os_EventKind_release,
  Os_EventKind_scroll,
  Os_EventKind_close,

  Os_EventKind_Count,
} Os_EventKind;

// NOTE: name, display string
#define OS_KEY_XLIST\
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

typedef enum Os_Key
{
#define X(name, str) Glue(Os_Key_, name),
  OS_KEY_XLIST
#undef X
  Os_Key_Count,
} Os_Key;

global String8 os_key_display_strings[Os_Key_Count] = {
#define X(name, str) [Glue(Os_Key_, name)] = Str8Lit(str),
  OS_KEY_XLIST
#undef X
};

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
  Os_Key key; // NOTE: this represents the _physical key_ on the keyboard/mouse that was pressed, _not_ the key symbol unicode codepoint the keypress maps to according to the user's layout
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

// -----------------------------------------------------------------------------
// helpers

proc Os_Event* os_event_list_push_new(Arena *arena, Os_EventList *list, Os_EventKind kind);

// -----------------------------------------------------------------------------
// interface

proc B32 os_gfx_init(void);

proc Os_Handle os_open_window(String8 name, S32 width, S32 height);
proc void os_close_window(Os_Handle window);

proc Os_EventList os_events(Arena *arena);

proc V2S32 os_window_get_dim(Os_Handle window);

// -----------------------------------------------------------------------------
// rendering
// TODO: move to render module

proc void os_window_begin_frame(Os_Handle window);
proc void os_window_end_frame(Os_Handle window);
