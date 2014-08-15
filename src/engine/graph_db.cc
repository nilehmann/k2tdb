/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <utils/utils.h>
#include <engine/graph_db.h>
#include <engine/dictionary_encoding.h>

#include <boost/tuple/tuple.hpp>
#include <boost/functional/hash.hpp>
#include <boost/tuple/tuple_comparison.hpp>

#include <sys/time.h>
#include <time.h>
namespace engine {
namespace utils = ::libk2tree::utils;
GraphDB::GraphDB(fs::path db_name)
    : k2trees_(),
      SO_(),
      P_() {
  SO_.Open(db_name.native() + ".so");
  P_.Open(db_name.native() + ".p");
  std::ifstream in(db_name.native() + ".k2tdb");
  uint s = utils::LoadValue<uint>(&in);
  k2trees_.reserve(s);
  for (uint i = 0; i < s; ++i)
    k2trees_.emplace_back(&in);
}
regexp::RegExp<uint> GraphDB::Encode(
    const regexp::RegExp<std::string> &exp) const {
  return P_.Encode(exp);
}
uint GraphDB::Encode(const std::string &s) const {
  uint val;
  SO_.Encode(s, &val);
  return val;
}

void GraphDB::Compute(std::string n, const regexp::RegExp<std::string> &exp) const {
  regexp::RegExp<uint> encoded = P_.Encode(exp);
  uint val;
  SO_.Encode(n, &val);
  Compute(val, encoded);
}

uint GraphDB::Count(std::string n, const regexp::RegExp<std::string> &exp) const {
  regexp::RegExp<uint> encoded = P_.Encode(exp);
  uint val;
  SO_.Encode(n, &val);
  return Count(val, encoded);
}
bool GraphDB::ExistsPath(std::string start,
                         std::string end,
                         const regexp::RegExp<std::string> &exp) const {
  regexp::RegExp<uint> encoded = P_.Encode(exp);
  uint int_start, int_end;
  SO_.Encode(start, &int_start);
  SO_.Encode(end, &int_end);
  bool b = ExistsPath(int_start, int_end, encoded);

  return b;
}

uint GraphDB::Count(uint n, const regexp::RegExp<uint> &exp) const {
  return Compute(n, NFA::NFA(exp));
}

void GraphDB::Compute(uint n, const regexp::RegExp<uint> &exp) const {
  Compute(n, NFA::NFA(exp), [] (const std::string &s) {
    std::cout << s << std::endl;
  });
}
bool GraphDB::ExistsPath(uint start, uint end,
                         const regexp::RegExp<uint> &exp) const {
  return ExistsPath(start, end, NFA::NFA(exp));
}

bool GraphDB::ExistsPath(uint start, uint end, const NFA::NFA &nfa) const {
  using boost::tuple;
  using boost::make_tuple;

  std::queue<tuple<uint, NFA::State>> Q;
  std::unordered_set<tuple<uint, NFA::State>> V;

  Q.emplace(start, nfa.StartState());
  while (!Q.empty()) {
    uint u = Q.front().get<0>();
    NFA::State q = Q.front().get<1>();
    Q.pop();

    if (nfa.IsAcceptState(q) && u == end)
      return true;


    for (auto &pair : nfa.neighbors(q)) {
      NFA::State r = pair.get<0>();
      NFA::Symbol symb = pair.get<1>();

      if (symb == NFA::EPS) {
        if (!V.count(make_tuple(u, r))) {
          V.emplace(u, r);
          Q.emplace(u, r);
        }
      } else if (!symb.IsConverse()){
        k2trees_[symb].DirectLinks(u, [=, &V, &Q] (uint v) {
          if (!V.count(make_tuple(v, r))) {
            V.emplace(v, r);
            Q.emplace(v, r);
          }
        });
      } else {
        k2trees_[symb].InverseLinks(u, [=, &V, &Q] (uint v) {
          if (!V.count(make_tuple(v, r))) {
            V.emplace(v, r);
            Q.emplace(v, r);
          }
        });
      }

    }
  }
  return false;
}

}
