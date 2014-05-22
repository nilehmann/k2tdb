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
#include <boost/fusion/include/io.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <iostream>
#include <string>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;
namespace re = regular_expressions;

template <typename Iterator>
struct employee_parser : qi::grammar<Iterator, re::Expression(), ascii::space_type> {
    employee_parser() : employee_parser::base_type(alternation) {
        using qi::int_;
        using qi::lit;
        using qi::double_;
        using ascii::char_;
        using qi::int_;
        using qi::_val;
        using qi::_1;
        using ascii::char_;


        alternation = concat[_val = _1] >>
                      *('|' >> alternation [_val = phoenix::bind(
                          re::make_binary<re::alternation>, _val, _1)]);

        concat = kleene[_val = _1] >>
                 *(alternation [_val = phoenix::bind(
                                          re::make_binary<re::concat>,
                                          _val,
                                          _1)]);

        kleene = (atom >> '*') [_val = phoenix::bind(
                                          re::make_unary<re::kleene>,
                                          _1)] | 
                 (atom) [_val = _1];

        atom = ('(' >> alternation >> ')') | symbol;

        symbol = (char_("A-Za-z"))[_val = phoenix::bind(&re::make_symbol, _1)];
                      
        //binary = (expr >> '+' >> expr)
                 //[_val = phoenix::bind(re::make_binary<re::alternation>, _1, _2)];
                 /*|
                 (expr >> expr)
                 [_val = phoenix::bind(re::make_binary<re::concat>, _1, _2)];*/

        //expr %= (symbol | binary);
    }

    qi::rule<Iterator, re::Expression(), ascii::space_type> symbol;
    qi::rule<Iterator, re::Expression(), ascii::space_type> alternation;
    qi::rule<Iterator, re::Expression(), ascii::space_type> concat;
    qi::rule<Iterator, re::Expression(), ascii::space_type> kleene;
    qi::rule<Iterator, re::Expression(), ascii::space_type> atom;
};

int main() {
    using boost::spirit::ascii::space;
    typedef std::string::const_iterator iterator_type;
    typedef employee_parser<iterator_type> employee_parser;

    employee_parser g; // Our grammar
    std::string str;
    while (getline(std::cin, str))
    {
        if (str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;

        re::Expression expr;
        std::string::const_iterator iter = str.begin();
        std::string::const_iterator end = str.end();
        phrase_parse(iter, end, g, space, expr);
        re::apply_visitor(re::print(), expr);
    }

    return 0;
}
