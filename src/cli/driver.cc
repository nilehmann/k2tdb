/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <cli/driver.h>
#include <cli/scanner.h>
#include <queries/nfa.h>

namespace k2tdb {
namespace cli {

Driver::Driver(const engine::GDBEngine &db)
    : trace_scanning(false),
      trace_parsing(false),
      lexer(),
      sym_table(),
      db_(db) {}

void Driver::query(const std::string &node, 
                   const regexp::RegExp<std::string> &expr,
                   bool count) {
  std::cout << node << "\n";
  regexp::print_expression(expr);
  if (count)
    std::cout << "  " << db_.Count(node, expr) << std::endl;
  else
    db_.Compute(node, expr, [] (const std::string &u) {
        std::cout << "  " << u << "\n";
    });
}

void Driver::query(std::string start,
                   std::string end,
                   const regexp::RegExp<std::string> &expr) {
  if (db_.ExistsPath(start, end, expr))
    std::cout << "  true" << std::endl;
  else
    std::cout << "  false" << std::endl;
}

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
  std::cout << "[k2tdb] ";
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

}  // namespace cli
}  // namespace k2tdb
