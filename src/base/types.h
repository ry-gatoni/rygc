#include <stdint.h>

typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef int8_t   S8;
typedef int16_t  S16;
typedef int32_t  S32;
typedef int64_t  S64;

typedef S8       B8;
typedef S16      B16;
typedef S32      B32;
typedef S64      B64;

typedef float    R32;
typedef double   R64;

#define U8_MAX  (0xFF)
#define U16_MAX (0xFFFF)
#define U32_MAX (0xFFFFFFFF)
#define U64_MAX (0xFFFFFFFFFFFFFFFF)

#define S8_MAX  (0x7F)
#define S16_MAX (0x7FFF)
#define S32_MAX (0x7FFFFFFF)
#define S64_MAX (0x7FFFFFFFFFFFFFFF)

#define proc static
#define local static
#define global static

#if COMPILER_CLANG || COMPILER_GCC
#  define thread_local __thread
#elif COMPILER_MSVC
#  define thread_local __declspec(thread)
#else
#  error ERROR: unsupported compiler
#endif
