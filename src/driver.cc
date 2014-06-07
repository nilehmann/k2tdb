/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <driver.h>
#include <scanner.h>

Driver::Driver()
    : trace_scanning(false),
      trace_parsing(false) {}

bool Driver::parse_stream(std::istream& in,
      const std::string& sname) {
  streamname = sname;

  Scanner scanner(&in);
  scanner.set_debug(trace_scanning);
  this->lexer = &scanner;

  Parser parser(*this);
  parser.set_debug_level(trace_parsing);
  return (parser.parse() == 0);

  return 1;
}

bool Driver::parse_stdio() {
  return parse_stream(std::cin, "standard input");
}

bool Driver::parse_string(const std::string& input,
      const std::string& sname) {
  std::istringstream iss(input);
  return parse_stream(iss, sname);
}

bool Driver::parse_file(const std::string& filename) {
  std::ifstream in(filename.c_str());
  if (!in.good()) return false;
  return parse_stream(in, filename);
}

void Driver::error(const parser::location& l, const std::string& m) const {
  std::cerr << l << ": " << m << std::endl;
}

void Driver::error(const std::string& m) const {
  std::cerr << m << std::endl;
}
