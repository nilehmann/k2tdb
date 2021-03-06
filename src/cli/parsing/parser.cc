// A Bison parser, made by GNU Bison 3.0.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2013 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.
// //                    "%code top" blocks.
#line 98 "parsing/parser.yy" // lalr1.cc:392

  #include <cli/scanner.h>
  /*#include <cli/driver.h>*/

  /* this "connects" the bison parser in the driver to the flex scanner class
   * object. it defines the yylex() function call to pull the next token from the
   * current lexer object of the driver context. */
  #undef yylex
  #define yylex driver.lexer->yylex


#line 46 "src/cli/parsing/parser.cc" // lalr1.cc:392


// First part of user declarations.

#line 51 "src/cli/parsing/parser.cc" // lalr1.cc:399

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

#include <cli/parsing/parser.h>

// User implementation prologue.

#line 65 "src/cli/parsing/parser.cc" // lalr1.cc:407


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (/*CONSTCOND*/ false)
# endif


// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << std::endl;                  \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE(Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void>(0)
# define YY_STACK_PRINT()                static_cast<void>(0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyempty = true)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 28 "parsing/parser.yy" // lalr1.cc:474
namespace parser {
#line 151 "src/cli/parsing/parser.cc" // lalr1.cc:474

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  Parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr = "";
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              // Fall through.
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }


  /// Build a parser object.
  Parser::Parser (class k2tdb::cli::Driver& driver_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      driver (driver_yyarg)
  {}

  Parser::~Parser ()
  {}


  /*---------------.
  | Symbol types.  |
  `---------------*/

  inline
  Parser::syntax_error::syntax_error (const location_type& l, const std::string& m)
    : std::runtime_error (m)
    , location (l)
  {}

  // basic_symbol.
  template <typename Base>
  inline
  Parser::basic_symbol<Base>::basic_symbol ()
    : value ()
  {}

  template <typename Base>
  inline
  Parser::basic_symbol<Base>::basic_symbol (const basic_symbol& other)
    : Base (other)
    , value ()
    , location (other.location)
  {
      switch (other.type_get ())
    {
      case 23: // reg_exp
      case 26: // kleene
      case 27: // repetition
      case 28: // converse
      case 29: // atom
        value.copy< RegExp<std::string> > (other.value);
        break;

      case 24: // alternation
        value.copy< alternation<std::string> > (other.value);
        break;

      case 25: // concat
        value.copy< concat<std::string> > (other.value);
        break;

      case 3: // "str_literal"
      case 4: // "escaped_literal"
      case 5: // "lit"
      case 30: // val
        value.copy< std::string > (other.value);
        break;

      case 6: // "num"
        value.copy< uint > (other.value);
        break;

      default:
        break;
    }

  }


  template <typename Base>
  inline
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const semantic_type& v, const location_type& l)
    : Base (t)
    , value ()
    , location (l)
  {
    (void) v;
      switch (this->type_get ())
    {
      case 23: // reg_exp
      case 26: // kleene
      case 27: // repetition
      case 28: // converse
      case 29: // atom
        value.copy< RegExp<std::string> > (v);
        break;

      case 24: // alternation
        value.copy< alternation<std::string> > (v);
        break;

      case 25: // concat
        value.copy< concat<std::string> > (v);
        break;

      case 3: // "str_literal"
      case 4: // "escaped_literal"
      case 5: // "lit"
      case 30: // val
        value.copy< std::string > (v);
        break;

      case 6: // "num"
        value.copy< uint > (v);
        break;

      default:
        break;
    }
}


  // Implementation of basic_symbol constructor for each type.

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const location_type& l)
    : Base (t)
    , value ()
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const RegExp<std::string> v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const alternation<std::string> v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const concat<std::string> v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const std::string v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const uint v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}


  template <typename Base>
  inline
  Parser::basic_symbol<Base>::~basic_symbol ()
  {
    // User destructor.
    symbol_number_type yytype = this->type_get ();
    switch (yytype)
    {
   default:
      break;
    }

    // Type destructor.
    switch (yytype)
    {
      case 23: // reg_exp
      case 26: // kleene
      case 27: // repetition
      case 28: // converse
      case 29: // atom
        value.template destroy< RegExp<std::string> > ();
        break;

      case 24: // alternation
        value.template destroy< alternation<std::string> > ();
        break;

      case 25: // concat
        value.template destroy< concat<std::string> > ();
        break;

      case 3: // "str_literal"
      case 4: // "escaped_literal"
      case 5: // "lit"
      case 30: // val
        value.template destroy< std::string > ();
        break;

      case 6: // "num"
        value.template destroy< uint > ();
        break;

      default:
        break;
    }

  }

  template <typename Base>
  inline
  void
  Parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move(s);
      switch (this->type_get ())
    {
      case 23: // reg_exp
      case 26: // kleene
      case 27: // repetition
      case 28: // converse
      case 29: // atom
        value.move< RegExp<std::string> > (s.value);
        break;

      case 24: // alternation
        value.move< alternation<std::string> > (s.value);
        break;

      case 25: // concat
        value.move< concat<std::string> > (s.value);
        break;

      case 3: // "str_literal"
      case 4: // "escaped_literal"
      case 5: // "lit"
      case 30: // val
        value.move< std::string > (s.value);
        break;

      case 6: // "num"
        value.move< uint > (s.value);
        break;

      default:
        break;
    }

    location = s.location;
  }

  // by_type.
  inline
  Parser::by_type::by_type ()
     : type (empty)
  {}

  inline
  Parser::by_type::by_type (const by_type& other)
    : type (other.type)
  {}

  inline
  Parser::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  inline
  void
  Parser::by_type::move (by_type& that)
  {
    type = that.type;
    that.type = empty;
  }

  inline
  int
  Parser::by_type::type_get () const
  {
    return type;
  }
  // Implementation of make_symbol for each symbol type.
  Parser::symbol_type
  Parser::make_EOL (const location_type& l)
  {
    return symbol_type (token::EOL, l);
  }

  Parser::symbol_type
  Parser::make_STR_LITERAL (const std::string& v, const location_type& l)
  {
    return symbol_type (token::STR_LITERAL, v, l);
  }

  Parser::symbol_type
  Parser::make_ESCAPED_LITERAL (const std::string& v, const location_type& l)
  {
    return symbol_type (token::ESCAPED_LITERAL, v, l);
  }

  Parser::symbol_type
  Parser::make_LIT (const std::string& v, const location_type& l)
  {
    return symbol_type (token::LIT, v, l);
  }

  Parser::symbol_type
  Parser::make_NUM (const uint& v, const location_type& l)
  {
    return symbol_type (token::NUM, v, l);
  }

  Parser::symbol_type
  Parser::make_ASSIGN (const location_type& l)
  {
    return symbol_type (token::ASSIGN, l);
  }

  Parser::symbol_type
  Parser::make_RE_LEFT (const location_type& l)
  {
    return symbol_type (token::RE_LEFT, l);
  }

  Parser::symbol_type
  Parser::make_RE_RIGHT (const location_type& l)
  {
    return symbol_type (token::RE_RIGHT, l);
  }

  Parser::symbol_type
  Parser::make_CONCAT (const location_type& l)
  {
    return symbol_type (token::CONCAT, l);
  }

  Parser::symbol_type
  Parser::make_ALTERNATION (const location_type& l)
  {
    return symbol_type (token::ALTERNATION, l);
  }

  Parser::symbol_type
  Parser::make_KLEENE (const location_type& l)
  {
    return symbol_type (token::KLEENE, l);
  }

  Parser::symbol_type
  Parser::make_LPAREN (const location_type& l)
  {
    return symbol_type (token::LPAREN, l);
  }

  Parser::symbol_type
  Parser::make_RPAREN (const location_type& l)
  {
    return symbol_type (token::RPAREN, l);
  }

  Parser::symbol_type
  Parser::make_CONVERSE (const location_type& l)
  {
    return symbol_type (token::CONVERSE, l);
  }

  Parser::symbol_type
  Parser::make_LBRACE (const location_type& l)
  {
    return symbol_type (token::LBRACE, l);
  }

  Parser::symbol_type
  Parser::make_RBRACE (const location_type& l)
  {
    return symbol_type (token::RBRACE, l);
  }

  Parser::symbol_type
  Parser::make_COMMA (const location_type& l)
  {
    return symbol_type (token::COMMA, l);
  }

  Parser::symbol_type
  Parser::make_COUNT (const location_type& l)
  {
    return symbol_type (token::COUNT, l);
  }

  Parser::symbol_type
  Parser::make_COLON (const location_type& l)
  {
    return symbol_type (token::COLON, l);
  }



  // by_state.
  inline
  Parser::by_state::by_state ()
    : state (empty)
  {}

  inline
  Parser::by_state::by_state (const by_state& other)
    : state (other.state)
  {}

  inline
  void
  Parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.state = empty;
  }

  inline
  Parser::by_state::by_state (state_type s)
    : state (s)
  {}

  inline
  Parser::symbol_number_type
  Parser::by_state::type_get () const
  {
    return state == empty ? 0 : yystos_[state];
  }

  inline
  Parser::stack_symbol_type::stack_symbol_type ()
  {}


  inline
  Parser::stack_symbol_type::stack_symbol_type (state_type s, symbol_type& that)
    : super_type (s, that.location)
  {
      switch (that.type_get ())
    {
      case 23: // reg_exp
      case 26: // kleene
      case 27: // repetition
      case 28: // converse
      case 29: // atom
        value.move< RegExp<std::string> > (that.value);
        break;

      case 24: // alternation
        value.move< alternation<std::string> > (that.value);
        break;

      case 25: // concat
        value.move< concat<std::string> > (that.value);
        break;

      case 3: // "str_literal"
      case 4: // "escaped_literal"
      case 5: // "lit"
      case 30: // val
        value.move< std::string > (that.value);
        break;

      case 6: // "num"
        value.move< uint > (that.value);
        break;

      default:
        break;
    }

    // that is emptied.
    that.type = empty;
  }

  inline
  Parser::stack_symbol_type&
  Parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
      switch (that.type_get ())
    {
      case 23: // reg_exp
      case 26: // kleene
      case 27: // repetition
      case 28: // converse
      case 29: // atom
        value.copy< RegExp<std::string> > (that.value);
        break;

      case 24: // alternation
        value.copy< alternation<std::string> > (that.value);
        break;

      case 25: // concat
        value.copy< concat<std::string> > (that.value);
        break;

      case 3: // "str_literal"
      case 4: // "escaped_literal"
      case 5: // "lit"
      case 30: // val
        value.copy< std::string > (that.value);
        break;

      case 6: // "num"
        value.copy< uint > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }


  template <typename Base>
  inline
  void
  Parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  Parser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " ("
        << yysym.location << ": ";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  inline
  void
  Parser::yypush_ (const char* m, state_type s, symbol_type& sym)
  {
    stack_symbol_type t (s, sym);
    yypush_ (m, t);
  }

  inline
  void
  Parser::yypush_ (const char* m, stack_symbol_type& s)
  {
    if (m)
      YY_SYMBOL_PRINT (m, s);
    yystack_.push (s);
  }

  inline
  void
  Parser::yypop_ (unsigned int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  Parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  Parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  Parser::debug_level_type
  Parser::debug_level () const
  {
    return yydebug_;
  }

  void
  Parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  inline Parser::state_type
  Parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  inline bool
  Parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  Parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  Parser::parse ()
  {
    /// Whether yyla contains a lookahead.
    bool yyempty = true;

    // State.
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse" << std::endl;


    // User initialization code.
    #line 36 "parsing/parser.yy" // lalr1.cc:725
{
    // initialize the initial location object
    yyla.location.begin.filename = yyla.location.end.filename = &driver.streamname;
}

#line 847 "src/cli/parsing/parser.cc" // lalr1.cc:725

    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, yyla);

    // A new symbol was pushed on the stack.
  yynewstate:
    YYCDEBUG << "Entering state " << yystack_[0].state << std::endl;

    // Accept?
    if (yystack_[0].state == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    // Backup.
  yybackup:

    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyempty)
      {
        YYCDEBUG << "Reading a token: ";
        try
          {
            yyla.type = yytranslate_ (yylex (&yyla.value, &yyla.location));
          }
        catch (const syntax_error& yyexc)
          {
            error (yyexc);
            goto yyerrlab1;
          }
        yyempty = false;
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Discard the token being shifted.
    yyempty = true;

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", yyn, yyla);
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_(yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
        switch (yyr1_[yyn])
    {
      case 23: // reg_exp
      case 26: // kleene
      case 27: // repetition
      case 28: // converse
      case 29: // atom
        yylhs.value.build< RegExp<std::string> > ();
        break;

      case 24: // alternation
        yylhs.value.build< alternation<std::string> > ();
        break;

      case 25: // concat
        yylhs.value.build< concat<std::string> > ();
        break;

      case 3: // "str_literal"
      case 4: // "escaped_literal"
      case 5: // "lit"
      case 30: // val
        yylhs.value.build< std::string > ();
        break;

      case 6: // "num"
        yylhs.value.build< uint > ();
        break;

      default:
        break;
    }


      // Compute the default @$.
      {
        slice<stack_symbol_type, stack_type> slice (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, slice, yylen);
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
      try
        {
          switch (yyn)
            {
  case 2:
#line 114 "parsing/parser.yy" // lalr1.cc:847
    {driver.sym_table.SetSymbol(yystack_[2].value.as< std::string > (), yystack_[0].value.as< std::string > ());}
#line 987 "src/cli/parsing/parser.cc" // lalr1.cc:847
    break;

  case 3:
#line 115 "parsing/parser.yy" // lalr1.cc:847
    {driver.sym_table.SetSymbol(yystack_[2].value.as< std::string > (), yystack_[0].value.as< std::string > ());}
#line 993 "src/cli/parsing/parser.cc" // lalr1.cc:847
    break;

  case 4:
#line 116 "parsing/parser.yy" // lalr1.cc:847
    {driver.sym_table.SetSymbol(yystack_[2].value.as< std::string > (), yystack_[0].value.as< std::string > ());}
#line 999 "src/cli/parsing/parser.cc" // lalr1.cc:847
    break;

  case 5:
#line 119 "parsing/parser.yy" // lalr1.cc:847
    {yylhs.value.as< RegExp<std::string> > () = std::move(yystack_[0].value.as< alternation<std::string> > ());}
#line 1005 "src/cli/parsing/parser.cc" // lalr1.cc:847
    break;

  case 6:
#line 123 "parsing/parser.yy" // lalr1.cc:847
    {yylhs.value.as< alternation<std::string> > ().push_expr(std::move(yystack_[0].value.as< concat<std::string> > ())); }
#line 1011 "src/cli/parsing/parser.cc" // lalr1.cc:847
    break;

  case 7:
#line 124 "parsing/parser.yy" // lalr1.cc:847
    {std::swap(yylhs.value.as< alternation<std::string> > (), yystack_[2].value.as< alternation<std::string> > ()); yylhs.value.as< alternation<std::string> > ().push_expr(std::move(yystack_[0].value.as< concat<std::string> > ())); }
#line 1017 "src/cli/parsing/parser.cc" // lalr1.cc:847
    break;

  case 8:
#line 128 "parsing/parser.yy" // lalr1.cc:847
    {yylhs.value.as< concat<std::string> > ().push_expr(std::move(yystack_[0].value.as< RegExp<std::string> > ())); }
#line 1023 "src/cli/parsing/parser.cc" // lalr1.cc:847
    break;

  case 9:
#line 129 "parsing/parser.yy" // lalr1.cc:847
    {std::swap(yylhs.value.as< concat<std::string> > (), yystack_[2].value.as< concat<std::string> > ()), yylhs.value.as< concat<std::string> > ().push_expr(std::move(yystack_[0].value.as< RegExp<std::string> > ())); }
#line 1029 "src/cli/parsing/parser.cc" // lalr1.cc:847
    break;

  case 10:
#line 130 "parsing/parser.yy" // lalr1.cc:847
    {std::swap(yylhs.value.as< concat<std::string> > (), yystack_[1].value.as< concat<std::string> > ()), yylhs.value.as< concat<std::string> > ().push_expr(std::move(yystack_[0].value.as< RegExp<std::string> > ())); }
#line 1035 "src/cli/parsing/parser.cc" // lalr1.cc:847
    break;

  case 11:
#line 134 "parsing/parser.yy" // lalr1.cc:847
    {std::swap(yylhs.value.as< RegExp<std::string> > (), yystack_[0].value.as< RegExp<std::string> > ());}
#line 1041 "src/cli/parsing/parser.cc" // lalr1.cc:847
    break;

  case 12:
#line 135 "parsing/parser.yy" // lalr1.cc:847
    {yylhs.value.as< RegExp<std::string> > () = kleene<std::string>(std::move(yystack_[1].value.as< RegExp<std::string> > ()));}
#line 1047 "src/cli/parsing/parser.cc" // lalr1.cc:847
    break;

  case 13:
#line 138 "parsing/parser.yy" // lalr1.cc:847
    {std::swap(yylhs.value.as< RegExp<std::string> > (), yystack_[0].value.as< RegExp<std::string> > ());}
#line 1053 "src/cli/parsing/parser.cc" // lalr1.cc:847
    break;

  case 14:
#line 139 "parsing/parser.yy" // lalr1.cc:847
    {yylhs.value.as< RegExp<std::string> > () = repetition<std::string>(std::move(yystack_[5].value.as< RegExp<std::string> > ()), yystack_[3].value.as< uint > (), yystack_[1].value.as< uint > ());}
#line 1059 "src/cli/parsing/parser.cc" // lalr1.cc:847
    break;

  case 15:
#line 143 "parsing/parser.yy" // lalr1.cc:847
    {std::swap(yylhs.value.as< RegExp<std::string> > (), yystack_[0].value.as< RegExp<std::string> > ());}
#line 1065 "src/cli/parsing/parser.cc" // lalr1.cc:847
    break;

  case 16:
#line 144 "parsing/parser.yy" // lalr1.cc:847
    {yylhs.value.as< RegExp<std::string> > () = converse<std::string>(std::move(yystack_[1].value.as< RegExp<std::string> > ()));}
#line 1071 "src/cli/parsing/parser.cc" // lalr1.cc:847
    break;

  case 17:
#line 148 "parsing/parser.yy" // lalr1.cc:847
    {yylhs.value.as< RegExp<std::string> > () = yystack_[0].value.as< std::string > ();}
#line 1077 "src/cli/parsing/parser.cc" // lalr1.cc:847
    break;

  case 18:
#line 149 "parsing/parser.yy" // lalr1.cc:847
    {std::swap(yylhs.value.as< RegExp<std::string> > (), yystack_[1].value.as< RegExp<std::string> > ());}
#line 1083 "src/cli/parsing/parser.cc" // lalr1.cc:847
    break;

  case 19:
#line 154 "parsing/parser.yy" // lalr1.cc:847
    {yylhs.value.as< std::string > () = yystack_[0].value.as< std::string > ();}
#line 1089 "src/cli/parsing/parser.cc" // lalr1.cc:847
    break;

  case 20:
#line 155 "parsing/parser.yy" // lalr1.cc:847
    {yylhs.value.as< std::string > () = yystack_[0].value.as< std::string > ();}
#line 1095 "src/cli/parsing/parser.cc" // lalr1.cc:847
    break;

  case 21:
#line 156 "parsing/parser.yy" // lalr1.cc:847
    {yylhs.value.as< std::string > () = yystack_[0].value.as< std::string > ();}
#line 1101 "src/cli/parsing/parser.cc" // lalr1.cc:847
    break;

  case 22:
#line 157 "parsing/parser.yy" // lalr1.cc:847
    {yylhs.value.as< std::string > () = driver.sym_table.LookupSymbol(yystack_[0].value.as< std::string > ());}
#line 1107 "src/cli/parsing/parser.cc" // lalr1.cc:847
    break;

  case 23:
#line 160 "parsing/parser.yy" // lalr1.cc:847
    {driver.query(yystack_[4].value.as< std::string > (), yystack_[2].value.as< RegExp<std::string> > (), true);}
#line 1113 "src/cli/parsing/parser.cc" // lalr1.cc:847
    break;

  case 24:
#line 161 "parsing/parser.yy" // lalr1.cc:847
    {driver.query(yystack_[3].value.as< std::string > (), yystack_[1].value.as< RegExp<std::string> > ());}
#line 1119 "src/cli/parsing/parser.cc" // lalr1.cc:847
    break;

  case 25:
#line 162 "parsing/parser.yy" // lalr1.cc:847
    {driver.query(yystack_[4].value.as< std::string > (), yystack_[0].value.as< std::string > (), yystack_[2].value.as< RegExp<std::string> > ());}
#line 1125 "src/cli/parsing/parser.cc" // lalr1.cc:847
    break;

  case 26:
#line 165 "parsing/parser.yy" // lalr1.cc:847
    {std::cout << "[k2tdb] ";}
#line 1131 "src/cli/parsing/parser.cc" // lalr1.cc:847
    break;

  case 27:
#line 166 "parsing/parser.yy" // lalr1.cc:847
    {std::cout << "[k2tdb] ";}
#line 1137 "src/cli/parsing/parser.cc" // lalr1.cc:847
    break;


#line 1141 "src/cli/parsing/parser.cc" // lalr1.cc:847
            default:
              break;
            }
        }
      catch (const syntax_error& yyexc)
        {
          error (yyexc);
          YYERROR;
        }
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, yylhs);
    }
    goto yynewstate;

  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yyla.location, yysyntax_error_ (yystack_[0].state,
                                           yyempty ? yyempty_ : yyla.type_get ()));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyempty)
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyempty = true;
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;
    yyerror_range[1].location = yystack_[yylen - 1].location;
    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yyterror_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
                {
                  yyn = yytable_[yyn];
                  if (0 < yyn)
                    break;
                }
            }

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yyerror_range[1].location = yystack_[0].location;
          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = yyn;
      yypush_ ("Shifting", error_token);
    }
    goto yynewstate;

    // Accept.
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    // Abort.
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (!yyempty)
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack"
                 << std::endl;
        // Do not try to display the values of the reclaimed symbols,
        // as their printer might throw an exception.
        if (!yyempty)
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  void
  Parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what());
  }

  // Generate an error message.
  std::string
  Parser::yysyntax_error_ (state_type yystate, symbol_number_type yytoken) const
  {
    std::string yyres;
    // Number of reported tokens (one for the "unexpected", one per
    // "expected").
    size_t yycount = 0;
    // Its maximum.
    enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
    // Arguments of yyformat.
    char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];

    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yytoken) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state
         merging (from LALR or IELR) and default reductions corrupt the
         expected token list.  However, the list is correct for
         canonical LR with one exception: it will still contain any
         token that will not be accepted due to an error action in a
         later state.
    */
    if (yytoken != yyempty_)
      {
        yyarg[yycount++] = yytname_[yytoken];
        int yyn = yypact_[yystate];
        if (!yy_pact_value_is_default_ (yyn))
          {
            /* Start YYX at -YYN if negative to avoid negative indexes in
               YYCHECK.  In other words, skip the first -YYN actions for
               this state because they are default actions.  */
            int yyxbegin = yyn < 0 ? -yyn : 0;
            // Stay within bounds of both yycheck and yytname.
            int yychecklim = yylast_ - yyn + 1;
            int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
            for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
              if (yycheck_[yyx + yyn] == yyx && yyx != yyterror_
                  && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
                {
                  if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                    {
                      yycount = 1;
                      break;
                    }
                  else
                    yyarg[yycount++] = yytname_[yyx];
                }
          }
      }

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
        YYCASE_(0, YY_("syntax error"));
        YYCASE_(1, YY_("syntax error, unexpected %s"));
        YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    // Argument number.
    size_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += yytnamerr_ (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char Parser::yypact_ninf_ = -14;

  const signed char Parser::yytable_ninf_ = -1;

  const signed char
  Parser::yypact_[] =
  {
     -14,     1,   -14,   -14,   -14,     6,    10,    18,    11,    22,
     -14,    31,   -14,   -14,     7,   -14,   -14,   -14,   -14,   -14,
       7,    14,    15,     4,   -14,    16,   -14,   -13,   -14,    23,
      27,     7,     7,   -14,   -14,   -14,    32,   -14,   -14,   -14,
       4,   -14,    21,    34,    24,   -14
  };

  const unsigned char
  Parser::yydefact_[] =
  {
      28,     0,     1,    19,    20,    21,     0,     0,     0,     0,
      29,     0,    22,    26,     0,    27,     2,     3,     4,    21,
       0,     0,     5,     6,     8,    11,    13,    15,    17,     0,
      24,     0,     0,    10,    12,    16,     0,    18,    23,    25,
       7,     9,     0,     0,     0,    14
  };

  const signed char
  Parser::yypgoto_[] =
  {
     -14,   -14,    25,   -14,    12,    -7,   -14,   -14,   -14,    -1,
     -14,   -14,   -14
  };

  const signed char
  Parser::yydefgoto_[] =
  {
      -1,     7,    21,    22,    23,    24,    25,    26,    27,    28,
       9,    10,     1
  };

  const unsigned char
  Parser::yytable_[] =
  {
       8,     2,    35,    36,     3,     4,     5,     3,     4,    19,
       3,     4,    19,    11,    32,    12,    33,    20,    13,    14,
      20,     6,    15,    30,     6,    41,    31,     6,    34,    39,
       3,     4,    19,    33,    16,    17,    18,    37,    42,    43,
      44,    45,     0,    40,     0,    29,    38,     6
  };

  const signed char
  Parser::yycheck_[] =
  {
       1,     0,    15,    16,     3,     4,     5,     3,     4,     5,
       3,     4,     5,     7,    10,     5,    23,    13,     0,     8,
      13,    20,     0,     9,    20,    32,    11,    20,    12,    30,
       3,     4,     5,    40,     3,     4,     5,    14,     6,    18,
       6,    17,    -1,    31,    -1,    20,    19,    20
  };

  const unsigned char
  Parser::yystos_[] =
  {
       0,    33,     0,     3,     4,     5,    20,    22,    30,    31,
      32,     7,     5,     0,     8,     0,     3,     4,     5,     5,
      13,    23,    24,    25,    26,    27,    28,    29,    30,    23,
       9,    11,    10,    26,    12,    15,    16,    14,    19,    30,
      25,    26,     6,    18,     6,    17
  };

  const unsigned char
  Parser::yyr1_[] =
  {
       0,    21,    22,    22,    22,    23,    24,    24,    25,    25,
      25,    26,    26,    27,    27,    28,    28,    29,    29,    30,
      30,    30,    30,    31,    31,    31,    32,    32,    33,    33
  };

  const unsigned char
  Parser::yyr2_[] =
  {
       0,     2,     3,     3,     3,     1,     1,     3,     1,     3,
       2,     1,     2,     1,     6,     1,     2,     1,     3,     1,
       1,     1,     2,     5,     4,     5,     2,     2,     0,     2
  };



  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const Parser::yytname_[] =
  {
  "\"end of file\"", "error", "$undefined", "\"str_literal\"",
  "\"escaped_literal\"", "\"lit\"", "\"num\"", "\"=\"", "\"[\"", "\"]\"",
  "\"/\"", "\"+\"", "\"*\"", "\"(\"", "\")\"", "\"^\"", "\"{\"", "\"}\"",
  "\",\"", "\"c\"", "\":\"", "$accept", "assignment", "reg_exp",
  "alternation", "concat", "kleene", "repetition", "converse", "atom",
  "val", "query", "command", "start", YY_NULLPTR
  };

#if YYDEBUG
  const unsigned char
  Parser::yyrline_[] =
  {
       0,   114,   114,   115,   116,   119,   123,   124,   128,   129,
     130,   134,   135,   138,   139,   143,   144,   148,   149,   154,
     155,   156,   157,   160,   161,   162,   165,   166,   169,   170
  };

  // Print the state stack on the debug stream.
  void
  Parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  Parser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):" << std::endl;
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  // Symbol number corresponding to token number t.
  inline
  Parser::token_number_type
  Parser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
     0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20
    };
    const unsigned int user_token_number_max_ = 275;
    const token_number_type undef_token_ = 2;

    if (static_cast<int>(t) <= yyeof_)
      return yyeof_;
    else if (static_cast<unsigned int> (t) <= user_token_number_max_)
      return translate_table[t];
    else
      return undef_token_;
  }

#line 28 "parsing/parser.yy" // lalr1.cc:1155
} // parser
#line 1584 "src/cli/parsing/parser.cc" // lalr1.cc:1155
#line 173 "parsing/parser.yy" // lalr1.cc:1156
 /*** Additional Code ***/

void parser::Parser::error(const Parser::location_type& l,
			    const std::string& m)
{
    driver.error(l, m);
}
