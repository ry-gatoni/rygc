proc Log*
log_alloc(void)
{
  Arena *arena = arena_alloc();
  Log *result = arena_push_struct(arena, Log);
  result->arena = arena;
  return(result);
}

proc void
log_release(Log *log)
{
  arena_release(log->arena);
}

proc void
log_select(Log *log)
{
  active_log = log;
}

proc void
log_msg(LogMessageKind msg_kind, String8 msg)
{
  if(active_log != 0 && active_log->active_scope != 0) {    
    String8 msg_copy = arena_push_str8_copy(active_log->arena, msg);
    str8_list_push(active_log->arena, &active_log->active_scope->msgs[msg_kind], msg_copy);
  }
}

proc void
log_msgf(LogMessageKind msg_kind, char *fmt, ...)
{
  if(active_log != 0 && active_log->active_scope != 0) {    
    va_list args;
    va_start(args, fmt);
    str8_list_push_fv(active_log->arena, &active_log->active_scope->msgs[msg_kind], fmt, args);
    va_end(args);
  }
}

proc void
log_begin_scope(void)
{
  if(active_log != 0) {
    U64 pos = arena_pos(active_log->arena);
    LogScope *scope = arena_push_struct(active_log->arena, LogScope);
    scope->arena_pos = pos;
    SLLStackPush(active_log->active_scope, scope);
  }
}

proc LogScopeResult
log_end_scope(void)
{
  LogScopeResult result = {0};
  if(active_log != 0 && active_log->active_scope != 0) {
    
    LogScope *scope = active_log->active_scope;
    ArenaTemp scratch = arena_get_scratch(0, 0);
    for(LogMessageKind msg_kind = 0; msg_kind < LogMessageKind_Count; ++msg_kind) {
      String8 msg = str8_join(scratch.arena, &scope->msgs[msg_kind]);
      result.msgs[msg_kind] = msg;
    }    
    arena_release_scratch(scratch);
    arena_pop_to(active_log->arena, scope->arena_pos);
    SLLStackPop(active_log->active_scope);
  }

  return(result);
}
