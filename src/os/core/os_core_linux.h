#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>

#if CPU_X86 || CPU_X64
#  include <x86intrin.h>
#endif

typedef struct Linux_ThreadInfo Linux_ThreadInfo;
struct Linux_ThreadInfo
{
  Linux_ThreadInfo *next;
  
  Os_ThreadProc *procedure;
  void *data;
  
  pthread_t handle;
};

typedef struct Linux_State
{
  Arena *arena;

  Linux_ThreadInfo *first_thread_info;
  Linux_ThreadInfo *last_thread_info;
  U64 thread_info_count;
  Linux_ThreadInfo *thread_info_freelist;
} Linux_State;

global Linux_State *linux_state = 0;

proc B32 linux_init(void);
proc Linux_ThreadInfo *linux_alloc_thread_info(void);

proc void* linux_thread_entry_point(void *param);
