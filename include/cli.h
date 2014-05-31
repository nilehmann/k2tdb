/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */
#include <graph_queries_basic.h>
#include <regular_expressions.h>
#include <symbol_table.h>
#include <string_table.h>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/support/deduce.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/home/classic/error_handling/exceptions.hpp>
#include <string>

namespace spirit = boost::spirit;
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;
namespace fusion = boost::fusion;
namespace re = regular_expressions;



BOOST_FUSION_ADAPT_STRUCT(
    re::multy_op<re::concat>,
    (std::vector<re::RegExp>, children)
)
BOOST_FUSION_ADAPT_STRUCT(
    re::multy_op<re::alternation>,
    (std::vector<re::RegExp>, children)
)
BOOST_FUSION_ADAPT_STRUCT(
    re::unary_op<re::kleene>,
    (re::RegExp, expr)
)

BOOST_PHOENIX_ADAPT_FUNCTION(void, print_expression, re::print_expression, 1)

namespace commands {
using qi::_1;
using qi::_2;
using qi::_4;
using qi::_3;
using qi::_pass;
using qi::_val;
using qi::char_;
using qi::int_;
using qi::eps;
using phoenix::if_;
using phoenix::construct;
using phoenix::at_c;
using phoenix::local_names::_a;
using phoenix::val;
using phoenix::nothing;
using spirit::as_string;
using spirit::lexeme;
using fusion::traits::deduce;




typedef std::string::const_iterator str_iterator;
class regexp_parser
    : public qi::grammar<str_iterator, re::RegExp(), ascii::space_type> {
 public:
  regexp_parser(SymbolTable *st, StringTable *strtab)
      :base_type(start), symtable(st), strtable(strtab) {
    start %=  '[' > alternation > ']';

    alternation %= eps >> (concat % '|');

    concat %= eps >> (kleene % '.');

    kleene = (term >> -char_('*'))
        [if_(_2) [
          let(_a = construct<re::unary_op<re::kleene>>()) [
            at_c<0>(_a) = _1,
            _val = _a
          ]
        ].else_ [
          _val = _1 
        ]];

    term %= ('(' >> alternation >> ')') | variable | literal;

    variable = as_string[+char_("A-Za-z")]
        [if_(phoenix::bind(&SymbolTable::Contains, symtable, _1)) [
          _val = phoenix::bind(&SymbolTable::GetValue, symtable,_1)
        ].else_ [
          std::cerr << val("error: undefined variable ") << _1 << std::endl,
          _pass = false
        ]];


    literal = as_string[lexeme['"' > +(char_ - '"') > '"']]
        [_val = phoenix::bind(&StringTable::AddString, strtable, _1)];

    qi::on_error<qi::fail>(
        start,
        std::cout << val("error: unexpected token") << val(" : \"")
          << construct<std::string>(_3, _2)
          << val("\"")
          << std::endl);
  }

 private:
  SymbolTable *symtable;
  StringTable *strtable;
  qi::rule<str_iterator, re::RegExp(), ascii::space_type> start;
  qi::rule<str_iterator, re::multy_op<re::alternation>(), ascii::space_type>
  alternation;
  qi::rule<str_iterator, re::multy_op<re::concat>(), ascii::space_type> concat;
  qi::rule<str_iterator, re::RegExp(), ascii::space_type> kleene;
  qi::rule<str_iterator, re::RegExp(), ascii::space_type> term;
  qi::rule<str_iterator, unsigned(), ascii::space_type> variable;
  qi::rule<str_iterator, unsigned(), ascii::space_type> literal;
};

class assignment_parser: public qi::grammar<str_iterator, void(), ascii::space_type> {
 public:
  assignment_parser(SymbolTable *st, StringTable *strtab)
      : base_type(assignment), symtable(st), strtable(strtab) {

    assignment = (lhs > '=' > rhs)
      [phoenix::bind(&SymbolTable::SetSymbol, symtable, _1, _2)];
     
    lhs %= +char_("A-Za-z");
    rhs = as_string[lexeme['"' > +char_("A-Za-z") > '"']]
      [_val = phoenix::bind(&StringTable::AddString, strtable, _1)];


    lhs.name("variable name");
    rhs.name("string literal");

    qi::on_error<qi::fail>(
        assignment,
        std::cout << val("error: expected ") << _4 << val(" : \"")
          << construct<std::string>(_3, _2)
          << val("\"")
          << std::endl);
  }
 private:
  SymbolTable *symtable;
  StringTable *strtable;

  qi::rule<str_iterator, void(), ascii::space_type> assignment;
  qi::rule<str_iterator, std::string(), ascii::space_type> lhs;
  qi::rule<str_iterator, int(), ascii::space_type> rhs;
};

class command_parser: public qi::grammar<str_iterator, void(), ascii::space_type> {
 public:
  command_parser()
      : base_type(prog),
        symtable(), 
        strtable(),
        g(&symtable, &strtable),
        assignment(&symtable, &strtable) {

    prog = g[print_expression(_1)] | assignment;


  }

 private:
  SymbolTable symtable;
  StringTable strtable;
  regexp_parser g;
  assignment_parser assignment;

  qi::rule<str_iterator, void(), ascii::space_type> prog;
};



}  // namespace commands 
