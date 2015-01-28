/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_ENGINE_NTLOADER_H_
#define INCLUDE_ENGINE_NTLOADER_H_

#include <regex>
#include <tuple>
#include <engine/dictionary_encoding.h>
#include <boost/filesystem/path.hpp>
#include <sys/time.h>
#include <k2tree.h>

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

namespace k2tdb {

namespace fs = ::boost::filesystem;
namespace lk2 = libk2tree;

template<typename K2Tree>
class NTLoader {
 public:
  NTLoader(fs::path db_name): db_name_(db_name) {
    log_stream.open(db_name.native() + ".log");
  }
  template<typename Fun>
  void Encode(std::ifstream *in, uint expected,
      Fun fun = [] (uint, uint, uint) {}) {
    engine::DictionaryEncoding SO(db_name_.native() + ".so", expected);
    engine::DictionaryEncoding P(db_name_.native() + ".p", expected);

    Read(in, [&] (std::string subject,
                  std::string object,
                  std::string predicate) {
      SO.Add(subject);
      SO.Add(object);
      P.Add(predicate);

      uint isubject, ipredicate, iobject;
      SO.Encode(subject, &isubject);
      SO.Encode(object, &iobject);
      P.Encode(predicate, &ipredicate);
      fun(isubject, ipredicate, iobject);
    });
    log_stream << "[ENC] Predicates: " << P.Count() << std::endl;
    log_stream << "[ENC] Subjects + Objects: " << SO.Count() << std::endl;
  }

  void EncodeAndBuild(std::ifstream *in, uint expected, 
                      uint k1, uint k2, uint kl, uint k1_levels) {
    engine::DictionaryEncoding SO(db_name_.native() + ".so", expected);
    engine::DictionaryEncoding P(db_name_.native() + ".p", expected);

    Read(in, [&] (std::string subject,
                  std::string object,
                  std::string predicate) {
      SO.Add(subject);
      SO.Add(object);
      P.Add(predicate);

      uint isubject, ipredicate, iobject;
      SO.Encode(subject, &isubject);
      SO.Encode(object, &iobject);
      P.Encode(predicate, &ipredicate);


      triples.emplace_back(isubject, ipredicate, iobject);
    });
    log_stream << "[ENC] Predicates: " << P.Count() << std::endl;
    log_stream << "[ENC] Subjects + Objects: " << SO.Count() << std::endl;

    BuildK2Trees(k1, k2, kl, k1_levels);
  }

  void Build(std::ifstream *in, uint k1, uint k2, uint kl, uint k1_levels) {
    engine::DictionaryEncoding SO(db_name_.native() + ".so");
    engine::DictionaryEncoding P(db_name_.native() + ".p");

    triples.reserve(70000000);

    Read(in, [&] (std::string subject,
                  std::string object,
                  std::string predicate) {
      uint isubject, ipredicate, iobject;
      SO.Encode(subject, &isubject);
      SO.Encode(object, &iobject);
      P.Encode(predicate, &ipredicate);
      triples.emplace_back(isubject, ipredicate, iobject);
    });
    BuildK2Trees(k1, k2, kl, k1_levels);
  }

 private:
  fs::path db_name_;
  std::vector<std::tuple<uint, uint, uint>> triples;
  std::ofstream log_stream;

  void BuildK2Trees(uint k1, uint k2, uint kl, uint k1_levels) {
    engine::DictionaryEncoding SO(db_name_.native() + ".so");
    engine::DictionaryEncoding P(db_name_.native() + ".p");
    std::ofstream k2tdb_out(db_name_.native() + ".k2tdb");

    uint npredicates = P.Count();
    k2tdb_out.write(reinterpret_cast<char*>(&npredicates), sizeof(uint));

    std::sort(triples.begin(), triples.end(),
              [](const std::tuple<uint, uint, uint> &a,
                 const std::tuple<uint, uint, uint> &b) {
      return std::get<1>(a) < std::get<1>(b);
    });

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
      subject = std::get<0>(triple);
      predicate = std::get<1>(triple);
      object = std::get<2>(triple);

      if (curr_predicate != predicate) {
        if (curr_predicate != (uint)-1) {
          auto tree = builder.Build();
          auto compressed = tree->CompressLeaves();
          compressed->Save(&k2tdb_out);
          std::cout << "OK" << std::endl;
          std::cout.flush();
          log_stream << "[BLD] Tree for '" << P.Decode(i) << "': " << tree->links() << " links\n";
          links += tree->links();
        }
        curr_predicate = predicate;
        builder.Clear();
        i++;
        std::cout << "[BLD] Building tree for '" << P.Decode(i) << "'...";
        std::cout.flush();
      }
      builder.AddLink(subject, object);
    }
    if (curr_predicate != (uint)-1) {
      auto tree = builder.Build();
      auto compressed = tree->CompressLeaves();
      compressed->Save(&k2tdb_out);
      log_stream << "[BLD] Tree for '" << P.Decode(i) << ": " << tree->links() << "' links\n";
      links += tree->links();
      std::cout << "OK" << std::endl;
    }
    gettimeofday(&t2, NULL);
    auto time = (double)(t2.tv_sec - t1.tv_sec);
    time += (double)(t2.tv_usec - t1.tv_usec)*0.000001;

    log_stream << "[BLD] Time to build: " << time << " s" << std::endl;
    log_stream << "[BLD] Total links: " << links << std::endl;
  }

  template<typename Fun>
  void Read(std::ifstream *in, Fun fun) {
    timeval t1, t2;
    gettimeofday(&t1, NULL);

    std::cout << "[ENC] Encoding started\n";

    //log_stream << STR((SUBJECT)WS+(PREDICATE)WS+(OBJECT)WS+\\.) << std::endl;
    std::string triple_str = STR((SUBJECT)WS+(PREDICATE)WS+(OBJECT)WS+\\.);
    std::regex triple(triple_str, std::regex::extended);
    std::smatch match;

    std::string line;

    uint nlines = 0;
    uint ntriples = 0;
    while (std::getline(*in, line)) {
      bool is_triple = std::regex_match(line, match, triple);
      if (is_triple) {
        ntriples++;
        std::string subject = match.str(1);
        std::string predicate = match.str(2);
        std::string object = match.str(3);

        fun(subject, object, predicate);

        if (ntriples % 1000000 == 0)
          std::cout << "[ENC] Triples Readed: " << ntriples << std::endl;
      } else {
        log_stream << "[ENC] No matched line: " << line << std::endl;
      }
      nlines++;
    }
    gettimeofday(&t2, NULL);
    auto time = (double)(t2.tv_sec - t1.tv_sec);
    time += (double)(t2.tv_usec - t1.tv_usec)*0.000001;

    log_stream << "[ENC] Time to encoded: " << time << " s" << std::endl;
    log_stream << "[ENC] Triples encoded: " << ntriples << std::endl;
    log_stream << "[ENC] Lines readed: " << nlines << std::endl;
  }
};
}  // namespace k2tdb
#endif  // INCLUDE_ENGINE_NTLOADER_H_
