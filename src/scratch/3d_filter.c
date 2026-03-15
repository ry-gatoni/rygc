#include "base/base.h"
#include "gfx/gfx.h"
#include "render/render.h"
/* #include "font/font.h" */
/* #include "OpenGL/ogl.h" */

// TODO: sort out dependencies. It should be possible to open a window without
// providing a renderer or graphics api. It should be possible to use a renderer
// without providing a font loader.

#include "base/base.c"
#include "gfx/gfx.c"
#include "render/render.c"
/* #include "font/font.c" */
/* #include "OpenGL/ogl.c" */

// NOTE: shitty float from string parser
proc R64
r64_from_str8(String8 str)
{
  R64 result = 0.0;
  B32 parsing_num = 1;
  U32 dec_places = 0;

  U8 *at = str.string;
  U8 *opl = at + str.count;
  for(; at < opl; ++at)
  {
    if(*at == '-' && at == str.string)
    {
      result *= -1;
    }

    if(*at == '.')
    {
      parsing_num = 0;
      continue;
    }

    if(!is_number(*at))
    {
      break;
    }

    if(parsing_num)
    {
      result *= 10.0;
      result += (R64)(*at - '0');
    }
    else
    {
      ++dec_places;
      R64 dec_inc = (R64)(*at - '0');
      for(U32 dec_idx = 0; dec_idx < dec_places; ++dec_idx)
      {
	dec_inc *= 0.1;
      }
      result += dec_inc;
    }
  }

  return(result);
}

typedef struct Filter
{
  U32 zero_order;
  U32 pole_order;
  R64 *zero_coeffs;
  R64 *pole_coeffs;
} Filter;

typedef struct FilterTermLoose FilterTermLoose;
struct FilterTermLoose
{
  FilterTermLoose *next;
  R64 coeff;
  S64 power;
};

typedef struct LooseFilterTermList
{
  FilterTermLoose *first;
  FilterTermLoose *last;
  U64 term_count;
  S64 max_power;
  S64 min_power;
} LooseFilterTermList;

proc LooseFilterTermList
filter_parse_string(Arena *arena, String8 poly)
{
  LooseFilterTermList list = {0};

  ArenaTemp scratch = arena_get_scratch(&arena, 1);

  Buffer pm_idxs = str8_indices_of_chars_in_string(scratch.arena, poly, Str8Lit("+-"));
  U64 *inc_ptr = buf_consume_struct(&pm_idxs, U64);
  U8 *start = poly.string;
  U8 *end = start + poly.count;
  U8 *opl = inc_ptr ? start + *inc_ptr : end;
  for(; start < end; )
  {
    if(start == opl)
    {
      // NOTE: should only happen if we start with a plus or minus sign. expand the range
      Assert(*start == '+' || *start == '-');
      opl += *buf_consume_struct(&pm_idxs, U64);
      Assert(start != opl);
    }

    if(*(opl - 1) == '^')
    {
      // NOTE: hit a sign in an exponent. expand the range
      opl += *buf_consume_struct(&pm_idxs, U64);
    }

    // NOTE: parse the term of the form ((\+|\-)?%d)?(z(^(\+|\-)?%d)?)? (i suck at regexp btw)
    Buffer term = buffer_from_range(start, opl);
    R64 coeff = 0.0;
    if(buf_peek_char(term, '+') || buf_peek_char(term, '-'))
    {
      B32 coeff_is_neg = '-' == *buf_consume_struct(&term, U8);
      if(buf_peek_char(term, 'z'))
      {
	coeff = coeff_is_neg ? -1.0 : 1.0;
      }
      else
      {
	String8 term_str = str8_from_buf(term);
	term_str = str8_parse_r64(term_str, &coeff);
	if(coeff_is_neg) coeff *= -1.0;
	term = buf_from_str8(term_str);
      }
    }
    else
    {
      String8 term_str = str8_from_buf(term);
      term_str = str8_parse_r64(term_str, &coeff);
      term = buf_from_str8(term_str);
    }

    // NOTE: parse the power
    S64 power = 0;
    if(buf_peek_char(term, 'z'))
    {
      buf_consume_struct(&term, U8);
      if(buf_peek_char(term, '^'))
      {
	buf_consume_struct(&term, U8);
	String8 term_str = str8_from_buf(term);
	term_str = str8_parse_s64(term_str, &power);
	term = buf_from_str8(term_str);
      }
      else
      {
	power = 1;
      }
    }

    // NOTE: new term
    FilterTermLoose *loose_term = arena_push_struct(arena, FilterTermLoose);
    loose_term->coeff = coeff;
    loose_term->power = power;
    SLLQueuePush(list.first, list.last, loose_term);
    ++list.term_count;
    list.max_power = Max(list.max_power, power);
    list.min_power = Min(list.min_power, power);

    // NOTE: increment
    inc_ptr = buf_consume_struct(&pm_idxs, U64);
    start = opl;
    opl = inc_ptr ? start + *inc_ptr : end;
  }

  arena_release_scratch(scratch);

  return(list);
}

proc Filter
filter_from_strings(Arena *arena, String8 zeros_poly, String8 poles_poly)
{
  ArenaTemp scratch = arena_get_scratch(&arena, 1);

  // NOTE: parse zeros
  zeros_poly = str8_strip_whitespace(scratch.arena, zeros_poly);
  LooseFilterTermList loose_zero_terms = filter_parse_string(scratch.arena, zeros_poly);

  // NOTE: parse poles
  poles_poly = str8_strip_whitespace(scratch.arena, poles_poly);
  LooseFilterTermList loose_pole_terms = filter_parse_string(scratch.arena, poles_poly);

  // NOTE: bake
  U64 zero_order = 0;
  U64 pole_order = 0;

  S64 min_power = Min(loose_zero_terms.min_power, loose_pole_terms.min_power);
  if(min_power < 0)
  {
    zero_order = loose_zero_terms.max_power - min_power;
    pole_order = loose_pole_terms.max_power - min_power;
  }
  else
  {
    zero_order = loose_zero_terms.max_power;
    pole_order = loose_pole_terms.max_power;
  }
  S64 zero_power_inc = zero_order - loose_zero_terms.max_power;
  S64 pole_power_inc = pole_order - loose_pole_terms.max_power;

  R64 *zero_coeffs = arena_push_array(arena, R64, zero_order + 1);
  for(FilterTermLoose *term = loose_zero_terms.first; term; term = term->next)
  {
    R64 coeff = term->coeff;
    U64 power = term->power + zero_power_inc;
    zero_coeffs[zero_order - power] = coeff;
  }

  R64 *pole_coeffs = arena_push_array(arena, R64, pole_order + 1);
  for(FilterTermLoose *term = loose_pole_terms.first; term; term = term->next)
  {
    R64 coeff = term->coeff;
    U64 power = term->power + pole_power_inc;
    pole_coeffs[pole_order - power] = coeff;
  }

  arena_release_scratch(scratch);

  Filter result = {0};
  result.zero_order = zero_order;
  result.pole_order = pole_order;
  result.zero_coeffs = zero_coeffs;
  result.pole_coeffs = pole_coeffs;
  return(result);
}

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  if(!gfx_init()) return 1;
  if(!render_init()) return 1;

  Os_Handle window = gfx_window_open(Str8Lit("3d filter"), 640, 480);
  render_equip_window(window);

  String8 zeros_string = Str8Lit("1 - z^2");
  String8 poles_string = Str8Lit("z^2");

  Arena *frame_arena = arena_alloc();
  B32 running = 1;
  while(running)
  {
    Os_EventList events = gfx_events(frame_arena);
    for(Os_Event *event = events.first; event; event = event->next)
    {
      if(event->kind == Os_EventKind_close)
      {
	running = 0;
	break;
      }
    }

    render_begin_frame();
    //gfx_window_begin_frame(window);

    Filter filter = filter_from_strings(frame_arena, zeros_string, poles_string);
    Unused(filter);

    render_end_frame();
    //gfx_window_end_frame(window);
    arena_clear(frame_arena);
  }

  gfx_window_close(window);
  gfx_uninit();

  return(0);
}
