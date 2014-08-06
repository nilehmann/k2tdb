/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

%{ /*** C/C++ Declarations ***/

#include <string>

#include <parsing/scanner.h>

/* By default yylex returns int, we use token_type. Unfortunately yyterminate
 * by default returns 0, which is not of token_type. */
#define yyterminate() return token::END

typedef parser::Parser::token token;
typedef parser::Parser::token_type token_type;
namespace re = regexp;

/* This disables inclusion of unistd.h, which is not available under Visual C++
 * on Win32. The C++ scanner uses STL streams instead. */
#define YY_NO_UNISTD_H

%}

/*** Flex Declarations and Options ***/

/* enable c++ scanner class generation */
%option c++

/* change the name of the base lexer class. results in "RegExpFlexLexer" */
%option prefix="RegExp"


/* enable scanner to generate debug output. disable this for release
 * versions. */
%option debug

/* no support for include files is planned */
%option yywrap nounput 

/* enables the use of start condition stacks */
%option stack

%x str_literal

id            [a-zA-Z][a-zA-Z_0-9]*
num           [0-9]+
blank         [ \t]
assign        =
reg_exp_left  \[
reg_exp_right \]
node_left     \<
node_right    \>


/* The following paragraph suffices to track locations accurately. Each time
 * yylex is invoked, the begin position is moved onto the end position. */
%{
//#define YY_USER_ACTION  yylloc->columns(yyleng);
%}

%% /*** Regular Expressions Part ***/

%{
  yyloc->step();
%}

<str_literal>\" {
  BEGIN(INITIAL);
  yylval->build(str_buff.str());
  return token::STR_LITERAL;
}

<str_literal>[^\n\"]+ {
  str_buff << std::string(yytext);
}
<str_literal>\n {
  printf("[error]");
}

\" {
  BEGIN(str_literal);
  str_buff.str("");
}

\.                  return token::CONCAT;
\*                  return token::KLEENE;
\^                  return token::CONVERSE;
\|                  return token::ALTERNATION;
\(                  return token::LPAREN;
\)                  return token::RPAREN;
\{                  return token::LBRACE;
\}                  return token::RBRACE;
\,                  return token::COMMA;
c                   return token::COUNT;
{assign}            return token::ASSIGN;
{reg_exp_left}      return token::RE_LEFT; 
{reg_exp_right}     return token::RE_RIGHT;
{node_left}         return token::NODE_LEFT;
{node_right}        return token::NODE_RIGHT;



{num} {
  yylval->build(std::stoi(std::string(yytext)));
  return token::NUM;
}

{id} {
  yylval->build(std::string(yytext));
  return token::ID;
}



{blank}+
[\n]+

\;                  return static_cast<token_type>(*yytext);
.                   printf("[error]");


%%


