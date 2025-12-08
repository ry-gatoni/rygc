typedef struct ProfileEntry
{
  String8 label;
  U64 thread_id;

  U64 tsc_elapsed;
  U64 tsc_elapsed_children;
  U64 tsc_elapsed_root;

  U64 hit_count;

  // NOTE: internal
  U64 start_tsc;
  U64 tsc_elapsed_root__old;
  struct ProfileEntry *parent;
} ProfileEntry;

typedef struct Profiler
{
  Arena *arena;

  U64 thread_id;

  U64 profile_entry_count;
  ProfileEntry *profile_entries;

  ProfileEntry *current_parent;

  U64 start_tsc;
  U64 end_tsc;
  U64 counter_freq;
} Profiler;

thread_var Profiler *active_profiler = 0;

// -----------------------------------------------------------------------------
// allocate, release, select

proc Profiler* profile_alloc(void);
proc void profile_release(Profiler *profiler);
proc void profile_select(Profiler *profiler);

// -----------------------------------------------------------------------------
// toplevel

proc void profile_begin(void);
proc void profile_end(void);

// -----------------------------------------------------------------------------
// per-block

proc void profile_begin_block(ProfileEntry *entry);
proc void profile_end_block(ProfileEntry *entry);
#define ProfileScope(name)\
  ProfileDefine(name) = {.label = Str8Lit(#name)};\
  for(int Glue(_i_, __LINE__) = (profile_begin_block(ProfileData(name)), 0);\
      !Glue(_i_, __LINE__);\
      ++Glue(_i_, __LINE__), profile_end_block(ProfileData(name)))
#define ProfileFunction() ProfileScope(__func__)

// -----------------------------------------------------------------------------
// helpers

#define ProfileDeclare(name) Section("rygcPROF") extern ProfileEntry Glue(profile__, name)
//#define ProfileDefine(name) Glue(profile__, name) ProfileDeclare(name)
#define ProfileDefine(name) static Section("rygcPROF") ProfileEntry Glue(profile__, name)
#define ProfileData(name) (&Glue(profile__, name))

ProfileDefine(0) = {0};
global ProfileEntry *profile_entry_null = &profile__0;

// NOTE: only tested on linux/clang
extern ProfileEntry __start_rygcPROF[];
extern ProfileEntry __stop_rygcPROF[];
static inline ProfileEntry* profile_entry_array_base(void) { return(__start_rygcPROF); }
static inline U64 profile_entry_array_count(void) { return(__stop_rygcPROF - __start_rygcPROF); }

/* #define RYGC_SYMBOL_SET profile */
/* #define profile_Type ProfileEntry */
/* #define profile_section ".ryPROFL" */
/* #include "symbol_set.define.h" */
/* #define ProfileId(name) SymbolId(RYGC_SYMBOL_SET, name) */
/* #define ProfileRaw(name) SymbolRaw(RYGC_SYMBOL_SET, name) */
/* #define ProfileDecl(name) SymbolDecl(RYGC_SYMBOL_SET, name) */
/* #undef RYGC_SYMBOL_SET */
