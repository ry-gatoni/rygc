#if 1
PROFILE_ON_TOP_SCOPE_EXIT_PROC(profile_on_top_scope_exit_default)
{
  Unused(user_data);
  return;
}

global ProfileOnTopScopeExitProc *profile_on_top_scope_exit = profile_on_top_scope_exit_default;
global void *profile_callback_user_data = 0;

proc void
profile_set_on_top_scope_exit_callback(ProfileOnTopScopeExitProc *cb, void *cb_data)
{
  profile_on_top_scope_exit = cb;
  profile_callback_user_data = cb_data;
}

proc void
profile_begin_scope(ProfileEvent *event)
{
  profile__current_parent = event->site;
  event->tsc_elapsed_root_old = event->site->tsc_elapsed_root;
  event->tsc_start = cpu_counter_fixed(); // TODO: actual cycle counters
}

proc void
profile_end_scope(ProfileEvent *event)
{
  U64 tsc_elapsed = cpu_counter_fixed() - event->tsc_start; // TODO: actual cycle counters

  ProfileSite *ev_site = event->site;
  ev_site->tsc_elapsed += tsc_elapsed;
  ev_site->tsc_elapsed_root = event->tsc_elapsed_root_old + tsc_elapsed;
  ev_site->hit_count++;

  ProfileSite *ev_parent = event->parent;
  ev_parent->tsc_elapsed_children += tsc_elapsed;
  profile__current_parent = ev_parent;

  if(profile__current_parent == ProfileNil)
  {
    // NOTE: top-level profiled scope exited, call configurable callback
    profile_on_top_scope_exit(profile_callback_user_data);

    // NOTE: clear per-run site data
    ProfileSite *sites = profile_site_array_base();
    U64 site_count = profile_site_array_count();
    for(U64 site_idx = 0; site_idx < site_count; ++site_idx)
    {
      ProfileSite *site = sites + site_idx;
      // TODO: filter out sites this thread didn't hit; think about what should
      // happen in multithreaded context where multiple threads run the same
      // instructions
      site->hit_count = 0;
      site->bytes_allocated = 0;
      site->tsc_elapsed = 0;
      site->tsc_elapsed_children = 0;
      site->tsc_elapsed_root = 0;
    }
  }
}
#else
// -----------------------------------------------------------------------------
// allocate, release, select

proc Profiler*
profile_alloc(void)
{
  Arena *arena = arena_alloc();
  Profiler *result = arena_push_struct(arena, Profiler);
  result->arena = arena;
  result->profile_entry_count = profile_entry_array_count();
  result->profile_entries = profile_entry_array_base();
  result->counter_freq = cpu_counter_freq();
  return(result);
}

proc void
profile_release(Profiler *profiler)
{
  Arena *arena = profiler->arena;
  arena_release(arena);
  profiler->arena = 0;
}

proc void
profile_select(Profiler *profiler)
{
  active_profiler = profiler;
  active_profiler->thread_id= os_thread_id();
}

// -----------------------------------------------------------------------------
// toplevel

proc void
profile_begin(void)
{
  if(active_profiler != 0)
  {
    /* log_begin_scope(Str8Lit("profile")); */
    active_profiler->current_parent = &profile__0;
    active_profiler->start_tsc = cpu_counter();
  }
}

proc void
profile_end(void)
{
  if(active_profiler != 0)
  {
    active_profiler->end_tsc = cpu_counter();
    U64 tsc_elapsed = active_profiler->end_tsc - active_profiler->start_tsc;
    R64 seconds_elapsed = (R64)tsc_elapsed / (R64)active_profiler->counter_freq;

    log_infof("tsc elapsed: %lu (%.4f ms)", tsc_elapsed, 1000.0*seconds_elapsed);
    for(U64 entry_idx = 0; entry_idx < active_profiler->profile_entry_count; ++entry_idx)
    {
      ProfileEntry *entry = active_profiler->profile_entries + entry_idx;
      if(entry != profile_entry_null)
      {
        if(entry->thread_id == active_profiler->thread_id)
	{
	  if(entry->hit_count)
	  {
	    U64 tsc_elapsed__self = entry->tsc_elapsed - entry->tsc_elapsed_children;
	    R64 percent = 100.0 * ((R64)tsc_elapsed__self/(R64)tsc_elapsed);
	    if(entry->tsc_elapsed_root != tsc_elapsed__self)
	    {
	      R64 percent_with_children = 100.0 * ((R64)entry->tsc_elapsed_root/(R64)tsc_elapsed);
	      log_infof("%.*s[%lu]: %lu(%lu) (%.2f%%, %.2f%% w/ children)",
			(int)entry->label.count, entry->label.string,
			entry->hit_count,
			tsc_elapsed__self,
			entry->tsc_elapsed_root,
			percent,
			percent_with_children);
	    }
	    else
	    {
	      log_infof("%.*s[%lu]: %lu(%lu) (%.2f%%)",
			(int)entry->label.count, entry->label.string,
			entry->hit_count,
			tsc_elapsed__self,
			entry->tsc_elapsed_root,
			percent);
	    }

	    // NOTE: write log messages to console
	    /* LogScopeResult profile_log = log_end_scope(); */
	    /* String8 profile_log_info = profile_log.msgs[LogMessageKind_info]; */
	    /* fprintf(stdout, "%.*s", (int)profile_log_info.count, profile_log_info.string); */

	    // NOTE: clear entry
	    entry->tsc_elapsed = 0;
	    entry->tsc_elapsed_children = 0;
	    entry->tsc_elapsed_root = 0;
	    entry->hit_count = 0;
	  }
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
// per-block

proc void
profile_begin_block(ProfileEntry *entry)
{
  if(active_profiler != 0)
  {
    entry->thread_id= os_thread_id();
    entry->tsc_elapsed_root__old = entry->tsc_elapsed_root;
    entry->parent = active_profiler->current_parent;
    active_profiler->current_parent = entry;
    entry->start_tsc = cpu_counter();
  }
}

proc void
profile_end_block(ProfileEntry *entry)
{
  if(active_profiler != 0)
  {
    U64 end_tsc = cpu_counter();
    U64 tsc_elapsed = end_tsc - entry->start_tsc;
    active_profiler->current_parent = entry->parent;

    entry->parent->tsc_elapsed_children += tsc_elapsed;

    entry->tsc_elapsed += tsc_elapsed;
    entry->tsc_elapsed_root = entry->tsc_elapsed_root__old + tsc_elapsed;
    entry->hit_count += 1;
  }
}
#endif
