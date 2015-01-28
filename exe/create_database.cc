/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <gdloader.h>
#include <ntloader.h>
#include <engine/dictionary_encoding.h>
#include <k2tree.h>
#include <iostream>
#include <fstream>
#include <queries/regexp.h>
#include <vector>


#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/timer/timer.hpp>

#include <boost/tokenizer.hpp>


#include <sys/time.h>


namespace po = boost::program_options;
namespace lk2 = libk2tree;


std::string in_file, out_base;
std::string format;
int k1, k2, kl, k1_levels;
int w;
bool build_only = false;

k2tdb::engine::DictionaryEncoding SO;
k2tdb::engine::DictionaryEncoding P_Dic;
std::ofstream k2tdb_out;

void ParseOps(int argc, char *argv[]) {
  po::options_description ops("Usage: create_dictionary [options] input-graph"
                              " output-base\n"
                              "Allowed options");
  ops.add_options()
    ("help,h", "Print this help")
    ("format,f", po::value<std::string>(&format)->default_value("nt"),
     "Format of input file: gd, nt (default)")
    ("so", po::value<int>(&w)->default_value(1000000),
     "Expected number of subject and object")
    ("k1", po::value<int>(&k1)->default_value(4), "Arity of the first leveles")
    ("k2", po::value<int>(&k2)->default_value(2), "Arity of the second part")
    ("kl", po::value<int>(&kl)->default_value(-1), "Arity of the level height - 1")
    ("build,b", "Build only. Only avalaible for NT")
    ("k1-levels", po::value<int>(&k1_levels)->default_value(5),
     "Number of level with arity k1");

  po::options_description files;
  files.add_options()
    ("rdf-graph", po::value<std::string>(), "Input ttl file")
    ("output", po::value<std::string>(), "Output basename");

  po::options_description all("All options");
  all.add(ops).add(files);

  po::positional_options_description p;
  p.add("rdf-graph", 1);
  p.add("output", 1);

  po::variables_map map;
  po::store(po::command_line_parser(argc, argv).
            options(all).positional(p).run(), map);
  po::notify(map);

  if(map.count("help")) {
    std::cout << ops;
    exit(0);
  }

  if(map.count("build")) {
    if (format != "nt") {
      std::cout << "Build only option is only avalaible with nt format\n";
      exit(0);
    }
    build_only = true;
  }

  if (kl == -1)
    kl = k2*k2*k2;

  in_file = map["rdf-graph"].as<std::string>();
  out_base = map["output"].as<std::string>();
}


int main(int argc, char *argv[]) {
  std::ofstream a;
  ParseOps(argc, argv);
  std::ifstream in(in_file);
  if (format == "gd") {
    k2tdb::GDLoader<lk2::CompressedHybrid> loader(out_base, w, k1, k2, kl, k1_levels);
    loader.EncodeAndBuild(&in);
  } else if (format == "nt") {
    k2tdb::NTLoader<lk2::CompressedHybrid> loader(out_base);
    if (build_only)
      loader.Build(&in, k1, k2, kl, k1_levels);
    else
      loader.EncodeAndBuild(&in, w, k1, k2, kl, k1_levels);
  } else {
    std::cout << "Error: format not accepted" << std::endl;
  }

  k2tdb_out.close();
  return 0;
}
