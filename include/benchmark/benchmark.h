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

namespace k2tdb {

namespace {
float operator-(timespec t1, timespec t2) {
  float us = (t1.tv_sec - t2.tv_sec)*1000000;
  us += (float) (t1.tv_nsec - t2.tv_nsec) / 1000.0;
  return us;
} timespec t1, t2;
}

namespace fs = ::boost::filesystem;
namespace re = regexp;
template<typename K2Tree>
class Benchmark {
 public:
  Benchmark(fs::path db_name)
      : k2tdb_in_(db_name.native() + ".k2tdb"),
        k2tdb_(&k2tdb_in_),
        SO_(),
        P_() {
    SO_.Open(db_name.native() + ".so");
    P_.Open(db_name.native() + ".p");
  }
      
  //Q1: Get people having name N
  float Q1(std::string name) const {
    uint id;
    auto e = P_.Encode(converse_name);
    id = SO_.Encode(name, &id);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    k2tdb_.Count(id, e);
    clock_gettime(CLOCK_MONOTONIC, &t2);

    return t2 - t1;
  }
  //Q2: Get people that likes a given Web page W
  float Q2(std::string webpage) const {
    uint id;
    auto e = P_.Encode(converse_like);
    SO_.Encode(webpage, &id);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    k2tdb_.Count(id, e);
    clock_gettime(CLOCK_MONOTONIC, &t2);

    return t2 - t1;
  }
  //Q3: Get the Web pages that person P likes
  float Q3(std::string person) const {
    uint id;
    auto e = P_.Encode(like);
    SO_.Encode(person, &id);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    k2tdb_.Count(id, e);
    clock_gettime(CLOCK_MONOTONIC, &t2);

    return t2 - t1;
  }
  //Q4: Get the name of the person with a given ID
  float Q4(std::string person) const {
    uint  id;
    auto e = P_.Encode(name);
    SO_.Encode(person, &id);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    k2tdb_.Count(id, e);
    clock_gettime(CLOCK_MONOTONIC, &t2);

    return t2 - t1;
  }
  //Q5: Get the friends of the friends of a given person P
  float Q5(std::string person) const {
    uint id;
    auto e = P_.Encode(friends_of_friends);
    SO_.Encode(person, &id);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    k2tdb_.Count(id, e);
    clock_gettime(CLOCK_MONOTONIC, &t2);

    return t2 - t1;
  }

  //Q6: Get the Web pages liked by the friends of a given person P
  float Q6(std::string person) const {
    uint id;
    auto e = P_.Encode(likes_of_friends);
    SO_.Encode(person, &id);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    k2tdb_.Count(id, e);
    clock_gettime(CLOCK_MONOTONIC, &t2);

    return t2 - t1;
  }
  //Q7: Get people that likes a Web page which a person P likes
  float Q7(std::string person) const {
    uint id;
    auto e = P_.Encode(likedby_of_likes);
    SO_.Encode(person, &id);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    k2tdb_.Count(id, e);
    clock_gettime(CLOCK_MONOTONIC, &t2);

    return t2 - t1;
  }
  //Q8: Is there a connection (path) between people P1 and P2?
  float Q8(std::string person1, std::string person2) const {
    uint id1, id2;
    auto e = P_.Encode(friend_path);
    SO_.Encode(person1, &id1);
    SO_.Encode(person2, &id2);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    k2tdb_.ExistsPath(id1, id2, e);
    clock_gettime(CLOCK_MONOTONIC, &t2);

    return t2 - t1;
  }

  //Q9: Get the number of friends of a person P
  float Q9(std::string person) const {
    uint id;
    auto e = P_.Encode(friends);
    SO_.Encode(person, &id);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    k2tdb_.Count(id, e);
    clock_gettime(CLOCK_MONOTONIC, &t2);

    return t2 - t1;
  }
  //Q10: Get the number of person connected with a person P by a path
  float Q10(std::string person) const {
    uint id;
    auto e = P_.Encode(friend_path);
    SO_.Encode(person, &id);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    k2tdb_.Count(id, e);
    clock_gettime(CLOCK_MONOTONIC, &t2);

    return t2 - t1;
  }

  void execute_query(int query, int max_query_count,
                     std::string scenario) const {
    typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
    boost::char_separator<char> sep("-");
    std::string line;
    std::vector<std::tuple<std::string, std::string, float>> results;
    float total_time = 0;

    std::ifstream in;

    if (query == 10)
      max_query_count = std::min(max_query_count, 10);

    switch (query) {
      case 1: in.open("test_data/people_names"); break;
      case 2: in.open("test_data/webpage_ids"); break;
      case 3: case 4: case 5: case 6: case 7: case 9: case 10:
              in.open("test_data/people_ids"); break;
      case 8: in.open("test_data/path_ids"); break;
    }


    std::cout << "Query type: " << query << "\n";
    int i = 0;
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
        case 2: time = Q2("webpage" + str1); break;
        case 3: time = Q3("person" + str1); break;
        case 4: time = Q4("person" + str1); break;
        case 5: time = Q5("person" + str1); break;
        case 6: time = Q6("person" + str1); break;
        case 7: time = Q7("person" + str1); break;
        case 8: time = Q8("person" + str1, "person" + str2); break;
        case 9: time = Q9("person" + str1); break;
        case 10: time = Q10("person" + str1); break;
      }
      total_time += time;
      results.emplace_back(str1, str2, time);
      i++;
      if (i % 100 == 0)
        std::cout << i << " queries\n";
    }

    std::ofstream out("k2tdb/log/q" + std::to_string(query) + "." + scenario);

    int i_total_time = rint(total_time);
    out << "Total time (us): " << i_total_time << std::endl;
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


  static const re::RegExp<std::string> converse_name;
  static const re::RegExp<std::string> converse_like;
  static const re::RegExp<std::string> like;
  static const re::RegExp<std::string> name;
  static const re::concat<std::string> friends_of_friends;
  static const re::concat<std::string> likes_of_friends;
  static const re::concat<std::string> likedby_of_likes;
  static const re::RegExp<std::string> friend_path;
  static const re::RegExp<std::string> friends;
};
template<typename K2Tree>
const re::RegExp<std::string>
Benchmark<K2Tree>::converse_name(re::converse<std::string>("name"));

template<typename K2Tree>
const re::RegExp<std::string>
Benchmark<K2Tree>::converse_like(re::converse<std::string>("like"));

template<typename K2Tree>
const re::RegExp<std::string>
Benchmark<K2Tree>::like("like");

template<typename K2Tree>
const re::RegExp<std::string>
Benchmark<K2Tree>::name("name");

template<typename K2Tree>
const re::concat<std::string>
Benchmark<K2Tree>::friends_of_friends("friend", "friend");

template<typename K2Tree>
const re::concat<std::string>
Benchmark<K2Tree>::likes_of_friends("friend", "like");

template<typename K2Tree>
const re::concat<std::string>
Benchmark<K2Tree>::likedby_of_likes("like", re::converse<std::string>("like"));

template<typename K2Tree>
const re::RegExp<std::string>
Benchmark<K2Tree>::friend_path(re::kleene<std::string>("friend"));

template<typename K2Tree>
const re::RegExp<std::string>
Benchmark<K2Tree>::friends("friend");

}  // namespace k2tdb
