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
%lex-param { class Driver& driver}


/* verbose error messages */
%error-verbose

 /*** BEGIN EXAMPLE - Change the example grammar's tokens below ***/


%code requires {
  #include <regular_expressions.h>
  
  namespace re = regular_expressions;
  typedef re::multy_op<re::concat> concat;
  typedef re::multy_op<re::alternation> alternation;
  typedef re::unary_op<re::kleene> kleene;
  using re::RegExp;
}
%define api.value.type variant

%type <RegExp> reg_exp kleene atom
%type <concat> concat
%type <alternation> alternation

%type <int> val
%token <int> STR_LITERAL "str_literal"	
%token <int> ID          "id"

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
;



%code top {
 #include <driver.h>
 #include <scanner.h>

  /* this "connects" the bison parser in the driver to the flex scanner class
   * object. it defines the yylex() function call to pull the next token from the
   * current lexer object of the driver context. */
  #undef yylex
  #define yylex driver.lexer->yylex

  using boost::get;
}

%% /*** Grammar Rules ***/


assignment: ID "=" STR_LITERAL {driver.sym_table.SetSymbol($1, $3);}

reg_exp: alternation {$$ = std::move($1);}

alternation:
  concat                  {$$ = alternation(std::move($1)); }
| alternation "|" concat  {$1.push(std::move($3)); $$ = std::move($1);}

concat:
  kleene             {$$ = concat(std::move($1)); }
| concat "." kleene  {$1.push(std::move($3)); $$ = std::move($1);}
| concat kleene      {$1.push(std::move($2)); $$ = std::move($1);}

kleene:
  atom               {std::swap($$, $1);}
| atom "*"           {$$ = kleene(std::move($1));}

atom:
  val                {$$ = $1;}
| "(" reg_exp ")"    {std::swap($$, $2);}

val: 
  STR_LITERAL {$$ = $1;}
| ID          {$$ = driver.sym_table.LookupSymbol($1);}

query: "<" val ">" "[" reg_exp "]" {re::print_expression($5);}

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
