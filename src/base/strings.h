typedef struct String8 {
  U8 *string;
  U64 count;
} String8;

proc U64 cstr_get_count(char *cstr);

#define Str8Lit(lit) (String8){.string = (U8 *)lit, .count = (ArrayCount(lit) - 1)}
#define Str8Cstr(cstr) (String8){.string = (U8 *)cstr, .count = cstr_get_count(cstr)}

typedef struct String16 {
  U16 *string;
  U64 count;
} String16;

typedef struct String32 {
  U32 *string;
  U64 count;
} String32;

typedef struct String8Node String8Node;
struct String8Node
{
  String8Node *next;
  String8 string;
};

typedef struct String8List
{
  String8Node *first;
  String8Node *last;
  U64 node_count;
  U64 total_count;
} String8List;

typedef struct String8Array
{
  String8 *strings;
  U64 count;
  U64 total_size;
} String8Array;

proc String8 str8(U8 *chars, U64 count);
proc String8 str8_range(U8 *first, U8 *opl);

proc String8 str8_push_f(Arena *arena, char *fmt, ...);
proc String8 str8_push_fv(Arena *arena, char *fmt, va_list args);
// TODO: we can get rid of this hacky thing with the next alignment now.
//       wayland message alignment is no longer coupled to arena memory unpoisoning
proc String8 arena_push_str8_copy_ex(Arena *arena, String8 string, U64 next_alignment);
#define arena_push_str8_copy(arena, string) arena_push_str8_copy_ex(arena, string, 0)

proc B32 str8s_are_equal(String8 s1, String8 s2);
proc B32 str8_contains(String8 s1, String8 s2);

proc String8 str8_concat(Arena *arena, String8 s1, String8 s2, String8 sep);

proc void str8_list_push_ex(String8List *list, String8 string, String8Node *node);
proc void str8_list_push(Arena *arena, String8List *list, String8 string);
proc void str8_list_push_f(Arena *arena, String8List *list, char *fmt, ...);
proc void str8_list_push_fv(Arena *arena, String8List *list, char *fmt, va_list args);

proc String8 str8_join(Arena *arena, String8List *list);
proc String8List str8_split(Arena *arena, String8 string, U8 *split_chars, U64 count);

proc String8Array str8_array_from_list(Arena *arena, String8List *list);

proc String16 str16_from_str8(Arena *arena, String8 str8);

typedef struct String16Node String16Node;
struct String16Node
{
  String16Node *next;
  String16 string;
};

typedef struct String16List
{
  String16Node *first;
  String16Node *last;
  U64 node_count;
  U64 total_count;
} String16List;

typedef struct String32Node String32Node;
struct String32Node
{
  String32Node *next;
  String32 string;
};

typedef struct String32List
{
  String32Node *first;
  String32Node *last;
  U64 node_count;
  U64 total_count;
} String32List;

// NOTE: unicode
typedef struct UnicodeDecode
{
  U32 inc;
  U32 cp;
} UnicodeDecode;

proc UnicodeDecode utf8_decode(U8 *str, U64 count);
proc UnicodeDecode utf16_decode(U16 *str, U64 count);

proc U32 utf8_encode(U8 *str, U32 cp);
proc U32 utf16_encode(U16 *str, U32 cp);

