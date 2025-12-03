typedef struct ProfileEntry
{
  String8 label;

  U64 tsc_elapsed;
  U64 tsc_elapsed_children;
  U64 tsc_elapsed_root;

  U64 hit_count;
} ProfileEntry;

typedef struct Profiler
{
  U64 profile_entry_count;
  ProfileEntry *profile_entries;

  U64 start_cylcles;
  U64 end_cycles;
  U64 counter_freq;
} Profiler;

#define RYGC_SYMBOL_SET profile
#define profile_Type ProfileEntry
#define profile_section ".ryPROFL"
#include "symbol_set.define.h"
#define ProfileId(name) SymbolId(RYGC_SYMBOL_SET, name)
#define ProfileRaw(name) SymbolRaw(RYGC_SYMBOL_SET, name)
#define ProfileDecl(name) SymbolDecl(RYGC_SYMBOL_SET, name)
#undef RYGC_SYMBOL_SET
