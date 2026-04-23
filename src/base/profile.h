#if 1
typedef struct ProfileSite ProfileSite;
typedef struct ProfileEvent ProfileEvent;

struct ProfileSite
{
  // NOTE: profile data for a particular location in the program. is referenced
  // by and stores data for runtime `ProfileEvent`s. These are stored
  // contiguously in a special section whose address is obtained at load time.
  String8 label;

  // TODO: this only supports a site only being executed on a single thread. we
  // should support mutlithreaded profiling
  U64 hit_count;
  U64 bytes_allocated;

  U64 tsc_elapsed; // cumulative elapsed time stamp count accross all calls
  U64 tsc_elapsed_children; // cumulative elapsed time stamp count of all children accross all calls
  U64 tsc_elapsed_root; // elapsed time stamp count with this site as root
};

struct ProfileEvent
{
  // NOTE: runtime profile data for a `ProfileSite` location. These are
  // allocated on the stack at runtime.
  ProfileEvent *parent;

  ProfileSite *site;

  U64 tsc_start; // counter when scope entered
  U64 tsc_elapsed_root_old; // parent site's root counter when scope entered
};

global U64 profile_site_count = 0;
global ProfileSite *profile_sites = 0;
// TODO: once we support multithreaded profiling, these should become
// thread-local (or go into thread-local context)
global ProfileSite *profile__current_parent;
global ProfileEvent *profile__open_scope;

// -----------------------------------------------------------------------------
// public interface

#if !defined(PROFILE_DISABLE)

#define ProfileFunction() ProfileScope(__func__)
#define ProfileScope(name)\
  ProfileData(name);\
  for(U32 Glue(_i_, __LINE__) = (profile_begin_scope(), 0); \
      Glue(_i_, __LINE__) < 1;\
      ++Glue(_i_, __LINE__), profile_end_scope())
#define ProfileData(name)\
    PROFILE_SITE_DEFINE = { .label = Str8Lit(name), };\
    profile__current_parent = &PROFILE_SITE_NAME;\
    PROFILE_EVENT_DEFINE = { .parent = profile__open_scope, .site = profile__current_parent, };\
    profile__open_scope = &PROFILE_EVENT_NAME;

#define PROFILE_SITE_DEFINE static Section("rygcPROF") ProfileSite PROFILE_SITE_NAME
#define PROFILE_EVENT_DEFINE ProfileEvent PROFILE_EVENT_NAME
#define PROFILE_SITE_NAME Glue(__profile_site_, __LINE__)
#define PROFILE_EVENT_NAME Glue(__profile_event_, __LINE__)

#else

#define ProfileFunction()
#define ProfileScope(name)
#define ProfileData(name)

#endif

// NOTE: clang/linux only
// TODO: support on other platforms
extern ProfileSite __start_rygcPROF[];
extern ProfileSite __stop_rygcPROF[];
proc inline ProfileSite* profile_site_array_base(void) { return(__start_rygcPROF); }
proc inline U64 profile_site_array_count(void) { return(__stop_rygcPROF - __start_rygcPROF); }

proc void profile_begin_scope(void);
proc void profile_end_scope(void);

#else
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
#endif
