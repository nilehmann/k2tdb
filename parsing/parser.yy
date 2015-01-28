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
%parse-param { class k2tdb::cli::Driver& driver }


/* verbose error messages */
%error-verbose

 /*** BEGIN EXAMPLE - Change the example grammar's tokens below ***/


%code requires {
  #include <queries/regexp.h>
  #include <cli/driver.h>
  
  namespace re = k2tdb::regexp;
  using re::concat;
  using re::alternation;
  using re::kleene;
  using re::converse;
  using re::repetition;
  using re::RegExp;
}
%define api.value.type variant

%type <RegExp<std::string>> reg_exp kleene converse atom repetition
%type <concat<std::string>> concat
%type <alternation<std::string>> alternation
%type <std::string> val

%token <std::string> STR_LITERAL "str_literal"	
%token <std::string> ESCAPED_LITERAL "escaped_literal"	
%token <std::string> LIT         "lit"
%token <uint>        NUM         "num"


%token
  EOL 0       "end of file"
  ASSIGN      "="
  RE_LEFT     "["
  RE_RIGHT    "]"
  CONCAT      "/"
  ALTERNATION "+"
  KLEENE      "*"
  LPAREN      "("
  RPAREN      ")"
  CONVERSE    "^"
  LBRACE      "{"
  RBRACE      "}"
  COMMA       ","
  COUNT       "c"
  COLON       ":"
;



%code top {
  #include <cli/scanner.h>
  /*#include <cli/driver.h>*/

  /* this "connects" the bison parser in the driver to the flex scanner class
   * object. it defines the yylex() function call to pull the next token from the
   * current lexer object of the driver context. */
  #undef yylex
  #define yylex driver.lexer->yylex

}

%% /*** Grammar Rules ***/


assignment: 
  LIT "=" STR_LITERAL     {driver.sym_table.SetSymbol($1, $3);}
| LIT "=" ESCAPED_LITERAL {driver.sym_table.SetSymbol($1, $3);}
| LIT "=" LIT             {driver.sym_table.SetSymbol($1, $3);}

/* type: regexp */
reg_exp: alternation {$$ = std::move($1);}

/* type: alternation */
alternation:
  concat                  {$$.push_expr(std::move($1)); }
| alternation "+" concat  {std::swap($$, $1); $$.push_expr(std::move($3)); }

/* type: concat */
concat:
  kleene             {$$.push_expr(std::move($1)); }
| concat "/" kleene  {std::swap($$, $1), $$.push_expr(std::move($3)); }
| concat kleene      {std::swap($$, $1), $$.push_expr(std::move($2)); }

/* type: regexp */
kleene:
  repetition           {std::swap($$, $1);}
| repetition "*"       {$$ = kleene<std::string>(std::move($1));}

repetition:
  converse                 {std::swap($$, $1);}
| atom "{" NUM "," NUM "}" {$$ = repetition<std::string>(std::move($1), $3, $5);}

/* type: regexp */
converse:
  atom      {std::swap($$, $1);}
| atom "^"  {$$ = converse<std::string>(std::move($1));}

/* type: regexp */
atom:
  val              {$$ = $1;}
| "(" reg_exp ")"  {std::swap($$, $2);}


/* type: string */
val: 
  STR_LITERAL      {$$ = $1;}
| ESCAPED_LITERAL  {$$ = $1;}
| LIT              {$$ = $1;}
| ":" LIT          {$$ = driver.sym_table.LookupSymbol($2);}

query: 
  val "[" reg_exp "]" "c"  {driver.query($1, $3, true);}
| val "[" reg_exp "]"      {driver.query($1, $3);}
| val "[" reg_exp "]" val  {driver.query($1, $5, $3);}

command: 
  assignment EOL    {std::cout << "[k2tdb] ";}
| query EOL         {std::cout << "[k2tdb] ";}
/*| EOL               {std::cout << "[k2tdb] ";}*/

start	: /* empty */
	| start command


%% /*** Additional Code ***/

void parser::Parser::error(const Parser::location_type& l,
			    const std::string& m)
{
    driver.error(l, m);
}
