/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 *
 *  My definition for FlexLexer.h that make use of the Curiously recurring
 *  template pattern for function binding.
 */


#ifndef INCLUDE_FLEX_LEXER_H_
#define INCLUDE_FLEX_LEXER_H_

#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/control/expr_if.hpp>

#include <iostream>

#define TO_PAIR_SEQ(seq) BOOST_PP_CAT(TO_PAIR_SEQ_A seq, _END)
#define TO_PAIR_SEQ_A(x, y) ((x,y)) TO_PAIR_SEQ_B
#define TO_PAIR_SEQ_B(x, y) ((x,y)) TO_PAIR_SEQ_A
#define TO_PAIR_SEQ_A_END
#define TO_PAIR_SEQ_B_END


#define COMMA_TYPE_NAME(r, data, i, type_name) \
BOOST_PP_COMMA_IF(i)\
BOOST_PP_TUPLE_ELEM(2, 0, type_name) BOOST_PP_TUPLE_ELEM(2, 1, type_name)


#define PARAM_LIST(seq) \
BOOST_PP_SEQ_FOR_EACH_I(COMMA_TYPE_NAME, _, TO_PAIR_SEQ(seq))


#define FLEX_YYLEX_DECL(class_name, result_type, param_seq) \
result_type class_name::yylex(PARAM_LIST(param_seq))

#define FLEX_DEFINE_CONSTRUCTOR(class_name, base_class_prefix)                 \
class_name(std::istream *arg_yyin = 0, std::ostream *arg_yyout = 0)            \
    : FLEX_BASE_CLASS_NAME(base_class_prefix)(arg_yyin, arg_yyout) {}

#define FLEX_BASE_CLASS_NAME(prefix) prefix##FlexLexer

#define FLEX_DEFINE_BASE_CLASS(prefix, define_yywrap)                          \
extern "C++" {                                                                 \
                                                                               \
struct yy_buffer_state;                                                        \
typedef int yy_state_type;                                                     \
                                                                               \
class prefix##FlexLexer {                                                      \
 public:                                                                       \
  prefix##FlexLexer(std::istream* arg_yyin = 0, std::ostream* arg_yyout = 0);  \
                                                                               \
  ~prefix##FlexLexer();                                                        \
                                                                               \
  const char *YYText() const { return yytext; }                                \
                                                                               \
  int YYLeng() const { return yyleng; }                                        \
                                                                               \
  int lineno() const { return yylineno; }                                      \
                                                                               \
  int debug() const	{ return yy_flex_debug; }                                  \
                                                                               \
  void switch_streams(std::istream *new_in, std::ostream *new_out);            \
                                                                               \
  void set_debug(bool flag) { yy_flex_debug = flag; }                          \
                                                                               \
  void yy_switch_to_buffer(struct yy_buffer_state *new_buffer);                \
                                                                               \
  struct yy_buffer_state* yy_create_buffer(std::istream *s, int size);         \
                                                                               \
  void yy_delete_buffer(struct yy_buffer_state *b);                            \
                                                                               \
  void yyrestart(std::istream *s);                                             \
                                                                               \
  void yypush_buffer_state(struct yy_buffer_state *new_buffer);                \
                                                                               \
  void yypop_buffer_state();                                                   \
                                                                               \
  BOOST_PP_EXPR_IF(define_yywrap,virtual int yywrap() = 0;)                    \
                                                                               \
 protected:                                                                    \
  char *yytext;                                                                \
  int yyleng;                                                                  \
  int yylineno;                                                                \
  int yy_flex_debug;                                                           \
                                                                               \
  int LexerInput(char *buf, int max_size);                                     \
  virtual void LexerOutput(const char *buf, int size);                         \
  virtual void LexerError(const char *msg);                                    \
                                                                               \
  void yyunput(int c, char* buf_ptr);                                          \
  int yyinput();                                                               \
                                                                               \
  void yy_load_buffer_state();                                                 \
  void yy_init_buffer(struct yy_buffer_state *b, std::istream *s);             \
  void yy_flush_buffer(struct yy_buffer_state* b);                             \
                                                                               \
  int yy_start_stack_ptr;                                                      \
  int yy_start_stack_depth;                                                    \
  int* yy_start_stack;                                                         \
                                                                               \
  void yy_push_state(int new_state);                                           \
  void yy_pop_state();                                                         \
  int yy_top_state();                                                          \
                                                                               \
  yy_state_type yy_get_previous_state();                                       \
  yy_state_type yy_try_NUL_trans(yy_state_type current_state);                 \
                                                                               \
  int yy_get_next_buffer();                                                    \
                                                                               \
  std::istream *yyin;                                                          \
  std::ostream *yyout;                                                         \
                                                                               \
  char yy_hold_char;                                                           \
  int yy_n_chars;                                                              \
  char *yy_c_buf_p;                                                            \
  int yy_init;                                                                 \
  int yy_start;                                                                \
  int yy_did_buffer_switch_on_eof;                                             \
  size_t yy_buffer_stack_top;                                                  \
  size_t yy_buffer_stack_max;                                                  \
                                                                               \
  struct yy_buffer_state ** yy_buffer_stack;                                   \
                                                                               \
  void yyensure_buffer_stack(void);                                            \
                                                                               \
  yy_state_type yy_last_accepting_state;                                       \
  char *yy_last_accepting_cpos;                                                \
  yy_state_type *yy_state_buf;                                                 \
  yy_state_type* yy_state_ptr;                                                 \
  char *yy_full_match;                                                         \
  int *yy_full_state;                                                          \
  int yy_full_lp;                                                              \
  int yy_lp;                                                                   \
  int yy_looking_for_trail_begin;                                              \
  int yy_more_flag;                                                            \
  int yy_more_len;                                                             \
  int yy_more_offset;                                                          \
  int yy_prev_more_offset;                                                     \
};                                                                             \
                                                                               \
}                                                                              \

#endif  // INCLUDE_FLEX_LEXER_H_
