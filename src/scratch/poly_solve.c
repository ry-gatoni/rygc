#include "base/base.h"

#include "base/base.c"

typedef struct Polynomial
{
  U64 order;
  R64 *coeffs; // NOTE: order+1 coefficients, starting with highest order
} Polynomial;

typedef struct PolynomialTermLoose PolynomialTermLoose;
struct PolynomialTermLoose
{
  PolynomialTermLoose *next;
  R64 coeff;
  U64 power;
};

typedef struct PolynomialLoose
{
  PolynomialTermLoose *first;
  PolynomialTermLoose *last;
  U64 term_count;
  U64 max_power;
} PolynomialLoose;

proc U64
poly_parse_u64(Buffer *buf)
{
  U64 result = 0;
  while(buf_peek_number(buf))
  {
    result *= 10;
    result += *buf_consume_char(buf) - '0';
  }
  return(result);
}

proc R64
poly_parse_r64(Buffer *buf)
{
  U64 integer = 0;
  while(buf_peek_number(buf))
  {
    integer *= 10;
    integer += *buf_consume_char(buf) - '0';
  }
  R64 result = (R64)integer;

  if(buf_peek_char(buf, '.'))
  {
    buf_consume_char(buf);

    R64 exp = 0.1;
    while(buf_peek_number(buf))
    {
      result += (*buf_consume_char(buf) - '0')*exp;
      exp *= 0.1;
    }
  }

  return(result);
}

// NOTE: syntax is (\+|\-)?<coeff>?(z(^<power>)?)?
proc void
poly_parse_term(PolynomialTermLoose *term, Buffer *buf)
{
  buf_consume_whitespace(buf);

  R64 coeff = 1.0;
  if(buf_peek_char(buf, '+') || buf_peek_char(buf, '-'))
  {
    if(*buf_consume_char(buf) == '-') coeff = -1.0;
    buf_consume_whitespace(buf);

    if(!buf_peek_char(buf, 'z'))
    {
      coeff *= poly_parse_r64(buf);
    }
    Assert(coeff != 0);
  }
  else
  {
    if(!buf_peek_char(buf, 'z'))
    {
      coeff = poly_parse_r64(buf);
    }
  }

  U64 power = 0;
  if(buf_peek_char(buf, 'z'))
  {
    buf_consume_char(buf);
    power = 1;

    if(buf_peek_char(buf, '^'))
    {
      buf_consume_char(buf);
      power = poly_parse_u64(buf);
    }
  }

  term->coeff = coeff;
  term->power = power;
}

proc PolynomialLoose
poly_loose_from_string(Arena *arena, String8 str)
{
  PolynomialLoose result = {0};
  Buffer buf = buf_from_str8(str);
  while(buf.size)
  {
    PolynomialTermLoose *term = arena_push_struct(arena, PolynomialTermLoose);
    poly_parse_term(term, &buf);
    SLLQueuePush(result.first, result.last, term);
    ++result.term_count;
    result.max_power = Max(result.max_power, term->power);
  }
  return(result);
}

proc Polynomial*
poly_from_string(Arena *arena, String8 str)
{
  Polynomial *result = arena_push_struct(arena, Polynomial);

  ArenaTemp scratch = arena_get_scratch(&arena, 1);

  PolynomialLoose loose_poly = poly_loose_from_string(scratch.arena, str);
  U64 order = loose_poly.max_power;
  R64 *coeffs = arena_push_array_z(arena, R64, order + 1);
  for(PolynomialTermLoose *term = loose_poly.first; term; term = term->next)
  {
    U64 coeff_idx = order - term->power;
    coeffs[coeff_idx] = term->coeff;
  }

  arena_release_scratch(scratch);

  result->order = order;
  result->coeffs = coeffs;
  return(result);
}

proc C64
poly_evaluate_at(Polynomial *p, C64 v)
{
  U64 order = p->order;
  R64 *coeffs = p->coeffs;

  C64 result = c64(coeffs[0], 0);
  for(U64 i = 0; i < order; ++i)
  {
    C64 coeff = c64(coeffs[i+1], 0);
    result = c64_add(coeff, c64_mul(v, result));
  }

  return(result);
}

typedef struct PolynomialRoot PolynomialRoot;
struct PolynomialRoot
{
  PolynomialRoot *next;
  C64 value;
  U64 order;
};

typedef struct PolynomialSolutions
{
  PolynomialRoot *first;
  PolynomialRoot *last;
  U64 count;
} PolynomialSolutions;


int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  Arena *arena = arena_alloc();
  Polynomial *poly = poly_from_string(arena, Str8Lit("1.5 - 0.2z - z^2"));
  for(U64 i = 0; i <= poly->order; ++i)
  {
    printf("[%lu]: %.2f\n", poly->order - i, poly->coeffs[i]);
  }

  return(0);
}
