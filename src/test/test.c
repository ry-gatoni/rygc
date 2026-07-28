BeforeMain__Named(run_all_tests)
{
  Arena *arena = arena_alloc();
  String8List log = {0};

  Tester *testers = tester_array_base();
  U64 tester_count = tester_array_count();
  for(U64 tester_idx = 0; tester_idx < tester_count; ++tester_idx)
  {
    Tester *tester = testers + tester_idx;

    str8_list_push_f(arena, &log, "--- %.*s ---\n",
		     (int)tester->name.count, tester->name.string);

    B32 result = tester->fn(arena, &log);

    str8_list_push_f(arena, &log, "%s\n",
		     result ? "success" : "failure");
  }

  String8 log_str = str8_join(arena, &log, 0);
  Buffer log_buf = buf_from_str8(log_str);
  os_write_entire_file(log_buf, Str8Lit(DATA_DIR "/test/log.txt"));

  arena_release(arena);
}
