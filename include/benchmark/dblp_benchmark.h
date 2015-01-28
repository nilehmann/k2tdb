/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <engine/k2tdb_engine.h>
#include <engine/dictionary_encoding.h>
#include <boost/filesystem/path.hpp>
#include <boost/tokenizer.hpp>
float operator-(timespec t1, timespec t2);
namespace k2tdb {

namespace fs = ::boost::filesystem;
namespace re = regexp;
template<typename K2Tree>
class DBLPBenchmark {
 public:
  DBLPBenchmark(fs::path db_name)
      : k2tdb_in_(db_name.native() + ".k2tdb"),
        k2tdb_(&k2tdb_in_),
        SO_(),
        P_() {
    SO_.Open(db_name.native() + ".so");
    P_.Open(db_name.native() + ".p");
  }
      
  //Q1: Get Publications of author
  float Q1(std::string author) const {
    timespec t1, t2;
    uint id;
    auto e = P_.Encode(publications);
    if (not SO_.Encode(author, &id))
      return -1.0;
    clock_gettime(CLOCK_MONOTONIC, &t1);
    k2tdb_.Count(id, e);
    clock_gettime(CLOCK_MONOTONIC, &t2);

    return t2 - t1;
  }
  //Q2: Get coauthors
  float Q2(std::string author) const {
    timespec t1, t2;
    uint id;
    auto e = P_.Encode(coauthors);
    if (not SO_.Encode(author, &id))
      return -1.0;

    clock_gettime(CLOCK_MONOTONIC, &t1);
    k2tdb_.Count(id, e);
    clock_gettime(CLOCK_MONOTONIC, &t2);

    return t2 - t1;
  }
  //Q3: Get the coauthors transitive closure
  float Q3(std::string author1, std::string author2) const {
    timespec t1, t2;
    uint id1, id2;
    auto e = P_.Encode(coauthors_closure);
    if (not SO_.Encode(author1, &id1) or not SO_.Encode(author2, &id2))
      return -1.0;
    clock_gettime(CLOCK_MONOTONIC, &t1);
    if (not k2tdb_.ExistsPath(id1, id2, e))
      return -1.0;
    clock_gettime(CLOCK_MONOTONIC, &t2);

    return t2 - t1;
  }

  void execute_query(int query, int max_query_count,
                     const std::string &scenario) const {
    typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
    boost::char_separator<char> sep(" ");
    std::string line;
    std::vector<std::tuple<std::string, std::string, float>> results;
    float total_time = 0;

    std::ifstream in;
    std::ofstream out_back;

    if (query == 10)
      max_query_count = std::min(max_query_count, 10);

    switch (query) {
      case 1: case 2: in.open("test_data/authors_random"); break;
      case 3:
        in.open("test_data/authors_pairs");
        out_back.open("test_data/authors_pairs_back");
        break;
      default: exit(0);
    }


    std::cout << "Query type: " << query << "\n";
    int i = 0;
    int not_found = 0;
    while (std::getline(in, line) and i < max_query_count) {
      tokenizer tokens(line, sep);
      auto it = tokens.begin();

      std::string str1 = *it++;
      std::string str2;
      if (it != tokens.end())
        str2 = *it;

      float time;
      switch (query) {
        case 1: time = Q1(str1); break;
        case 2: time = Q2(str1); break;
        case 3: time = Q3(str1, str2); break;
      }
      if (time > 0) {
        total_time += time;
        results.emplace_back(str1, str2, time);
        if (query == 3)
          out_back << str1 << " " << str2 << "\n";
      } else {
        not_found++;
      }
      i++;
      if (i % 100 == 0)
        std::cout << i << " queries\n";
    }

    std::ofstream out("k2tdb/log/q" + std::to_string(query) + "." + scenario);

    int i_total_time = rint(total_time);
    out << "Total time (us): " << i_total_time << std::endl;
    out << "Total found: " << results.size() << std::endl;
    out << "Not found: " << not_found << std::endl;
    for (auto &result : results) {
      out << std::get<0>(result);
      if (std::get<1>(result) != "")
        out << "\t" << std::get<1>(result);
      out << "\t" << std::get<2>(result) << "\n";
    }
  }

 private:
  std::ifstream k2tdb_in_;
  engine::K2tdbEngine<K2Tree> k2tdb_;
  engine::DictionaryEncoding SO_;
  engine::DictionaryEncoding P_;
  static const re::RegExp<std::string> publications;
  static const re::RegExp<std::string> coauthors;
  static const re::RegExp<std::string> coauthors_closure;
};
template<typename K2Tree>
const re::RegExp<std::string>
DBLPBenchmark<K2Tree>::publications = 
    re::converse<std::string>("<http://purl.org/dc/elements/1.1/creator>");

template<typename K2Tree>
const re::RegExp<std::string>
DBLPBenchmark<K2Tree>::coauthors = 
    re::concat<std::string>(
      re::converse<std::string>("<http://purl.org/dc/elements/1.1/creator>"),
      "<http://purl.org/dc/elements/1.1/creator>");

template<typename K2Tree>
const re::RegExp<std::string>
DBLPBenchmark<K2Tree>::coauthors_closure =
  re::kleene<std::string>(
    re::concat<std::string>(
      re::converse<std::string>("<http://purl.org/dc/elements/1.1/creator>"),
      "<http://purl.org/dc/elements/1.1/creator>")
    );

}  // namespace k2tdb
