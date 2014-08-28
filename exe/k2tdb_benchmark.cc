/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <regexp.h>
#include <engine/graph_db.h>

#include <boost/tokenizer.hpp>

using engine::GraphDB;
namespace re = regexp;



timespec t1, t2;
re::RegExp<std::string> converse_name(re::converse<std::string>("name"));
re::RegExp<std::string> converse_like(re::converse<std::string>("like"));
re::RegExp<std::string> like("like");
re::RegExp<std::string> name("name");
re::concat<std::string> friends_of_friends("friend", "friend");
re::concat<std::string> likes_of_friends("friend", "like");
re::concat<std::string> likedby_of_likes("like", re::converse<std::string>("like"));
re::RegExp<std::string> friend_path(re::kleene<std::string>("friend"));
re::RegExp<std::string> friends("friend");


float operator-(timespec t1, timespec t2) {
  float us = (t1.tv_sec - t2.tv_sec)*1000000;
  us += (float) (t1.tv_nsec - t2.tv_nsec) / 1000.0;
  return us;
}


//Q1: Get people having name N
float Q1(const GraphDB &db, std::string name) {
  auto e = db.Encode(converse_name);
  uint id = db.Encode(name);
  clock_gettime(CLOCK_MONOTONIC, &t1);
  db.Count(id, e);
  clock_gettime(CLOCK_MONOTONIC, &t2);

  return t2 - t1;
}
//Q2: Get people that likes a given Web page W
float Q2(const GraphDB &db, std::string webpage) {
  auto e = db.Encode(converse_like);
  uint id = db.Encode(webpage);
  clock_gettime(CLOCK_MONOTONIC, &t1);
  db.Count(id, e);
  clock_gettime(CLOCK_MONOTONIC, &t2);

  return t2 - t1;
}
//Q3: Get the Web pages that person P likes
float Q3(const GraphDB &db, std::string person) {
  auto e = db.Encode(like);
  uint id = db.Encode(person);
  clock_gettime(CLOCK_MONOTONIC, &t1);
  db.Count(id, e);
  clock_gettime(CLOCK_MONOTONIC, &t2);

  return t2 - t1;
}
//Q4: Get the name of the person with a given ID
float Q4(const GraphDB &db, std::string person) {
  auto e = db.Encode(name);
  uint id = db.Encode(person);
  clock_gettime(CLOCK_MONOTONIC, &t1);
  db.Count(id, e);
  clock_gettime(CLOCK_MONOTONIC, &t2);

  return t2 - t1;
}
//Q5: Get the friends of the friends of a given person P
float Q5(const GraphDB &db, std::string person) {
  auto e = db.Encode(friends_of_friends);
  uint id = db.Encode(person);
  clock_gettime(CLOCK_MONOTONIC, &t1);
  db.Count(id, e);
  clock_gettime(CLOCK_MONOTONIC, &t2);

  return t2 - t1;
}

//Q6: Get the Web pages liked by the friends of a given person P
float Q6(const GraphDB &db, std::string person) {
  auto e = db.Encode(likes_of_friends);
  uint id = db.Encode(person);
  clock_gettime(CLOCK_MONOTONIC, &t1);
  db.Count(id, e);
  clock_gettime(CLOCK_MONOTONIC, &t2);

  return t2 - t1;
}
//Q7: Get people that likes a Web page which a person P likes
float Q7(const GraphDB &db, std::string person) {
  auto e = db.Encode(likedby_of_likes);
  uint id = db.Encode(person);
  clock_gettime(CLOCK_MONOTONIC, &t1);
  db.Count(id, e);
  clock_gettime(CLOCK_MONOTONIC, &t2);

  return t2 - t1;
}
//Q8: Is there a connection (path) between people P1 and P2?
float Q8(const GraphDB &db, std::string person1, std::string person2) {
  auto e = db.Encode(friend_path);
  uint id1 = db.Encode(person1);
  uint id2 = db.Encode(person2);
  clock_gettime(CLOCK_MONOTONIC, &t1);
  db.ExistsPath(id1, id2, e);
  clock_gettime(CLOCK_MONOTONIC, &t2);

  return t2 - t1;
}

//Q9: Get the number of friends of a person P
float Q9(const GraphDB &db, std::string person) {
  auto e = db.Encode(friends);
  uint id = db.Encode(person);
  clock_gettime(CLOCK_MONOTONIC, &t1);
  db.Count(id, e);
  clock_gettime(CLOCK_MONOTONIC, &t2);

  return t2 - t1;
}
//Q10: Get the number of person connected with a person P by a path
float Q10(const GraphDB &db, std::string person) {
  auto e = db.Encode(friend_path);
  uint id = db.Encode(person);
  clock_gettime(CLOCK_MONOTONIC, &t1);
  db.Count(id, e);
  clock_gettime(CLOCK_MONOTONIC, &t2);

  return t2 - t1;
}

int main(int argc, char *argv[]) {
  typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
  boost::char_separator<char> sep("-");

  if (argc < 3) {
    std::cout << "Usage: " << argv[0] << " base_path query_numer\n";
    return 0;
  }

  GraphDB gdb(argv[1]);
  uint q = std::stoi(argv[2]);

  std::string line;
  std::vector<std::tuple<std::string, std::string, float>> results;
  float total_time = 0;

  std::ifstream in;

  switch (q) {
    case 1: in.open("test_data/people_names"); break;
    case 2: in.open("test_data/webpage_ids"); break;
    case 3: case 4: case 5: case 6: case 7: case 9: case 10:
            in.open("test_data/people_ids"); break;
    case 8: in.open("test_data/path_ids"); break;
  }


  while (std::getline(in, line)) {
    tokenizer tokens(line, sep);
    auto it = tokens.begin();

    std::string str1 = *it++;
    std::string str2;
    if (it != tokens.end())
      str2 = *it;

    float time;
    switch (q) {
      case 1: time = Q1(gdb, str1); break;
      case 2: time = Q2(gdb, "webpage" + str1); break;
      case 3: time = Q3(gdb, "person" + str1); break;
      case 4: time = Q4(gdb, "person" + str1); break;
      case 5: time = Q5(gdb, "person" + str1); break;
      case 6: time = Q6(gdb, "person" + str1); break;
      case 7: time = Q7(gdb, "person" + str1); break;
      case 8: time = Q8(gdb, "person" + str1, "person" + str2); break;
      case 9: time = Q9(gdb, str1); break;
      case 10: time = Q10(gdb, str1); break;
    }
    total_time += time;
    results.emplace_back(str1, str2, time);
  }

  std::cout << "Total time (us): " << total_time << std::endl;
  for (auto &result : results) {
    std::cout << std::get<0>(result);
    if (std::get<1>(result) != "")
      std::cout << "\t" << std::get<1>(result);
    std::cout << "\t" << std::get<2>(result) << "\n";
  }

  return 0;
}

