#include "base.os.posix.c"

// -----------------------------------------------------------------------------
// initialization

proc B32
mac_init(void)
{
  B32 result = 1;
  Arena *arena = arena_alloc();
  mac_state = arena_push_struct(arena, Mac_State);
  mac_state->arena = arena;

  // TODO: how to generate all this?
  mac_state->class_count = MacClass_Count;
  mac_state->classes = arena_push_array(arena, Class, mac_state->class_count);
  mac_state->classes[MacClass_NSObject] = objc_getClass("NSObject");
  mac_state->classes[MacClass_NSApplication] = objc_getClass("NSApplication");
  mac_state->classes[MacClass_NSWindow] = objc_getClass("NSWindow");
  mac_state->classes[MacClass_NSWindowDelegate] = objc_getClass("NSWindowDelegate");
  mac_state->classes[MacClass_NSEvent] = objc_getClass("NSEvent");
  mac_state->classes[MacClass_NSDate] = objc_getClass("NSDate");
  mac_state->classes[MacClass_NSString] = objc_getClass("NSString");

  mac_state->sel_count = MacSelector_Count;
  mac_state->sels = arena_push_array(arena, SEL, mac_state->sel_count);

  // universal
  mac_state->sels[MacSelector_alloc] = sel_registerName("alloc");
  mac_state->sels[MacSelector_release] = sel_registerName("release");
  mac_state->sels[MacSelector_init] = sel_registerName("init");

  // NSApplication
  mac_state->sels[MacSelector_sharedApplication] = sel_registerName("sharedApplication");
  mac_state->sels[MacSelector_setActivationPolicy] = sel_registerName("setActivationPolicy:");
  mac_state->sels[MacSelector_activate] = sel_registerName("activate");
  mac_state->sels[MacSelector_finishLaunching] = sel_registerName("finishLaunching");
  mac_state->sels[MacSelector_nextEventMatchingMask] = sel_registerName("nextEventMatchingMask:untilDate:inMode:dequeue:");
  mac_state->sels[MacSelector_sendEvent] = sel_registerName("sendEvent:");
  mac_state->sels[MacSelector_updateWindows] = sel_registerName("updateWindows");

  // NSWindow
  mac_state->sels[MacSelector_initWithContentRect] = sel_registerName("initWithContentRect:styleMask:backing:defer:");
  mac_state->sels[MacSelector_makeKeyAndOrderFront] = sel_registerName("makeKeyAndOrderFront:");
  mac_state->sels[MacSelector_setIsVisible] = sel_registerName("setIsVisible:");

  // NSWindowDelegate
  mac_state->sels[MacSelector_windowShouldClose] = sel_registerName("windowShouldClose:");

  // NSEvent
  mac_state->sels[MacSelector_type]                             = sel_registerName("type");
  mac_state->sels[MacSelector_subtype]                          = sel_registerName("subtype");
  mac_state->sels[MacSelector_locationInWindow]                 = sel_registerName("locationInWindow");
  mac_state->sels[MacSelector_timestamp]                        = sel_registerName("timestamp");
  mac_state->sels[MacSelector_window]                           = sel_registerName("window");
  mac_state->sels[MacSelector_windowNumber]                     = sel_registerName("windowNumber");
  mac_state->sels[MacSelector_modifierFlags]                    = sel_registerName("modifierFlags");
  mac_state->sels[MacSelector_keyCode]                          = sel_registerName("keyCode");
  mac_state->sels[MacSelector_keyRepeatDelay]                   = sel_registerName("keyRepeatDelay");
  mac_state->sels[MacSelector_keyRepeatInterval]                = sel_registerName("keyRepeatInterval");
  mac_state->sels[MacSelector_pressedMouseButtons]              = sel_registerName("pressedMouseButtons");
  mac_state->sels[MacSelector_doubleClickInterval]              = sel_registerName("doubleClickInterval");
  mac_state->sels[MacSelector_mouseLocation]                    = sel_registerName("mouseLocation");
  mac_state->sels[MacSelector_buttonNumber]                     = sel_registerName("buttonNumber");
  mac_state->sels[MacSelector_clickCount]                       = sel_registerName("clickCount");
  mac_state->sels[MacSelector_deltaX]                           = sel_registerName("deltaX");
  mac_state->sels[MacSelector_deltaY]                           = sel_registerName("deltaY");
  mac_state->sels[MacSelector_deltaZ]                           = sel_registerName("deltaZ");
  mac_state->sels[MacSelector_hasPreciseScrollingDeltas]        = sel_registerName("hasPreciseScrollingDeltas");
  mac_state->sels[MacSelector_scrollingDeltaX]                  = sel_registerName("scrollingDeltaX");
  mac_state->sels[MacSelector_scrollingDeltaY]                  = sel_registerName("scrollingDeltaY");
  mac_state->sels[MacSelector_momentumPhase]                    = sel_registerName("momentumPhase");
  mac_state->sels[MacSelector_isDirectionInvertedFromDevice]    = sel_registerName("isDirectionInvertedFromDevice");

  // NSString
  mac_state->sels[MacSelector_stringWithUTF8String] = sel_registerName("stringWithUTF8String:");

  mac_state->cpu_counter_freq = cpu_counter_fixed_freq();
  mac_state->page_size = posix_page_size();

  posix_init(arena);
  return(result);
}

proc B32
os_init(void)
{
  B32 result = mac_init();
  return(result);
}

// -----------------------------------------------------------------------------
// system info

proc U64
os_page_size(void)
{
  U64 result = mac_state->page_size;
  return(result);
}

// -----------------------------------------------------------------------------
// memory

proc void*
os_mem_reserve(U64 size)
{
  void *result = posix_mem_reserve(size);
  return(result);
}

proc B32
os_mem_commit(void *mem, U64 size)
{
  B32 result = posix_mem_commit(mem, size);
  return(result);
}

proc void
os_mem_decommit(void *mem, U64 size)
{
  posix_mem_decommit(mem, size);
}

proc void
os_mem_release(void *mem, U64 size)
{
  posix_mem_release(mem, size);
}

// -----------------------------------------------------------------------------
// files

proc Os_Handle
os_file_open(String8 path, Os_FileOpenFlags flags)
{
  Os_Handle result = posix_file_open(path, flags);
  return(result);
}

proc Os_FileAttributes
os_file_attributes(Os_Handle file)
{
  Os_FileAttributes result = posix_file_attributes(file);
  return(result);
}

proc Buffer
os_file_read(Arena *arena, Os_Handle file, U64 size)
{
  Buffer result = posix_file_read(arena, file, size);
  return(result);
}

proc Buffer
os_file_read_from(Arena *arena, Os_Handle file, U64 pos, U64 size)
{
  Buffer result = posix_file_read_from(arena, file, pos, size);
  return(result);
}

proc B32
os_file_write(Buffer file_contents, Os_Handle file)
{
  B32 result = posix_file_write(file_contents, file);
  return(result);
}

proc B32
os_file_write_to(Buffer file_contents, U64 pos, Os_Handle file)
{
  B32 result = posix_file_write_to(file_contents, pos, file);
  return(result);
}

proc Buffer
os_read_entire_file(Arena *arena, String8 path)
{
  Os_Handle file = os_file_open(path, Os_FileOpenFlag_read);
  Os_FileAttributes attr = os_file_attributes(file);
  Buffer result = os_file_read(arena, file, attr.size);

  return(result);
}

proc B32
os_write_entire_file(Buffer file_contents, String8 path)
{
  Os_Handle file = os_file_open(path, Os_FileOpenFlag_write);
  B32 result = os_file_write(file_contents, file);

  return(result);
}

// -----------------------------------------------------------------------------
// threads

proc Os_Handle
os_thread_launch(Os_ThreadProc *procedure, void *data)
{
  Os_Handle result = posix_thread_launch(procedure, data);
  return(result);
}

proc U64
os_thread_id(void)
{
  U64 result = posix_thread_id();
  return(result);
}

// -----------------------------------------------------------------------------
// libraries

proc Os_Handle
os_lib_open(String8 path)
{
  Os_Handle result = posix_lib_open(path);
  return(result);
}

proc void
os_lib_close(Os_Handle lib)
{
  posix_lib_close(lib);
}

proc void*
os_lib_get_sym(Os_Handle lib, String8 sym_name)
{
  void *result = posix_lib_get_sym(lib, sym_name);
  return(result);
}
