/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */
#include <engine/dictionary_encoding.h>
#include <k2tree.h>
#include <iostream>
#include <fstream>
#include <regex>
#include <vector>


#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/timer/timer.hpp>



#define URIREF <[^> ]+>
#define ECHAR \\\\[tbnrf\042\047\\\\]
#define UCHAR \\\\\165[0-9A-Fa-f]{4}|\\\\\125[0-9A-Fa-f]{8}
#define LANGTAG @[a-zA-Z]+(-[a-zA-Z0-9]+)*
#define STR_LITERAL \042([^\042]|ECHAR|UCHAR)*\042
#define LITERAL STR_LITERAL(\\^\\^URIREF|LANGTAG)?
#define NAMEDNODE \047_:\047[A-Za-z][A-Za-z0-9]*
#define SUBJECT URIREF|NAMEDNODE
#define PREDICATE URIREF
#define OBJECT URIREF|NAMEDNODE|LITERAL
#define WS [ \t]
#define STR(v) STR0(v)
#define STR0(v) #v

namespace po = boost::program_options;
namespace lk2 = libk2tree;


std::string in_file, out_base;
int k1, k2, kl, k1_levels;
int w;
int precision;

//std::string trim(const std::string &s) {
  //if (s[0] == '<' || s[0] == '"')
    //return s.substr(1, s.size() - 2);
  //return s;
//}
void ParseOps(int argc, char *argv[]) {
  po::options_description ops("Usage: create_dictionary [options] input-graph"
                              " output-base\n"
                              "Allowed options");
  ops.add_options()
    ("help,h", "Print this help")
    ("so", po::value<int>(&w)->default_value(1000000),
     "Expected number of subject and object")
    ("precision,p", po::value<int>(&precision)->default_value(2),
     "Expected number of subject and object")
    ("k1", po::value<int>(&k1)->default_value(4), "Arity of the first leveles")
    ("k2", po::value<int>(&k2)->default_value(2), "Arity of the second part")
    ("kl", po::value<int>(&kl)->default_value(-1), "Arity of the level height - 1")
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

  if (kl == -1)
    kl = k2*k2*k2;

  in_file = map["rdf-graph"].as<std::string>();
  out_base = map["output"].as<std::string>();
}

std::vector<boost::tuple<uint, uint, uint>> triples;

template<int N>
struct tuple_compare {
  bool operator()(const boost::tuple<uint, uint, uint> &a,
                  const boost::tuple<uint, uint, uint> &b) {
    return boost::get<N>(a) < boost::get<N>(b);
  }
};
void Encode() {
  boost::timer::auto_cpu_timer timer(
      precision,
      "[ENC] Encoding finished: %us user, %ss sys, %ws all\n");
  std::cout << "[ENC] Encoding started\n";
  DictionaryEncoding SO;
  SO.Create(out_base + ".so", w);
  DictionaryEncoding P;
  P.Create(out_base + ".p", 10000);

  //std::clog << STR((SUBJECT)WS+(PREDICATE)WS+(OBJECT)WS+\\.) << std::endl;
  std::string triple_str = STR((SUBJECT)WS+(PREDICATE)WS+(OBJECT)WS+\\.);
  std::regex triple(triple_str, std::regex::extended);
  std::smatch match;

  std::ifstream in(in_file, std::ifstream::in);
  std::string line;

  uint nlines = 0;
  while (std::getline(in, line)) {
    bool is_triple = std::regex_match(line, match, triple);
    if (is_triple) {
      std::string subject = match.str(1);
      std::string predicate = match.str(2);
      std::string object = match.str(3);

      //std::clog << "|" << subject << "| |" << predicate << "| |" << object << "|\n";

      SO.Add(subject);
      SO.Add(object);
      P.Add(predicate);

      uint isubject, ipredicate, iobject;
      SO.Encode(subject, &isubject);
      SO.Encode(object, &iobject);
      P.Encode(predicate, &ipredicate);

      triples.emplace_back(isubject, ipredicate, iobject);
      if (triples.size() % 1000000 == 0)
        std::cout << "[ENC] Triples encoded: " << triples.size() << std::endl;
    } else {
      std::clog << "[ENC] No matched line: " << line << std::endl;
    }
    nlines++;
  }
  std::clog << "[ENC] Triples encoded: " << triples.size() << std::endl;
  std::clog << "[ENC] Lines readed: " << nlines << std::endl;
  std::clog << "[ENC] Predicates: " << P.Count() << std::endl;
  std::clog << "[ENC] Subjects + Objects: " << SO.Count() << std::endl;
  std::sort(triples.begin(), triples.end(), tuple_compare<1>());
}

void Build() {
  boost::timer::auto_cpu_timer timer(
      precision,
      "[BLD] Building finished: %us user, %ss sys, %ws all\n");
  std::cout << "[BLD] Building started\n";
  DictionaryEncoding SO;
  SO.Open(out_base + ".so");
  DictionaryEncoding P;
  P.Open(out_base + ".p");
  std::ofstream out(out_base + ".k2tdb");

  uint npredicates = P.Count();
  out.write(reinterpret_cast<char*>(&npredicates), sizeof(uint));
  uint curr_predicate = -1;
  lk2::K2TreeBuilder builder(SO.Count(), k1, k2, kl, k1_levels);
  uint i = 1;
  uint links = 0;
  std::cout << "[BLD] Building tree #" << (i++) << "...";
  std::cout.flush();
  for (auto &triple : triples) {
    uint subject, predicate, object;
    subject = boost::get<0>(triple);
    predicate = boost::get<1>(triple);
    object = boost::get<2>(triple);

    if (curr_predicate != predicate) {
      if (curr_predicate != (uint)-1) {
        auto tree = builder.Build();
        auto compressed = tree->CompressLeaves();
        compressed->Save(&out);
        std::cout << "OK" << std::endl;
        std::cout.flush();
        std::clog << "[BLD] Tree #" << i - 1 << ": " << tree->links() << " links\n";
        links += tree->links();
        std::cout << "[BLD] Building tree #" << (i++) << "...";
        std::cout.flush();
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
    std::clog << "[BLD] Tree #" << i - 1 << ": " << tree->links() << " links\n";
    links += tree->links();
    std::cout << "OK" << std::endl;
  }
  std::clog << "[BLD] Total links: " << links << std::endl;
}

int main(int argc, char *argv[]) {
  ParseOps(argc, argv);
  Encode();
  Build();
  return 0;
}
