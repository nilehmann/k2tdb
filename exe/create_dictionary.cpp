/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <iostream>
#include <fstream>
#include <regex>

#define uriref <[^> ]+>
#define literal \042[^\042]+\042
#define name [A-Za-z][A-Za-z0-9]*
#define namedNode \047_:\047[A-Za-z][A-Za-z0-9]*
#define subject uriref|namedNode
#define predicate uriref
#define object uriref|namedNode|literal
#define ws [ \t]
#define STR(v) STR0(v)
#define STR0(v) #v

std::string trim(const std::string &s) {
  if (s[0] == '<' || s[0] = '"')
    return s;
}

int main() {
  std::string triple_str = STR((subject)ws+(predicate)ws+(object)ws+\\.);
  std::regex triple(triple_str, std::regex::extended);

  std::smatch match;

  std::string filename = "test.ttl";
  std::ifstream in(filename, std::ifstream::in);
  std::string line;
  while (std::getline(in, line)) {
    bool good = std::regex_match(line, match, triple);
    if (good)
      std::cerr << match.str(1) << " " << match.str(2) << " " << match.str(3) << std::endl;
    else
      std::cerr << "not matched line: " << line << std::endl;
  }

  return 0;
}
