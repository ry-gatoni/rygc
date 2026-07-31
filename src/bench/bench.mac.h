// NOTE: see
// https://gist.github.com/ibireme/173517c208c7dc333ba962c1f0d67d12
// https://gist.github.com/mmozeiko/bd5923bcd9d20b5b9946691932ec95fa
// https://github.com/lemire/Code-used-on-Daniel-Lemire-s-blog/tree/master/2021/03/24

typedef struct kpep_db kpep_db;
typedef struct kpep_event kpep_event;
typedef struct kpep_config kpep_config;
typedef U64 kpc_config_t;

#define KPC_MAX_COUNTERS 32
#define KPC_CLASS_CONFIGURABLE (1)
#define KPC_CLASS_CONFIGURABLE_MASK (1U << KPC_CLASS_CONFIGURABLE)

typedef struct Bench_MacState
{
  kpep_db *kpepdb;
  kpep_config *kpepcfg;

  U32 counter_classes;
  size_t counter_reg_count;
  size_t counter_map[KPC_MAX_COUNTERS];
  kpc_config_t counter_regs[KPC_MAX_COUNTERS];

  Bench_CounterResult *result_buf;
} Bench_MacState;

global Bench_MacState *bench_mac_state = 0;

global const char *mac_bench_event_names[][3] = {
  [Bench_Counter_cpu_cycles]          = { "FIXED_CYCLES",           "CPU_CLK_UNHALTED.THREAD",      0 },
  [Bench_Counter_instructions]        = { "FIXED_INSTRUCTIONS",     "INST_RETIRED.ANY",             0 },
  [Bench_Counter_branch_misses]       = { "BRANCH_MISPRED_NONSPEC", "BRANCH_MISPREDICT",            "BR_MISP_RETIRED.ALL_BRANCHES" },
  [Bench_Counter_branch_instructions] = { "INST_BRANCH",            "BR_INST_RETIRED.ALL_BRANCHES", 0 },
};

#define KPERF_PROC_XLIST\
  X(int, kpc_force_all_ctrs_set,  (int value))\
  X(int, kpc_set_config,          (U32 classes, kpc_config_t *cfg))\
  X(int, kpc_set_counting,        (U32 classes))\
  X(int, kpc_set_thread_counting, (U32 classes))\
  X(int, kpc_get_thread_counters, (U32 thread_id, U32 buf_count, void *buf))\
  X(U32, kpc_get_counter_count,   (U32 classes))\

#define KPERFDATA_PROC_XLIST\
  X(int, kpep_db_create,             (const char *name, kpep_db **db))\
  X(int, kpep_db_events_count,       (kpep_db *db, size_t *count))\
  X(int, kpep_db_events,             (kpep_db *db, kpep_event **buf, size_t buf_size))\
  X(int, kpep_db_event,              (kpep_db *db, const char *name, kpep_event **event))\
  X(int, kpep_db_free,               (kpep_db *db))\
  X(int, kpep_event_name,            (kpep_event *event, const char **name))\
  X(int, kpep_event_description,     (kpep_event *event, const char **desc))\
  X(int, kpep_config_create,         (kpep_db *db, kpep_config **cfg))\
  X(int, kpep_config_force_counters, (kpep_config *cfg))\
  X(int, kpep_config_add_event,      (kpep_config *cfg, kpep_event **event, U32 flag, U32 *err))\
  X(int, kpep_config_kpc_classes,    (kpep_config *cfg, U32 *classes))\
  X(int, kpep_config_kpc_count,      (kpep_config *cfg, size_t *count))\
  X(int, kpep_config_kpc_map,        (kpep_config *cfg, void *buf, size_t buf_size))\
  X(int, kpep_config_kpc,            (kpep_config *cfg, kpc_config_t *buf, size_t buf_size))\
  X(int, kpep_config_free,           (kpep_config *cfg))\

#define X(ret, name, args) global ret (*name)args;
KPERF_PROC_XLIST;
KPERFDATA_PROC_XLIST;
#undef X

proc B32 bench_init__mac(void);
proc B32 bench_use_counters__mac(void);
proc Bench_Result bench_run_proc__mac(Bench_Proc *p, void *args);
