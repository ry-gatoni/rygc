proc Ui_State*
ui_state_alloc(void)
{
  Arena *arena = arena_alloc();
  Ui_State *result = arena_push_struct(arena, Ui_State);
  result->arena = arena;

  result->box_table_count = 4096;
  result->box_table = arena_push_array(arena, Ui_BoxBucket, result->box_table_count);
  return(result);
}

proc void
ui_state_release(Ui_State *state)
{
  arena_release(state->arena);
}

proc void
ui_select_state(Ui_State *state)
{
  ui_state = state;
}

proc B32
ui_box_is_nil(Ui_Box *box)
{
  B32 result = (box == 0 || box == &ui_nil_box);
  return(result);
}

proc Ui_Key
ui_key_from_string(String8 string)
{
  // TOOD: better hash function
  U64 hash = 5381;
  for(U32 i = 0; i < string.count; ++i) {
    hash = ((hash << 5) + hash + string.string[i]);
  }

  Ui_Key result = {0};
  result.hash = hash;
  return(result);
}

proc B32
ui_keys_match(Ui_Key a, Ui_Key b)
{
  B32 result = a.hash == b.hash;
  return(result);
}

proc Ui_Box*
ui_box_from_key(Ui_Key key)
{
  Ui_Box *result = &ui_nil_box;
  if(ui_state) {
    U64 box_table_idx = key.hash % ui_state->box_table_count;
    Ui_BoxBucket *bucket = ui_state->box_table + box_table_idx;
    for(Ui_Box *box = bucket->first; box; box = box->next) {
      if(ui_keys_match(box->key, key)) {
	result = box;
	break;
      }
    }
  }
  return(result);
}

proc void
ui_begin_build(Os_Handle window)
{
  ui_state->window = window;
  ui_state->root = &ui_nil_box;
}

proc void
ui_end_build(void)
{
  
}

proc Ui_Box*
ui_root(void)
{
  Ui_Box *result = 0;
  if(ui_state) {
    result = ui_state->root;
  }
  return(result);
}

proc Ui_Box*
ui_parent(void)
{
  Ui_Box *result = 0;
  if(ui_state != 0 && ui_state->parent_stack != 0) {
    result = ui_state->parent_stack->box;
  }
  return(result);
}

proc Ui_Box*
ui_build_box(Ui_BoxFlags flags, String8 string)
{  
  Ui_Key key = ui_key_from_string(string);
  Ui_Box *box = ui_build_box_from_key(flags, key);
  if(flags & Ui_BoxFlags_draw_text) {
    ui_box_equip_display_string(box, string);
  }

  return(box);
}

proc Ui_Box*
ui_build_boxf(Ui_BoxFlags flags, char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  ArenaTemp scratch = arena_get_scratch(0, 0);
  String8 string = str8_push_fv(scratch.arena, fmt, args);
  Ui_Box *box = ui_build_box(flags, string);
  va_end(args);
  arena_release_scratch(scratch);
  return(box);
}

proc Ui_Box*
ui_build_box_from_key(Ui_BoxFlags flags, Ui_Key key)
{
  Ui_Box *parent = ui_parent();
  Ui_Box *box = ui_box_from_key(key);
  B32 box_first_frame = ui_box_is_nil(box);

  if(box_first_frame) {
    // NOTE: allocate the box
    box = ui_state->box_freelist;
    if(!ui_box_is_nil(box)) {
      SLLStackPop(ui_state->box_freelist);
    }else {
      box = arena_push_struct(ui_state->arena, Ui_Box);
    }
    ZeroStruct(box);

    // NOTE: put the box in the persistent state table
    U64 slot_idx = key.hash % ui_state->box_table_count;
    Ui_BoxBucket *bucket = ui_state->box_table + slot_idx;
    DLLPushBack_NP(bucket->first, bucket->last, box, hash_next, hash_prev);
  }

  // TODO: clear non-persistent box data

  if(!ui_box_is_nil(parent)) {
    // NOTE: put the box in the per-frame tree
    DLLPushBack(parent->first, parent->last, box);
    ++parent->child_count;
    box->parent = parent;
  }

  // NOTE: set box data
  box->flags = flags;
  box->key = key;

  return(box);
}

proc void
ui_box_equip_display_string(Ui_Box *box, String8 string)
{
  box->string = string;
}

proc Ui_Comm
ui_comm_from_box(Ui_Box *box)
{
  Ui_Comm result = {0};
  result.box = box;
  return(result);
}
