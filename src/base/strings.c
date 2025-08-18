proc U64
cstr_get_count(char *cstr)
{
  U64 count = 0;
  char *at = cstr;
  while(*at) {
    ++count;
    ++at;
  }
  
  return(count);
}

proc String8
str8_push_f(Arena *arena, char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  String8 string = str8_push_fv(arena, fmt, args);
  va_end(args);

  return(string);
}

proc String8
str8_push_fv(Arena *arena, char *fmt, va_list args)
{
  U64 buffer_count = 1024;
  U8 *buffer = arena_push_array(arena, U8, buffer_count);
  U64 count = vsnprintf((char *)buffer, buffer_count, fmt, args);
  if(count < buffer_count) {
    // NOTE: if success, pop off the unused amount
    arena_pop(arena, buffer_count - count - 1);
  }
  else {
    // NOTE: if failure, retry with real count
    arena_pop(arena, count);
    buffer = arena_push_array(arena, U8, count + 1);
    count = vsnprintf((char *)buffer, count + 1, fmt, args);    
  }

  String8 result = {.string = buffer, .count = count};
  return(result);
}

// NOTE: for situations where we need to push variable length strings onto a buffer, whose starting addresses require
//       a particular alignment, and each byte of this buffer will be read downstream by something we have no control
//       over (eg when we send certain requests to a wayland display server), we need to account for the padding
//       bytes in the allocation here, so that the later read will not trip the address sanitizer.
//       There is probably a more robust way of solving this problem, but this way is really easy.
proc String8
arena_push_str8_copy_ex(Arena *arena, String8 string, U64 next_alignment)
{
  String8 result = {0};
  U64 push_size = string.count + 1; // NOTE: null-termination
  if(next_alignment) {
    push_size = AlignPow2(push_size, next_alignment);
  }
  result.string = arena_push_array_z(arena, U8, push_size);
  result.count = string.count;
  CopySize(result.string, string.string, string.count);

  return(result);
}

proc B32
str8s_are_equal(String8 s1, String8 s2)
{
  B32 result = s1.count == s2.count;
  if(result) {
    U8 *at1 = s1.string;
    U8 *at2 = s2.string;
    for(U64 i = 0; i < s1.count; ++i, ++at1, ++at2) {
      if(*at1 != *at2) {
	result = 0;
	break;
      }
    }
  }

  return(result);
}

proc B32
str8_contains(String8 s1, String8 s2)
{
  B32 result = 0;
  
  U8 *at1 = s1.string;
  for(U64 i = 0; i < s1.count && !result; ++i, ++at1) {
    result = 1;
    for(U64 j = 0; j < s2.count && result; ++j) {
      result = at1[j] == s2.string[j];
    }
  }

  return(result);
}

proc String8
str8_concat(Arena *arena, String8 s1, String8 s2, String8 sep)
{
  String8 result = {0};
  result.count = s1.count + s2.count + sep.count;
  result.string = arena_push_array_z(arena, U8, result.count + 1);
  CopySize(result.string, s1.string, s1.count);
  CopySize(result.string + s1.count, sep.string, sep.count);
  CopySize(result.string + s1.count + sep.count, s2.string, s2.count);

  return(result);
}

proc void
str8_list_push_ex(String8List *list, String8 string, String8Node *node)
{
  node->string = string;
  SLLQueuePush(list->first, list->last, node);
  ++list->node_count;
  list->total_count += string.count;
}

proc void
str8_list_push(Arena *arena, String8List *list, String8 string)
{
  String8Node *node = arena_push_struct(arena, String8Node);
  str8_list_push_ex(list, string, node);
}

proc void
str8_list_push_f(Arena *arena, String8List *list, char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  //String8 string = str8_push_fv(arena, fmt, args);
  str8_list_push_fv(arena, list, fmt, args);
  va_end(args);

  //str8_list_push(arena, list, string);
}

proc void
str8_list_push_fv(Arena *arena, String8List *list, char *fmt, va_list args) {
  String8 string = str8_push_fv(arena, fmt, args);
  str8_list_push(arena, list, string);
}

proc String8
str8_join(Arena *arena, String8List *list)
{
  String8 result = {0};
  result.string = arena_push_array(arena, U8, list->total_count);
  result.count = list->total_count;

  U8 *string_at = result.string;
  for(String8Node *node = list->first; node; node = node->next) {
    U64 node_string_count = node->string.count;
    CopySize(string_at, node->string.string, node_string_count);
    string_at += node_string_count;
  }

  return(result);
}

proc String8List
str8_split(Arena *arena, String8 string, U8 *split_chars, U64 count)
{
  String8List result = {0};
  U8 *string_start, *string_at = string.string;
  U64 counter = 0;
  for(U64 i = 0; i < string.count; ++i) {    
    if(str8s_are_equal((String8){string_at, count},
		       (String8){split_chars, count})) {
      String8 new_string = {string_start, counter};
      str8_list_push(arena, &result, new_string);
      
      counter = 0;
      string_at += count;
      string_start = string_at;
      continue;
    }
    
    ++string_at;
    ++counter;
  }

  return(result);
}

proc String16
str16_from_str8(Arena *arena, String8 str8)
{  
  U64 cap = 2*str8.count;
  U16 *string = arena_push_array(arena, U16, cap + 1);
  U64 count = 0;
  {
    U8 *start = str8.string;
    U8 *opl = start + str8.count;
    U16 *dest = string;
    UnicodeDecode decode;    
    for(U8 *src = start; src < opl; src += decode.inc) {
      decode = utf8_decode(src, opl - src);
      count += utf16_encode(dest, decode.cp);
    }
  }
  string[count] = 0;
  arena_pop(arena, (cap - count)*2);
  
  String16 result = {0};
  result.string = string;
  result.count = count;
  return(result);
}

// NOTE: map the high nibble of a utf8 byte to its class.
//       classes 1 through 4 correspond to the length of the encoding, and
//       class 0 is a non-leading byte (invalid starting byte)
global U8 utf8_class[16] = {
  1, 1, 1, 1,
  1, 1, 1, 1,
  0, 0, 0, 0,
  2, 2, 3, 4,
};

proc UnicodeDecode
utf8_decode(U8 *str, U64 count)
{
  U32 inc = 1;
  U32 cp = U32_MAX;
  {
    U8 byte_1 = str[0];
    U8 top_4 = byte_1 >> 4;
    U8 class = utf8_class[top_4];
    switch(class) {
    case 1: {
      cp = byte_1 & 0x7F;
    } break;    
    case 2: {
      if(count > 1) {
	U8 byte_2 = str[1];
	Assert(utf8_class[byte_2 >> 4] == 0);
	cp = (((byte_1 & 0x1F) << 6) |
	      (byte_2 & 0x3F));
      }
    } break;
    case 3: {
      if(count > 2) {
	U8 byte_2 = str[1];
	U8 byte_3 = str[2];
	Assert(utf8_class[byte_2 >> 4] == 0);
	Assert(utf8_class[byte_3 >> 4] == 0);
	cp = (((byte_1 & 0xF) << 12) |
	      ((byte_2 & 0x3F) << 6) |
	      (byte_3 & 0x3F));
      }
    } break;
    case 4: {
      if(count > 3) {
	U8 byte_2 = str[1];
	U8 byte_3 = str[2];
	U8 byte_4 = str[3];
	Assert(utf8_class[byte_2 >> 4] == 0);      
	Assert(utf8_class[byte_3 >> 4] == 0);
	Assert(utf8_class[byte_4 >> 4] == 0);
	cp = (((byte_1 & 0x7) << 18) |
	      ((byte_2 & 0x3F) << 12) |
	      ((byte_3 & 0x3F) << 6) |
	      (byte_4 & 0x3F));
      }
    } break;
    default: {
      Assert(!"ERROR: decoding invalid utf8 byte");
    } break;
    }
    inc = class;
  }
  
  UnicodeDecode result = {0};
  result.inc = inc;
  result.cp = cp;
  return(result);
}

proc UnicodeDecode
utf16_decode(U16 *str, U64 count)
{
  U32 inc = 1;
  U32 cp = U32_MAX;
  {
    U16 word_1 = str[0];
    if(word_1 < 0xD800 || word_1 > 0xDFFF) {
      // NOTE: single word encoding
      cp = word_1;
    }else {
      if(count > 1) {
	U16 word_2 = str[1];
	Assert((word_1 >> 10) == 0b110110);
	Assert((word_2 >> 10) == 0b110111);
	cp = (((word_1 & 0x3FF) << 10) |
	      (word_2 & 0x3FF)) + 0x10000;
	inc = 2;
      }
    }
  }

  UnicodeDecode result = {0};
  result.inc = inc;
  result.cp = cp;
  return(result);
}

proc U32
utf8_encode(U8 *str, U32 cp)
{
  U32 count = 1;
  if(cp < 0x80) {
    str[0] = cp;
  }else
  if(cp < 0x800) {
    str[0] = (0x80 | (cp & 0x3F));
    str[1] = (0xC0 | ((cp >> 6) & 0x1F));
    count = 2;
  }else
  if(cp < 0x10000) {
    str[0] = (0x80 | (cp & 0x3F));
    str[1] = (0x80 | ((cp >> 6) & 0x3F));
    str[2] = (0xE0 | ((cp >> 12) & 0xF));
    count = 3;
  }else
  if(cp < 0x110000) {
    str[0] = (0x80 | (cp & 0x3F));
    str[1] = (0x80 | ((cp >> 6) & 0x3F));
    str[2] = (0x80 | ((cp >> 12) & 0x3F));    
    str[3] = (0xE0 | ((cp >> 18) & 0x7));
    count = 4;
  }else {
    str[0] = '?';
  }
  return(count);
}

proc U32
utf16_encode(U16 *str, U32 cp)
{
  U32 count = 1;
  if(cp < 0xD800 || (cp > 0xE000 && cp < 0x10000)) {
    str[0] = cp;
  }else
  if(cp >= 0x10000 && cp <= 0x10FFFF) {
    U16 u = cp - 0x10000;
    str[0] = 0xD800 + ((u >> 10) & 0x3F);
    str[1] = 0xDC00 + (u & 0x3F);
    count = 2;
  }else {
    str[0] = (U16)'?';
  }
  return(count);
}
