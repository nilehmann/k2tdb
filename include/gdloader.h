/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_GDLOADER_H_
#define INCLUDE_GDLOADER_H_

#include <k2tree.h>
#include <engine/dictionary_encoding.h>
#include <vector>
#include <tuple>
#include <sys/time.h>

#include <boost/tokenizer.hpp>

namespace k2tdb {
template<int N>
struct tuple_compare {
  bool operator()(const std::tuple<uint, uint, uint> &a,
                  const std::tuple<uint, uint, uint> &b) {
    return std::get<N>(a) < std::get<N>(b);
  }
};
namespace fs = ::boost::filesystem;
namespace lk2 = libk2tree;
template<typename K2Tree>
class GDLoader {
 public:
  GDLoader(fs::path db_name, uint expected,
           uint k1, uint k2, uint kl, uint k1_levels)
      : db_name_(db_name),
        log_stream_(db_name_.native() + ".log"),
        k2tdb_out_(db_name_.native() + ".k2tdb"),
        k1_(k1), k2_(k2), kl_(kl), k1_levels_(k1_levels) {
    SO_.Create(db_name_.native() + ".so", expected);
    P_.Create(db_name_.native() + ".p", 10000);

  }
  void EncodeAndBuild(std::ifstream *in) {
    timeval t1, t2;
    gettimeofday(&t1, NULL);

    std::string line;

    std::cout << "[ENC] Encoding Started\n";
    std::getline(*in, line);
    if (line != "<PEOPLE>") {
      std::cerr << "Expecting People block\n";
      exit(1);
    }
    ReadPeople(in);

    std::getline(*in, line);
    if (line != "<WEBPAGES>") {
      std::cerr << "Expecting webpages block\n";
      exit(1);
    }
    ReadWebpages(in);
    uint npredicates = P_.Count() + 2;
    k2tdb_out_.write(reinterpret_cast<char*>(&npredicates), sizeof(uint));
    BuildFromTriplesArray();

    triples.clear();

    std::getline(*in, line);
    if (line != "<FRIENDS>") {
      std::cerr << "Expecting friends block\n";
      exit(1);
    }
    ReadFriends(in);

    std::getline(*in, line);
    if (line != "<LIKES>"){
      std::cerr << "Expecting likes block\n";
      exit(1);
    }
    ReadLikes(in);

    gettimeofday(&t2, NULL);
    auto time = (double)(t2.tv_sec - t1.tv_sec);
    time += (double)(t2.tv_usec - t1.tv_usec) * 0.000001;

    log_stream_ << " Total Time: " << time << " s" << std::endl;
  }
 private:
  fs::path db_name_;
  engine::DictionaryEncoding SO_;
  engine::DictionaryEncoding P_;
  std::vector<std::tuple<uint, uint, uint>> triples;
  std::ofstream log_stream_;
  std::ofstream k2tdb_out_;
  uint k1_, k2_, kl_, k1_levels_;

  void ReadPeople(std::ifstream *in) {
    typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
    boost::char_separator<char> sep("-");

    P_.Add("name");
    P_.Add("age");
    P_.Add("location");

    uint name_pred, age_pred, loc_pred;
    P_.Encode("name", &name_pred);
    P_.Encode("age", &age_pred);
    P_.Encode("location", &loc_pred);

    std::string line;
    uint i = 1;
    while (std::getline(*in, line)) {
      if (line == "</PEOPLE>")
        break;
      tokenizer tokens(line, sep);
      auto it = tokens.begin();
      std::string id = *it++;
      std::string name = *it++;
      std::string age = *it++;
      std::string location = *it;

      uint id_enc, name_enc, age_enc, loc_enc;

      SO_.Add("person" + id);
      SO_.Encode("person" + id, &id_enc);

      if (name != "?") {
        SO_.Add(name);
        SO_.Encode(name, &name_enc);
        triples.emplace_back(id_enc, name_pred, name_enc);
      }

      if (age != "?") {
        SO_.Add(age);
        SO_.Encode(age, &age_enc);
        triples.emplace_back(id_enc, age_pred, age_enc);
      }

      if (location != "?") {
        SO_.Add(location);
        SO_.Encode(location, &loc_enc);
        triples.emplace_back(id_enc, loc_pred, loc_enc);
      }
      if (i % 1000000 == 0)
        std::cout << "[ENC] " << i << " people encoded.\n";
      i++;
    }
    log_stream_ << "[ENC] People encoded: " << i-1 << std::endl;
  }
  void ReadWebpages(std::ifstream *in) {
    typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
    boost::char_separator<char> sep("-");

    P_.Add("url");
    P_.Add("creation");

    uint url_pred, creation_pred;
    P_.Encode("url", &url_pred);
    P_.Encode("creation", &creation_pred);

    std::string line;
    uint i = 0;
    while (std::getline(*in, line)) {
      if (line == "</WEBPAGES>")
        break;
      tokenizer tokens(line, sep);
      auto it = tokens.begin();
      std::string id = *it++;
      std::string creation = *it;

      uint id_enc, url_enc, creation_enc;

      SO_.Add("webpage" + id);
      SO_.Encode("webpage" + id, &id_enc);

      if (creation != "?") {
        SO_.Add(creation);
        SO_.Encode(creation, &creation_enc);
        triples.emplace_back(id_enc, creation_pred, creation_enc);
      }

      SO_.Add("http://www.site.org/webpage" + id + ".html");
      SO_.Encode("http://www.site.org/webpage" + id + ".html", &url_enc);
      triples.emplace_back(id_enc, url_pred, url_enc);

      i++;
      if (i % 1000000 == 0)
        std::cout << "[ENC] " << i << " webpages encoded.\n";
    }
    log_stream_ << "[ENC] Webpages encoded: " << i-1 << std::endl;
  }
  void ReadFriends(std::ifstream *in) {
    typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
    boost::char_separator<char> sep("-");

    lk2::K2TreeBuilder builder(SO_.Count(), k1_, k2_, kl_, k1_levels_);

    uint friend_pred;
    P_.Add("friend");
    P_.Encode("friend", &friend_pred);

    std::cout << "[BLD] Building tree for 'friend'\n";
    std::string line;
    uint i = 0;
    while (std::getline(*in, line)) {
      if (line == "</FRIENDS>")
        break;
      tokenizer tokens(line, sep);
      auto it = tokens.begin();
      std::string eid = *it++;
      std::string from = *it++;
      std::string to = *it;

      uint from_enc, to_enc;

      SO_.Encode("person"+from, &from_enc);
      SO_.Encode("person"+to, &to_enc);

      builder.AddLink(from_enc, to_enc);
      builder.AddLink(to_enc, from_enc);

      i++;
      if (i % 1000000 == 0)
        std::cout << "[READ] " << i << " friends readed.\n";
    }
    auto tree = builder.Build();
    auto compressed = tree->CompressLeaves();
    compressed->Save(&k2tdb_out_);
    log_stream_ << "[READ] Friend readed: " << i << std::endl;
    log_stream_ << "[BLD] Tree for 'friend': " << tree->links() << " links\n";
  }
  void ReadLikes(std::ifstream *in) {
    typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
    boost::char_separator<char> sep("-");

    lk2::K2TreeBuilder builder(SO_.Count(), k1_, k2_, kl_, k1_levels_);
    P_.Add("like");

    std::cout << "[BLD] Building tree for 'like'\n";
    std::string line;
    uint i = 0;
    while (std::getline(*in, line)) {
      if (line == "</LIKES>")
        break;
      tokenizer tokens(line, sep);
      auto it = tokens.begin();
      std::string eid = *it++;
      std::string from = *it++;
      std::string to = *it;

      uint from_enc, to_enc;
      SO_.Encode("person"+from, &from_enc);
      SO_.Encode("webpage"+to, &to_enc);

      builder.AddLink(from_enc, to_enc);

      i++;
      if (i % 1000000 == 0)
        std::cout << "[READ] " << i << " likes readed.\n";
    }
    auto tree = builder.Build();
    auto compressed = tree->CompressLeaves();
    compressed->Save(&k2tdb_out_);
    log_stream_ << "[READ] Likes readed: " << i << std::endl;
    log_stream_ << "[BLD] Tree for 'likes': " << tree->links() << " links\n";
  }
  void BuildFromTriplesArray() {
    std::sort(triples.begin(), triples.end(), tuple_compare<1>());

    timeval t1, t2;
    gettimeofday(&t1, NULL);
    std::cout << "[BLD] Building started\n";

    uint curr_predicate = -1;
    lk2::K2TreeBuilder builder(SO_.Count(), k1_, k2_, kl_, k1_levels_);
    uint i = -1;
    uint links = 0;
    std::cout.flush();
    for (auto &triple : triples) {
      uint subject, predicate, object;
      subject = std::get<0>(triple);
      predicate = std::get<1>(triple);
      object = std::get<2>(triple);

      if (curr_predicate != predicate) {
        if (curr_predicate != (uint)-1) {
          auto tree = builder.Build();
          auto compressed = tree->CompressLeaves();
          compressed->Save(&k2tdb_out_);
          std::cout << "OK" << std::endl;
          std::cout.flush();
          log_stream_ << "[BLD] Tree for '" << P_.Decode(i) << "': " << tree->links() << " links\n";
          links += tree->links();
        }
        curr_predicate = predicate;
        builder.Clear();
        i++;
        std::cout << "[BLD] Building tree for '" << P_.Decode(i) << "'...";
        std::cout.flush();
      }
      builder.AddLink(subject, object);
    }
    if (curr_predicate != (uint)-1) {
      auto tree = builder.Build();
      auto compressed = tree->CompressLeaves();
      compressed->Save(&k2tdb_out_);
      log_stream_ << "[BLD] Tree for '" << P_.Decode(i) << ": " << tree->links() << "' links\n";
      links += tree->links();
      std::cout << "OK" << std::endl;
    }
    gettimeofday(&t2, NULL);
    auto time = (double)(t2.tv_sec - t1.tv_sec);
    time += (double)(t2.tv_usec - t1.tv_usec)*0.000001;

    log_stream_ << "[BLD] Time to build: " << time << " s" << std::endl;
    log_stream_ << "[BLD] Total links: " << links << std::endl;
  }
};
}  // namespace k2tdb
#endif  // INCLUDE_GDLOADER_H_
