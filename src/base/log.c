// -----------------------------------------------------------------------------
// allocate, release, selection

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

// -----------------------------------------------------------------------------
// messaging

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

// -----------------------------------------------------------------------------
// scope management

proc void
log_begin_scope(String8 name)
{
  if(active_log != 0)
  {
    U64 pos = arena_pos(active_log->arena);
    LogScope *scope = arena_push_struct_z(active_log->arena, LogScope);
    scope->arena_pos = pos;
    scope->name = arena_push_str8_copy(active_log->arena, name);
    scope->depth = active_log->active_scope ? active_log->active_scope->depth + 1 : 0;
    SLLStackPush(active_log->active_scope, scope);
  }
}

proc LogScopeResult
log_end_scope(void)
{
  LogScopeResult result = {0};
  if(active_log != 0 && active_log->active_scope != 0)
  {
    LogScope *scope = active_log->active_scope;

    ArenaTemp scratch = arena_get_scratch(0, 0);
    {
      StringJoin joiner = {0};
      joiner.pre = str8_push_f(scratch.arena, "%*s%.*s\n%*s",
                               2*scope->depth, "",
                               (int)scope->name.count, scope->name.string,
                               2*(scope->depth + 1), "");
      joiner.sep = str8_push_f(scratch.arena, "\n%*s",
                               2*(scope->depth + 1), "");
      joiner.post = Str8Lit("\n");

      for(LogMessageKind msg_kind = 0; msg_kind < LogMessageKind_Count; ++msg_kind)
      {
        String8 msg = str8_join(scratch.arena, &scope->msgs[msg_kind], &joiner);
        result.msgs[msg_kind] = msg;
      }
    }
    arena_release_scratch(scratch);

    SLLStackPop(active_log->active_scope);
    arena_pop_to(active_log->arena, scope->arena_pos);
  }

  return(result);
}
