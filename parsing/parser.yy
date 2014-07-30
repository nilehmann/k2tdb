/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */


/*** yacc/bison Declarations ***/

/* Require bison 2.3 or later */
%require "3.0.2"

/* add debug output code to generated parser. disable this for release
 * versions. */
%debug

/* start symbol is named "start" */
%start start


/* use newer C++ skeleton file */
%skeleton "lalr1.cc"

/* namespace to enclose parser in */
%define api.namespace {parser}

/* set the parser's class identifier */
%define parser_class_name {Parser}

/* keep track of the current position within the input */
%locations
%initial-action
{
    // initialize the initial location object
    @$.begin.filename = @$.end.filename = &driver.streamname;
};

/* The driver is passed by reference to the parser and to the scanner. This
 * provides a simple but effective pure interface, not relying on global
 * variables. */
%parse-param { class Driver& driver }


/* verbose error messages */
%error-verbose

 /*** BEGIN EXAMPLE - Change the example grammar's tokens below ***/


%code requires {
  #include <regexp.h>
  
  namespace re = regexp;
  using re::concat;
  using re::alternation;
  using re::kleene;
  using re::converse;
  using re::RegExp;
}
%define api.value.type variant

%type <RegExp<std::string>> reg_exp kleene converse atom
%type <concat<std::string>> concat
%type <alternation<std::string>> alternation

%type <std::string> val
%token <std::string> STR_LITERAL "str_literal"	
%token <std::string> ID          "id"

%token
  END 0       "end of file"
  ASSIGN      "="
  NODE_LEFT   "<"
  NODE_RIGHT  ">"
  RE_LEFT     "["
  RE_RIGHT    "]"
  CONCAT      "."
  ALTERNATION "|"
  KLEENE      "*"
  LPAREN      "("
  RPAREN      ")"
  CONVERSE    "^"
;



%code top {
 #include <driver.h>
 #include <parsing/scanner.h>

  /* this "connects" the bison parser in the driver to the flex scanner class
   * object. it defines the yylex() function call to pull the next token from the
   * current lexer object of the driver context. */
  #undef yylex
  #define yylex driver.lexer->yylex

}

%% /*** Grammar Rules ***/


assignment: ID "=" STR_LITERAL {driver.sym_table.SetSymbol($1, $3);}

/* type: regexp */
reg_exp: alternation {$$ = std::move($1);}

/* type: alternation */
alternation:
  concat                  {$$.push_expr(std::move($1)); }
| alternation "|" concat  {std::swap($$, $1); $$.push_expr(std::move($3)); }

/* type: concat */
concat:
  kleene             {$$.push_expr(std::move($1)); }
| concat "." kleene  {std::swap($$, $1), $$.push_expr(std::move($3)); }
| concat kleene      {std::swap($$, $1), $$.push_expr(std::move($2)); }

/* type: regexp */
kleene:
  converse           {std::swap($$, $1);}
| converse "*"       {$$ = kleene<std::string>(std::move($1));}

/* type: regexp */
converse:
  atom               {std::swap($$, $1);}
| "^" atom           {$$ = converse<std::string>(std::move($2));}

/* type: regexp */
atom:
  val                {$$ = $1;}
| "(" reg_exp ")"    {std::swap($$, $2);}

/* type: int */
val: 
  STR_LITERAL {$$ = $1;}
| ID          {$$ = driver.sym_table.LookupSymbol($1);}

query: "<" val ">" "[" reg_exp "]" {driver.query($2, $5);}

command: 
  assignment ';'
| query ';'

start	: /* empty */
	| start command


%% /*** Additional Code ***/

void parser::Parser::error(const Parser::location_type& l,
			    const std::string& m)
{
    driver.error(l, m);
}
