#include <string.h>
#include <stdarg.h>
#include <stdio.h> // TODO: eliminate

#define KB(n) (1024ULL * n)
#define MB(n) (1024ULL * KB(n))
#define GB(n) (1024ULL * MB(n))
#define TB(n) (1024ULL * GB(n))

#define ArrayCount(arr) (sizeof((arr))/sizeof(*(arr)))
#define Unused(v) (void)(v)

#define Statement(expr) do {expr} while(0)
#define Glue(a, b) a##b

#if BUILD_DEBUG == 1
#  if COMPILER_CLANG || COMPILER_GCC
#    define Assert(cond) if(!(cond)) __builtin_trap()
#  elif COMPILER_MSVC
#    define Assert(cond) if(!(cond)) __debugbreak
#  else
#    define Assert(cond) if(!(cond)) *(void *)0
#  endif
#else
#  define Assert(cond)
#endif
// TODO: implement
#define StaticAssert(cond, var)

#define Max(a, b) (((a) < (b)) ? (b) : (a))
#define Min(a, b) (((a) > (b)) ? (b) : (a))

#define Align(n, a) (((n) + ((a) - 1)) - (((n) + ((a) - 1)) % (a)))
#define AlignPow2(n, a) (((n) + (a - 1)) & ~(a - 1))

// NOTE: linked-list utils
#define SLLStackPush_N(l, n, next) ((n)->next = (l), (l) = (n))
#define SLLStackPush(l, n) SLLStackPush_N(l, n, next)

#define SLLStackPop_N(l, next) ((l)=(l)->next)
#define SLLStackPop(l) SLLStackPop_N(l, next)

#define SLLQueuePush_N(f, l, n, next) ((((f)==0) ? ((f)=(l)=(n)) : ((l)->next=n, (l)=(n))), (n)->next=0)
#define SLLQueuePush(f, l, n) SLLQueuePush_N(f, l, n, next)

#define DLLPushBack_NP(f, l, n, next, prev) ((((f)==0) ? ((f)=(l)=(n), (n)->prev=0) : ((n)->prev=(l), (l)->next=(n), (l)=(n))), (n)->next=0)
#define DLLPushBack(f, l, n) DLLPushBack_NP(f, l, n, next, prev)

// NOTE: copying/zeroing utils
#define CopySize(dest, source, size) memcpy(dest, source, size)
#define CopyStruct(dest, source, type) memcpy(dest, source, sizeof(type))
#define CopyArray(dest, source, type, count) memcpy(dest, source, (count)*sizeof(type))
#define ZeroSize(dest, size) memset(dest, 0, size)
#define ZeroStruct(dest, type) memset(dest, 0, sizeof(type))
#define ZeroArray(dest, type, count) memset(dest, 0, (count)*sizeof(type))
