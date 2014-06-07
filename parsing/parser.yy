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
  #include <regular_expressions.h>
  #include <memory>
  
  namespace re = regular_expressions;
  typedef re::multy_op<re::concat> concat;
  typedef re::multy_op<re::alternation> alternation;
  typedef re::unary_op<re::kleene> kleene;
  using re::RegExp;
  using std::make_shared;
}
%define api.value.type variant

%type <std::shared_ptr<RegExp>> reg_exp concat kleene atom

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
}

%% /*** Grammar Rules ***/

 /*** BEGIN EXAMPLE - Change the example grammar rules below ***/

assignment: ID "=" STR_LITERAL 


reg_exp:
  concat             {$$ = make_shared<RegExp>(alternation(std::move(*$1))); }
| reg_exp "|" concat {re::push_expr(*$1, std::move(*$3)); $$ = $1;}

concat:
  kleene             {$$ = make_shared<RegExp>(concat(std::move(*$1))); }
| concat "." kleene  {push_expr(*$1, std::move(*$3)); $$ = $1;}
| concat kleene      {push_expr(*$1, std::move(*$2)); $$ = $1;}

kleene:
  atom               {$$ = $1;}
| atom "*"           {$$ = make_shared<RegExp>(kleene(std::move(*$1)));}

atom:
  val                {$$ = make_shared<RegExp>($1);}
| "(" reg_exp ")"    {$$ = $2;}

val: 
  STR_LITERAL {$$ = $1;}
| ID          {$$ = $1;}

query: "<" val ">" "[" reg_exp "]" {re::print_expression(*$5);}

command: 
  assignment ';'
| query ';'

start	: /* empty */
	| start command

 /*** END EXAMPLE - Change the example grammar rules above ***/

%% /*** Additional Code ***/

void parser::Parser::error(const Parser::location_type& l,
			    const std::string& m)
{
    driver.error(l, m);
}
