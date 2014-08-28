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

#include <boost/tokenizer.hpp>

#include <sys/time.h>



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

std::ofstream log_stream;

std::string in_file, out_base;
std::string format;
int k1, k2, kl, k1_levels;
int w;
int precision;

DictionaryEncoding SO;
DictionaryEncoding P_Dic;
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

void BuildFromTriplesArray() {
  std::sort(triples.begin(), triples.end(), tuple_compare<1>());

  timeval t1, t2;
  gettimeofday(&t1, NULL);
  std::cout << "[BLD] Building started\n";

  uint curr_predicate = -1;
  lk2::K2TreeBuilder builder(SO.Count(), k1, k2, kl, k1_levels);
  uint i = -1;
  uint links = 0;
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
        compressed->Save(&k2tdb_out);
        std::cout << "OK" << std::endl;
        std::cout.flush();
        log_stream << "[BLD] Tree for '" << P_Dic.Decode(i) << "': " << tree->links() << " links\n";
        links += tree->links();
      }
      curr_predicate = predicate;
      builder.Clear();
      i++;
      std::cout << "[BLD] Building tree for '" << P_Dic.Decode(i) << "'...";
      std::cout.flush();
    }
    builder.AddLink(subject, object);
  }
  if (curr_predicate != (uint)-1) {
    auto tree = builder.Build();
    auto compressed = tree->CompressLeaves();
    compressed->Save(&k2tdb_out);
    log_stream << "[BLD] Tree for '" << P_Dic.Decode(i) << ": " << tree->links() << "' links\n";
    links += tree->links();
    std::cout << "OK" << std::endl;
  }
  gettimeofday(&t2, NULL);
  auto time = t2.tv_sec - t1.tv_sec;
  time += (t2.tv_usec - t1.tv_usec)*1000000;

  log_stream << "[BLD] Time to build: " << time << " s" << std::endl;
  log_stream << "[BLD] Total links: " << links << std::endl;
}
void EncodeNT() {
  timeval t1, t2;
  gettimeofday(&t1, NULL);

  std::cout << "[ENC] Encoding started\n";

  //log_stream << STR((SUBJECT)WS+(PREDICATE)WS+(OBJECT)WS+\\.) << std::endl;
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

      //log_stream << "|" << subject << "| |" << predicate << "| |" << object << "|\n";

      SO.Add(subject);
      SO.Add(object);
      P_Dic.Add(predicate);

      uint isubject, ipredicate, iobject;
      SO.Encode(subject, &isubject);
      SO.Encode(object, &iobject);
      P_Dic.Encode(predicate, &ipredicate);

      triples.emplace_back(isubject, ipredicate, iobject);
      if (triples.size() % 1000000 == 0)
        std::cout << "[ENC] Triples encoded: " << triples.size() << std::endl;
    } else {
      log_stream << "[ENC] No matched line: " << line << std::endl;
    }
    nlines++;
  }
  gettimeofday(&t2, NULL);
  auto time = t2.tv_sec - t1.tv_sec;
  time += (t2.tv_usec - t1.tv_usec)*1000000;

  log_stream << "[ENC] Time to encoded: " << time << " s" << std::endl;
  log_stream << "[ENC] Triples encoded: " << triples.size() << std::endl;
  log_stream << "[ENC] Lines readed: " << nlines << std::endl;
  log_stream << "[ENC] Predicates: " << P_Dic.Count() << std::endl;
  log_stream << "[ENC] Subjects + Objects: " << SO.Count() << std::endl;
}

void ReadPeople(std::ifstream &in) {
  typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
  boost::char_separator<char> sep("-");

  P_Dic.Add("name");
  P_Dic.Add("age");
  P_Dic.Add("location");

  uint name_pred, age_pred, loc_pred;
  P_Dic.Encode("name", &name_pred);
  P_Dic.Encode("age", &age_pred);
  P_Dic.Encode("location", &loc_pred);

  std::string line;
  uint i = 1;
  while (std::getline(in, line)) {
    if (line == "</PEOPLE>")
      break;
    tokenizer tokens(line, sep);
    auto it = tokens.begin();
    std::string id = *it++;
    std::string name = *it++;
    std::string age = *it++;
    std::string location = *it;

    uint id_enc, name_enc, age_enc, loc_enc;

    SO.Add("person" + id);
    SO.Encode("person" + id, &id_enc);

    if (name != "?") {
      SO.Add(name);
      SO.Encode(name, &name_enc);
      triples.emplace_back(id_enc, name_pred, name_enc);
    }

    if (age != "?") {
      SO.Add(age);
      SO.Encode(age, &age_enc);
      triples.emplace_back(id_enc, age_pred, age_enc);
    }

    if (location != "?") {
      SO.Add(location);
      SO.Encode(location, &loc_enc);
      triples.emplace_back(id_enc, loc_pred, loc_enc);
    }
    if (i % 1000000 == 0)
      std::cout << "[ENC] " << i << " people encoded.\n";
    i++;
  }
  log_stream << "[ENC] People encoded: " << i-1 << std::endl;
}

void ReadWebpages(std::ifstream &in) {
  typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
  boost::char_separator<char> sep("-");

  P_Dic.Add("url");
  P_Dic.Add("creation");

  uint url_pred, creation_pred;
  P_Dic.Encode("url", &url_pred);
  P_Dic.Encode("creation", &creation_pred);

  std::string line;
  uint i = 0;
  while (std::getline(in, line)) {
    if (line == "</WEBPAGES>")
      break;
    tokenizer tokens(line, sep);
    auto it = tokens.begin();
    std::string id = *it++;
    std::string creation = *it;

    uint id_enc, url_enc, creation_enc;

    SO.Add("webpage" + id);
    SO.Encode("webpage" + id, &id_enc);

    if (creation != "?") {
      SO.Add(creation);
      SO.Encode(creation, &creation_enc);
      triples.emplace_back(id_enc, creation_pred, creation_enc);
    }

    SO.Add("http://www.site.org/webpage" + id + ".html");
    SO.Encode("http://www.site.org/webpage" + id + ".html", &url_enc);
    triples.emplace_back(id_enc, url_pred, url_enc);

    i++;
    if (i % 1000000 == 0)
      std::cout << "[ENC] " << i << " webpages encoded.\n";
  }
  log_stream << "[ENC] Webpages encoded: " << i-1 << std::endl;
}
void ReadFriends(std::ifstream &in) {
  typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
  boost::char_separator<char> sep("-");

  lk2::K2TreeBuilder builder(SO.Count(), k1, k2, kl, k1_levels);

  uint friend_pred;
  P_Dic.Add("friend");
  P_Dic.Encode("friend", &friend_pred);

  std::cout << "[BLD] Building tree for 'friend'\n";
  std::string line;
  uint i = 0;
  while (std::getline(in, line)) {
    if (line == "</FRIENDS>")
      break;
    tokenizer tokens(line, sep);
    auto it = tokens.begin();
    std::string eid = *it++;
    std::string from = *it++;
    std::string to = *it;

    uint from_enc, to_enc;

    SO.Encode("person"+from, &from_enc);
    SO.Encode("person"+to, &to_enc);

    builder.AddLink(from_enc, to_enc);
    builder.AddLink(to_enc, from_enc);

    i++;
    if (i % 1000000 == 0)
      std::cout << "[READ] " << i << " friends readed.\n";
  }
  auto tree = builder.Build();
  auto compressed = tree->CompressLeaves();
  compressed->Save(&k2tdb_out);
  log_stream << "[READ] Friend readed: " << i << std::endl;
  log_stream << "[BLD] Tree for 'friend': " << tree->links() << " links\n";
}
void ReadLikes(std::ifstream &in) {
  typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
  boost::char_separator<char> sep("-");

  lk2::K2TreeBuilder builder(SO.Count(), k1, k2, kl, k1_levels);
  P_Dic.Add("like");

  std::cout << "[BLD] Building tree for 'like'\n";
  std::string line;
  uint i = 0;
  while (std::getline(in, line)) {
    if (line == "</LIKES>")
      break;
    tokenizer tokens(line, sep);
    auto it = tokens.begin();
    std::string eid = *it++;
    std::string from = *it++;
    std::string to = *it;

    uint from_enc, to_enc;
    SO.Encode("person"+from, &from_enc);
    SO.Encode("webpage"+to, &to_enc);

    builder.AddLink(from_enc, to_enc);

    i++;
    if (i % 1000000 == 0)
      std::cout << "[READ] " << i << " likes readed.\n";
  }
  auto tree = builder.Build();
  auto compressed = tree->CompressLeaves();
  compressed->Save(&k2tdb_out);
  log_stream << "[READ] Likes readed: " << i << std::endl;
  log_stream << "[BLD] Tree for 'likes': " << tree->links() << " links\n";
}


void EncodeAndBuildGD() {
  timeval t1, t2;
  gettimeofday(&t1, NULL);

  std::ifstream in(in_file, std::ifstream::in);
  std::string line;

  std::cout << "[ENC] Encoding Started\n";
  std::getline(in, line);
  if (line != "<PEOPLE>") {
    std::cerr << "Expecting People block\n";
    exit(1);
  }
  ReadPeople(in);

  std::getline(in, line);
  if (line != "<WEBPAGES>") {
    std::cerr << "Expecting webpages block\n";
    exit(1);
  }
  ReadWebpages(in);
  uint npredicates = P_Dic.Count() + 2;
  k2tdb_out.write(reinterpret_cast<char*>(&npredicates), sizeof(uint));
  BuildFromTriplesArray();

  triples.clear();

  std::getline(in, line);
  if (line != "<FRIENDS>") {
    std::cerr << "Expecting friends block\n";
    exit(1);
  }
  ReadFriends(in);

  std::getline(in, line);
  if (line != "<LIKES>"){
    std::cerr << "Expecting likes block\n";
    exit(1);
  }
  ReadLikes(in);

  gettimeofday(&t2, NULL);
  auto time = t2.tv_sec - t1.tv_sec;
  time += (t2.tv_usec - t1.tv_usec)*1000000;

  log_stream << " Total Time: " << time << " s" << std::endl;
}


int main(int argc, char *argv[]) {
  std::ofstream a;
  ParseOps(argc, argv);
  log_stream.open(out_base + ".log");
  SO.Create(out_base + ".so", w);
  P_Dic.Create(out_base + ".p", 10000);
  k2tdb_out.open(out_base + ".k2tdb");
  if (format == "gd") {
    EncodeAndBuildGD();
  } else if (format == "nt") {
    EncodeNT();
    uint npredicates = P_Dic.Count();
    k2tdb_out.write(reinterpret_cast<char*>(&npredicates), sizeof(uint));
    BuildFromTriplesArray();
  } else {
    std::cout << "Error: format not accepted" << std::endl;
  }

  k2tdb_out.close();
  return 0;
}
