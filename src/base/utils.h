#include <string.h>
#include <stdarg.h>
#include <stdio.h> // TODO: eliminate

// -----------------------------------------------------------------------------
// units

#define KB(n) (1024ULL * n)
#define MB(n) (1024ULL * KB(n))
#define GB(n) (1024ULL * MB(n))
#define TB(n) (1024ULL * GB(n))

#define Thousand(n) (1000ULL * n)
#define Million(n)  (1000ULL * Thousand(n))
#define Billion(n)  (1000ULL * Million(n))
#define Trillion(n) (1000ULL * Billion(n))

#define ArrayCount(arr) (sizeof((arr))/sizeof(*(arr)))
#define Unused(v) (void)(v)

// -----------------------------------------------------------------------------
// macro helpers

#define Statement(expr) do {expr} while(0)

#define Stringify_(a) #a
#define Stringify(a) Stringify_(a)

#define Glue_(a, b) a##b
#define Glue(a, b) Glue_(a, b)

// -----------------------------------------------------------------------------
// object section names

#if COMPILER_CLANG || COMPILER_GCC
#  define Section(name) __attribute__((__section__(name)))
#elif COMPILER_MSVC
#  define Section(name) __declspec(allocate(name))
#else
#  error ERROR: `Section` not defined for this compiler
#endif

// -----------------------------------------------------------------------------
// before main

#if OS_LINUX
#  define BeforeMain__Named(name)\
  __attribute__((constructor)) static void name(void)
#elif OS_WINDOWS
#  define BeforeMain__Named(name)\
  static void name(void);\
  Section(".CRT$XCU")\
  __pragma(comment(linker, "/include:"#name"__"))\
  CLinkage void (*Glue(name, __))(void) = name;  \
  static void name(void)
#else
#  error ERROR: BeforeMain not defined for this compiler
#endif

#define BeforeMain_(name) BeforeMain__Named(name)
#define BeforeMain() BeforeMain_(Glue(beforemain__, __COUNTER__))

// -----------------------------------------------------------------------------
// struct field alignment

#if COMPILER_CLANG || COMPILER_GCC
#  define AlignField(n) __attribute((aligned(n)))
#elif COMPILER_MSVC
#  define AlignField(n) __declspec(align(n))
#else
#  warning WARNING: `AlignField()` macro not supported on this compiler
#endif

// -----------------------------------------------------------------------------
// asserts

#if COMPILER_CLANG || COMPILER_GCC
#  define AssertAlways(cond) if(!(cond)) __builtin_trap()
#elif COMPILER_MSVC
#  define AssertAlways(cond) if(!(cond)) __debugbreak()
#else
#  warning "WARNING: unsupported compiler"
#  define AssertAlways(cond) if(!(cond)) *(void*)0
#endif

#if BUILD_DEBUG == 1
#  define Assert(cond) AssertAlways(cond)

#  include <sanitizer/asan_interface.h>
#  define AsanPoison(addr, size)   __asan_poison_memory_region((addr), (size))
#  define AsanUnpoison(addr, size) __asan_unpoison_memory_region((addr), (size))
#else
#  define Assert(cond)
#  define AsanPoison(addr, size)
#  define AsanUnpoison(addr, size)
#endif

#define StaticAssert(cond, var) typedef U8 Glue(var, __LINE__) [(cond)?1:-1]

#define Unreachable() Assert(!"Unreachable")
#define UnreachableDefault() default: { Unreachable(); } break;

#define Max(a, b) (((a) < (b)) ? (b) : (a))
#define Min(a, b) (((a) > (b)) ? (b) : (a))

#define ClampToRange(n, min, max) Min(Max(n, min), max)

#define Align(n, a) (((n) + ((a) - 1)) - (((n) + ((a) - 1)) % (a)))
#define AlignPow2(n, a) (((n) + ((a) - 1)) & ~((a) - 1))
#define TruncPow2(n, a) ((n) & ~((a) - 1))
#define RoundUpPow2(n) (1ULL << (MSB(n)))

#define IntFromPtr(p) (U64)((U8*)(p))
#define PtrFromInt(n) (void*)((uintptr_t)(n))//(void *)((U8 *)0 + (n))

#define OffsetOf(s, m) (IntFromPtr(&((s*)0)->m))

#define FOURCC(str) ((U32)((str[0]<<0)|(str[1]<<8)|(str[2]<<16)|(str[3]<<24)))

#define FloorU32(f) ((U32)(f))
#define RoundU32(f) FloorU32(f + 0.5f)

// -----------------------------------------------------------------------------
// linked list utils

#define SLLStackPush_N(l, n, next) ((n)->next = (l), (l) = (n))
#define SLLStackPush(l, n) SLLStackPush_N(l, n, next)

#define SLLStackPop_N(l, next) ((l)=(l)->next)
#define SLLStackPop(l) SLLStackPop_N(l, next)

#define SLLQueuePush_N(f, l, n, next)\
  ((((f)==0) ? ((f)=(l)=(n)) : ((l)->next=(n), (l)=(n))), (n)->next=0)
#define SLLQueuePush(f, l, n) SLLQueuePush_N(f, l, n, next)

#define SLLQueuePushFront_N(f, l, n, next)\
  (((f)==0) ? ((f)=(l)=(n), (n)->next=0) : ((n)->next=(f), (f)=(n)))
#define SLLQueuePushFront(f, l, n) SLLQueuePushFront_N(f, l, n, next)

#define SLLQueuePop_N(f, l, next) (((f)==(l)) ? ((f)=(l)=0) : ((f)=(f)->next))
#define SLLQueuePop(f, l) SLLQueuePop_N(f, l, next)

#define DLLPushBack_NP(f, l, n, next, prev) ((((f)==0) ? ((f)=(l)=(n), (n)->prev=0) : ((n)->prev=(l), (l)->next=(n), (l)=(n))), (n)->next=0)
#define DLLPushBack(f, l, n) DLLPushBack_NP(f, l, n, next, prev)

#define DLLRemove_NP(f, l, n, next, prev)\
  ((((n)==(f)) ? (f)=(n)->next : (0)),                          \
   (((n)==(l)) ? (l)=(n)->prev : (0)),                          \
   (((n)->prev==0) ? (0) : ((n)->prev->next=(n)->next)),                \
   (((n)->next==0) ? (0) : ((n)->next->prev=(n)->prev)))
#define DLLRemove(f, l, n) DLLRemove_NP(f, l, n, next, prev)

// -----------------------------------------------------------------------------
// memory copying/zeroing/setting

#define CopySize(dest, source, size) memcpy(dest, source, size)
#define CopyStruct(dest, source) memcpy(dest, source, sizeof(*(src)))
#define CopyArray(dest, source, type, count) memcpy(dest, source, (count)*sizeof(type))
#define ZeroSize(dest, size) memset(dest, 0, size)
#define ZeroStruct(dest) memset(dest, 0, sizeof(*(dest)))
#define ZeroArray(dest, type, count) memset(dest, 0, (count)*sizeof(type))

#define SetSize(dest, val, size) memset(dest, val, size);
#define SetArray(dest, val, type, count) SetSize(dest, val, (count)*sizeof(type))
