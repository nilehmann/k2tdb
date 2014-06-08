/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <FlexLexer.h>
#include <sstream>
#include <parser.h>
#include <driver.h>

typedef parser::Parser Parser;

FLEX_DEFINE_BASE_CLASS(RegExp, 1)

class Scanner: public FLEX_BASE_CLASS_NAME(RegExp) {
 public:
  Scanner(std::istream *arg_yyin = 0, std::ostream *arg_yyout = 0)
    : FLEX_BASE_CLASS_NAME(RegExp)(arg_yyin, arg_yyout), str_buff() {}

  int yylex(Parser::semantic_type *yylval,
            Parser::location_type *yyloc,
            Driver &driver);

  /*
  int yylex(Parser::sematic_type yylval,
            Parser::location_type yyloc,
            std::istream* new_in,
            std::ostream* new_out = 0) {

    switch_streams(new_in, new_out);
    return yylex(yylval, yyloc);
  }
  */

  /** 
   * When the scanner receives an end-of-file indication from YY_INPUT, it then
   * checks the yywrap() function. If yywrap() returns false (zero), then it is
   * assumed that the function has gone ahead and set up `yyin' to point to
   * another input file, and scanning continues. If it returns true (non-zero),
   * then the scanner terminates, returning 0 to its caller. 
   */
  int yywrap() {
    return 1;
  }
  
  std::ostringstream str_buff;
};

#define YY_DECL FLEX_YYLEX_DECL(        \
    Scanner,                            \
    int,                                \
    (Parser::semantic_type *, yylval)   \
    (Parser::location_type *, yyloc)    \
    (Driver &, driver)                  \
)
