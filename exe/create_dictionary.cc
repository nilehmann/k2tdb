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
#include <regex>
#include <vector>


#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

#define URIREF <[^> ]+>
#define LITERAL \042[^\042]+\042
#define NAMEDNODE \047_:\047[A-Za-z][A-Za-z0-9]*
#define SUBJECT URIREF|NAMEDNODE
#define PREDICATE URIREF
#define OBJECT URIREF|NAMEDNODE|LITERAL
#define WS [ \t]
#define STR(v) STR0(v)
#define STR0(v) #v

namespace po = boost::program_options;

std::string in_file, out_base;

std::string trim(const std::string &s) {
  if (s[0] == '<' || s[0] == '"')
    return s.substr(1, s.size() - 2);
  return s;
}
void ParseOps(int argc, char *argv[]) {
  po::options_description ops("Usage: create_dictionary[options] input-graph"
                              " output-base\n"
                              "Allowed options");
  ops.add_options()
    ("help,h", "Print this help");

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

  in_file = map["rdf-graph"].as<std::string>();
  out_base = map["output"].as<std::string>();
}

int main(int argc, char *argv[]) {
  ParseOps(argc, argv);
  DictionaryEncoding SO(out_base + ".so", true);
  DictionaryEncoding P(out_base + ".p", true);

  std::string triple_str = STR((SUBJECT)WS+(PREDICATE)WS+(OBJECT)WS+\\.);
  std::regex triple(triple_str, std::regex::extended);
  std::smatch match;

  std::ifstream in(in_file, std::ifstream::in);
  std::string line;

  std::vector<uint> npredicates(1,0);
  while (std::getline(in, line)) {
    bool is_triple = std::regex_match(line, match, triple);
    if (is_triple) {
      std::string subject = trim(match.str(1));
      std::string predicate = trim(match.str(2));
      std::string object = trim(match.str(3));

      SO.Add(subject);
      SO.Add(object);
      if (P.Add(predicate))
        npredicates.push_back(0);

      uint n;
      P.String2Int(predicate, &n);
      ++npredicates[n+1];
      
      uint isubject, ipredicate, iobject;
      SO.String2Int(subject, &isubject);
      SO.String2Int(object, &iobject);
      P.String2Int(predicate, &ipredicate);
    }
  }
  for (uint i = 1; i < npredicates.size(); ++i)
    npredicates[i] += npredicates[i-1];

  std::ofstream triples_out(out_base + ".triples",
                            std::ios::out | std::ios::binary);
  in.clear();
  in.seekg(0, in.beg);

  while (std::getline(in, line)) {
    bool is_triple = std::regex_match(line, match, triple);
    if (is_triple) {
      std::string subject = trim(match.str(1));
      std::string predicate = trim(match.str(2));
      std::string object = trim(match.str(3));


      uint isubject, ipredicate, iobject;
      SO.String2Int(subject, &isubject);
      SO.String2Int(object, &iobject);
      P.String2Int(predicate, &ipredicate);
      triples_out.seekp(3*npredicates[ipredicate]*sizeof(uint));

      std::cerr << 3*npredicates[ipredicate]*sizeof(uint) << std::endl;
      std::cerr << isubject << " " << ipredicate << " " << iobject << std::endl;

      ++npredicates[ipredicate];
      triples_out.write(reinterpret_cast<char*>(&isubject), sizeof(uint));
      triples_out.write(reinterpret_cast<char*>(&ipredicate), sizeof(uint));
      triples_out.write(reinterpret_cast<char*>(&iobject), sizeof(uint));
    }
  }

  return 0;
}
