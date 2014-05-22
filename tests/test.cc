/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WAExpression LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <regular_expressions.h>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <iostream>
#include <string>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;
namespace re = regular_expressions;

BOOST_FUSION_ADAPT_STRUCT(
    re::multy_op<re::concat>,
    (std::vector<re::Expression>, children)
)
BOOST_FUSION_ADAPT_STRUCT(
    re::multy_op<re::alternation>,
    (std::vector<re::Expression>, children)
)
BOOST_FUSION_ADAPT_STRUCT(
    re::unary_op<re::kleene>,
    (re::Expression, expr)
)

typedef qi::symbols<char, unsigned> table;
table variable;
table lit_map;

void print_expression(const re::Expression &expr) {
  boost::apply_visitor(re::print(), expr);
  std::cout << std::endl;
}

uint get_literal(const std::string &lit) {
  return lit_map.at(lit);
}

template <typename Iterator>
struct regexp_parser : qi::grammar<Iterator, re::Expression(), ascii::space_type> {
    regexp_parser() : regexp_parser::base_type(start) {
        using qi::int_;
        using ascii::char_;
        using qi::eps;
        using qi::_1;
        using qi::_val;
        using boost::spirit::lexeme;

        lit_map.add("hola", 'a')("chao",'b');

        start %= alternation;

        alternation %= eps >> concat % '|';

        concat %= eps >> ((kleene | atom) % '.');

        kleene %= atom >> '*';

        atom %= ('(' >> alternation >> ')') | variable | lit2num;

        lit2num = literal[_val = phoenix::bind(get_literal, _1)];
        literal %= lexeme['"' >> +(char_ - '"') >> '"'];
    }

    qi::rule<Iterator, re::Expression(), ascii::space_type> start;
    qi::rule<Iterator, re::multy_op<re::alternation>(), ascii::space_type>
    alternation;
    qi::rule<Iterator, re::multy_op<re::concat>(), ascii::space_type> concat;
    qi::rule<Iterator, re::unary_op<re::kleene>(), ascii::space_type> kleene;
    qi::rule<Iterator, re::Expression(), ascii::space_type> atom;
    qi::rule<Iterator, std::string(), ascii::space_type> literal;
    qi::rule<Iterator, unsigned(), ascii::space_type> lit2num;
    //qi::rule<Iterator, std::string(), ascii::space_type> variable;
};


void add_variable(const std::string &lhs, uint rhs) {
  variable.add(lhs, rhs);
  std::cerr << lhs << " = " << rhs << std::endl;
}

template<typename Iterator>
struct cli : qi::grammar<Iterator, void(), ascii::space_type> {
  cli() : cli::base_type(prog) {
    using qi::char_;
    using qi::int_;
    using qi::_1;
    using qi::_2;


    prog = assign | g[phoenix::bind(print_expression, _1)];
    assign = (lhs >> '=' >> rhs)[phoenix::bind(add_variable, _1, _2)];
     
    lhs %= +char_("A-Za-z");
    rhs = int_;
  }
  regexp_parser<Iterator> g;

  qi::rule<Iterator, void(), ascii::space_type> prog;
  qi::rule<Iterator, void(), ascii::space_type> assign;
  qi::rule<Iterator, std::string(), ascii::space_type> lhs;
  qi::rule<Iterator, int(), ascii::space_type> rhs;
};

int main() {
    using boost::spirit::ascii::space;
    typedef std::string::const_iterator iterator_type;
    typedef regexp_parser<iterator_type> regexp_parser;
    typedef cli<iterator_type> cli;

    regexp_parser g; // Our grammar
    cli grammar;
    std::string str;
    while (getline(std::cin, str))
    {
        if (str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;

        std::string::const_iterator iter = str.begin();
        std::string::const_iterator end = str.end();
        phrase_parse(iter, end, grammar, space);
        //phrase_parse(iter, end, g, space);
    }

    return 0;
}
