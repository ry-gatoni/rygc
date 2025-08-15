typedef U32 Ui_BoxFlags;
enum
{
  // NOTE: interaction
  Ui_BoxFlags_clickable	      = (1 << 0),
  
  // NOTE: appearance
  Ui_BoxFlags_draw_background = (1 << 1),
  Ui_BoxFlags_draw_border     = (1 << 2),
  Ui_BoxFlags_draw_text	      = (1 << 3),
};

typedef struct Ui_Key
{
  U64 hash;
} Ui_Key;

typedef struct Ui_Box Ui_Box;
struct Ui_Box
{
  // NOTE: persistent links
  Ui_Box *hash_next;
  Ui_Box *hash_prev;

  // NOTE: per-build links
  Ui_Box *first;
  Ui_Box *last;
  Ui_Box *next;
  Ui_Box *prev;
  Ui_Box *parent;
  U64 child_count;  

  // NOTE: specified per-build
  Ui_BoxFlags flags;
  Ui_Key key;
  String8 string;
  void *user_data;

  // NOTE: computed per-build
  Rect2 rect;

  // NOTE: persistent data
  
};

typedef U32 Ui_CommFlags;
enum
{
  Ui_CommFlag_hovering	      = (1 << 0),
  
  Ui_CommFlag_left_pressed    = (1 << 1),
  Ui_CommFlag_right_pressed   = (1 << 2),
  Ui_CommFlag_middle_pressed  = (1 << 3),
  
  Ui_CommFlag_left_released   = (1 << 4),
  Ui_CommFlag_right_released  = (1 << 5),
  Ui_CommFlag_middle_released = (1 << 6),
  
  Ui_CommFlag_left_clicked    = (1 << 7),
  Ui_CommFlag_right_clicked   = (1 << 8),
  Ui_CommFlag_middle_clicked  = (1 << 9),
  
  Ui_CommFlag_left_dragging   = (1 << 10),
  Ui_CommFlag_right_dragging  = (1 << 11),
  Ui_CommFlag_middle_dragging = (1 << 12),
  
  Ui_CommFlag_key_pressed     = (1 << 13),

  Ui_CommFlag_pressed  = Ui_CommFlag_left_pressed|Ui_CommFlag_key_pressed,
  Ui_CommFlag_released = Ui_CommFlag_left_released,
  Ui_CommFlag_clicked  = Ui_CommFlag_left_clicked|Ui_CommFlag_key_pressed,
  Ui_CommFlag_dragging = Ui_CommFlag_left_dragging,
};

typedef struct Ui_Comm
{
  Ui_Box *box;
  Ui_CommFlags flags;
} Ui_Comm;

#define ui_hovering(comm) !!((comm).flags & Ui_CommFlag_hovering)
#define ui_pressed(comm)  !!((comm).flags & Ui_CommFlag_pressed)
#define ui_released(comm) !!((comm).flags & Ui_CommFlag_released)
#define ui_clicked(comm)  !!((comm).flags & Ui_CommFlag_clicked)
#define ui_dragging(comm) !!((comm).flags & Ui_CommFlag_dragging)

typedef struct Ui_BoxBucket
{
  Ui_Box *first;
  Ui_Box *last;
} Ui_BoxBucket;

typedef struct Ui_ParentNode Ui_ParentNode;
struct Ui_ParentNode
{
  Ui_ParentNode *next;
  Ui_Box *box;
};

typedef struct Ui_State
{
  Arena *arena;

  // NOTE: box cache
  Ui_Box *box_freelist;
  Ui_BoxBucket *box_table;
  U64 box_table_count;

  // NOTE: build tree
  Ui_Box *root;
  U64 box_count;

  Ui_ParentNode *parent_stack;

  // NOTE: build parameters
  Os_Handle window;

  // NOTE: interaction state
} Ui_State;

global Ui_State *ui_state = 0;

read_only global Ui_Box ui_nil_box = {
  .hash_next = &ui_nil_box,
  .hash_prev = &ui_nil_box,
  .first     = &ui_nil_box,
  .last      = &ui_nil_box,
  .next      = &ui_nil_box,
  .prev      = &ui_nil_box,
  .parent    = &ui_nil_box,
};

// NOTE: interface
// NOTE: state alloation/equipment
proc Ui_State* ui_state_alloc(void);
proc void ui_state_release(Ui_State *state);
proc void ui_select_state(Ui_State *state);

proc B32 ui_box_is_nil(Ui_Box *box);

// NOTE: key management
proc Ui_Key ui_key_from_string(String8 string);
proc B32 ui_keys_match(Ui_Key a, Ui_Key b);
proc Ui_Box* ui_box_from_key(Ui_Key key);

// NOTE: ui state getters
proc Ui_Box* ui_root(void);
proc Ui_Box* ui_parent(void);

// NOTE: box building
proc void ui_begin_build(Os_Handle window);
proc void ui_end_build(void);

proc Ui_Box* ui_build_box(Ui_BoxFlags flags, String8 string);
proc Ui_Box* ui_build_boxf(Ui_BoxFlags flags, char *fmt, ...);
proc Ui_Box* ui_build_box_from_key(Ui_BoxFlags flags, Ui_Key key);

// NOTE: ui box setters
proc void ui_box_equip_display_string(Ui_Box *box, String8 string);

// NOTE: box interface
proc Ui_Comm ui_comm_from_box(Ui_Box *box);
