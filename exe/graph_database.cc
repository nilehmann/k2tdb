/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <driver.h>

#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>


namespace po = boost::program_options;
std::ifstream db_in;


void ParseOps(int argc, char *argv[]) {
  po::options_description ops("Usage: create_dictionary[options] db"
                              "\n"
                              "Allowed options");
  ops.add_options()
    ("help,h", "Print this help");

  po::options_description files;
  files.add_options()
    ("input", po::value<std::string>(), "Input basename");

  po::options_description all("All options");
  all.add(ops).add(files);

  po::positional_options_description p;
  p.add("input", 1);

  po::variables_map map;
  po::store(po::command_line_parser(argc, argv).
            options(all).positional(p).run(), map);
  po::notify(map);

  if(map.count("help")) {
    std::cout << ops;
    exit(0);
  }

  db_in.open(map["input"].as<std::string>());
}
int main(int argc, char *argv[]) {
  ParseOps(argc, argv);
  Driver d(&db_in);
  d.parse_stdio();
}
