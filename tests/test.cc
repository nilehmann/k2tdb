/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <cli.h>
#include <boost/spirit/include/qi_parse.hpp>
#include <iostream>


int main() {
    using boost::spirit::ascii::space;

    commands::command_parser grammar;
    std::string str;
    std::cout << "> ";
    while (getline(std::cin, str))
    {
        if (str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;

        std::string::const_iterator iter = str.begin();
        std::string::const_iterator end = str.end();
        phrase_parse(iter, end, grammar, space);
        std::cout << std::endl << "> ";
    }
    return 0;
}

