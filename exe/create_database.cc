/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */


#include <dictionary_encoding.h>
#include <iostream>
#include <fstream>

#include <k2tree.h>

#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>


namespace po = boost::program_options;
namespace lk2 = libk2tree;

std::string base_name;
int k1, k2, kl, k1_levels;

void ParseOps(int argc, char *argv[]) {
  po::options_description ops("Usage: create_dictionary[options] input-graph"
                              " output-base\n"
                              "Allowed options");
  ops.add_options()
    ("help,h", "Print this help")
    ("k1", po::value<int>(&k1)->default_value(4), "Arity of the first leveles")
    ("k2", po::value<int>(&k2)->default_value(2), "Arity of the second part")
    ("kl", po::value<int>(&kl)->default_value(-1), "Arity of the level height - 1")
    ("k1-levels", po::value<int>(&k1_levels)->default_value(5),
     "Number of level with arity k1");

  po::options_description files;
  files.add_options()
    ("base_name", po::value<std::string>(), "Basename");

  po::options_description all("All options");
  all.add(ops).add(files);

  po::positional_options_description p;
  p.add("base_name", 1);

  po::variables_map map;
  po::store(po::command_line_parser(argc, argv).
            options(all).positional(p).run(), map);
  po::notify(map);

  if(map.count("help")) {
    std::cout << ops;
    exit(0);
  }
  if (kl == -1)
    kl = k2*k2*k2;

  base_name = map["base_name"].as<std::string>();
}

int main(int argc, char *argv[]) {
  ParseOps(argc, argv);
  std::ifstream in_triples(base_name + ".triples",
                           std::ios::in | std::ios::binary);
  DictionaryEncoding SO(base_name + ".so", false);
  DictionaryEncoding P(base_name + ".p", false);

  in_triples.seekg (0, in_triples.end);
  uint ntriples = in_triples.tellg()/3/sizeof(uint);
  in_triples.seekg (0, in_triples.beg);

  std::ofstream out(base_name + ".k2tdb");
  uint npredicates = P.Count();
  out.write(reinterpret_cast<char*>(&npredicates), sizeof(uint));
  uint curr_predicate = -1;
  lk2::K2TreeBuilder builder(SO.Count(), k1, k2, kl, k1_levels);
  for (uint i = 0; i < ntriples; ++i) {
    uint subject, predicate, object;
    in_triples.read(reinterpret_cast<char*>(&subject), sizeof(uint));
    in_triples.read(reinterpret_cast<char*>(&predicate), sizeof(uint));
    in_triples.read(reinterpret_cast<char*>(&object), sizeof(uint));
    if (curr_predicate != predicate) {
      if (curr_predicate != (uint)-1) {
        auto tree = builder.Build();
        auto compressed = tree->CompressLeaves();
        compressed->Save(&out);
      }
      curr_predicate = predicate;
      builder.Clear();
    }
    builder.AddLink(subject, object);
  }
  if (curr_predicate != (uint)-1) {
    auto tree = builder.Build();
    auto compressed = tree->CompressLeaves();
    compressed->Save(&out);
  }

  return 0;
}
