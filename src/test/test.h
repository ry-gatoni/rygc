#define TEST_FN(name) B32 (name)(Arena *arena, String8List *log)
typedef TEST_FN(TestProc);

typedef struct Tester
{
  String8 name;
  TestProc *fn;
} Tester;

#undef SYMBOL_SET_DEFINE
#define SYMBOL_SET_DEFINE TST
#define TEST_Sym_Set TST
#define TEST_Sym_Type Tester
#define TEST_Sym_Marker Glue(rygc, TEST_Sym_Set)
#define TEST_Sym_First Glue(__start_, TEST_Sym_Marker)
#define TEST_Sym_Last Glue(__stop_, TEST_Sym_Marker)
#if COMPILER_MSVC
#  pragma section("." Stringify(TEST_Sym_Marker) "$a", read, write)
#  pragma section("." Stringify(TEST_Sym_Marker) "$i", read, write)
#  pragma section("." Stringify(TEST_Sym_Marker) "$z", read, write)
#  define TEST_Sym_Section "." Stringify(TEST_Sym_Marker) "$i"
global Section("." Stringify(TEST_Sym_Marker) "$a") TEST_Sym_First;
global Section("." Stringify(TEST_Sym_Marker) "$z") TEST_Sym_Last;
#elif COMPILER_CLANG || COMPILER_GCC
#  define TEST_Sym_Section Stringify(TEST_Sym_Marker)
#  if OS_MAC
#    undef nil // TODO: don't include all of objc.h
extern TEST_Sym_Type TEST_Sym_First[] asm("section$start$__DATA$" Stringify(TEST_Sym_Marker));
extern TEST_Sym_Type TEST_Sym_Last[] asm("section$end$__DATA$" Stringify(TEST_Sym_Marker));
#  elif OS_LINUX
extern TEST_Sym_Type TEST_Sym_First[];
extern TEST_Sym_Type TEST_Sym_Last[];
#  else
#    error unsupported os
#  endif
#else
#  error unsupported compiler
#endif

proc inline Tester*
tester_array_base(void)
{ return TEST_Sym_First + 1; }

proc inline U64
tester_array_count(void)
{ return (U64)(TEST_Sym_Last - TEST_Sym_First - 1); }

#define TEST_FN_DEF(n)\
  TEST_FN(Glue(testfn__, n));\
  Section(TEST_Sym_Section) TEST_Sym_Type Glue(tester__, n) = (TEST_Sym_Type){ .name = Str8Lit(Stringify(n)), .fn = Glue(testfn__, n), }; \
  TEST_FN(Glue(testfn__, n))

TEST_FN_DEF(nil)
{
  Unused(arena);
  Unused(log);
  return 0;
}

#include "test/test.c"
