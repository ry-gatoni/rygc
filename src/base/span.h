typedef struct Span
{
  void *start;
  void *end;
} Span;

typedef struct SpanU8
{
  U8 *start;
  U8 *end;
} SpanU8;

typedef struct SpanU32
{
  U32 *start;
  U32 *end;
} SpanU32;

typedef struct SpanR32
{
  R32 *start;
  R32 *end;
} SpanR32;

#define span_count(span, type) span_byte_count(span)/sizeof(type)
proc inline U64
span_byte_count(SpanU8 span)
{
  U64 result = span.end - span.start;
  return result;
}
