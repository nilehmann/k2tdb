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
namespace utils = libk2tree::utils; GraphDB::GraphDB(fs::path db_name)
    : k2trees_(),
      SO_(db_name.native() + ".so"),
      P_(db_name.native() + ".p") {
  std::ifstream in(db_name.native() + ".k2tdb");
  uint s = utils::LoadValue<uint>(&in);
  k2trees_.reserve(s);
  for (uint i = 0; i < s; ++i)
    k2trees_.emplace_back(&in);
}

void GraphDB::Compute(std::string n, regexp::RegExp<std::string> &exp) const {
  regexp::RegExp<uint> encoded = P_.Encode(exp);
  uint val;
  SO_.Encode(n, &val);
  Compute(val, encoded);
}

void GraphDB::Compute(uint n, const regexp::RegExp<uint> &exp) const {
  Compute(n, NFA::NFA(exp));
}

namespace std {
template<typename A, typename B>
struct hash<boost::tuple<A, B>> {
  size_t operator()(const boost::tuple<A,B> &t) const {
    size_t seed = 0;
    boost::hash_combine(seed, t.template get<0>());
    boost::hash_combine(seed, t.template get<1>());
    return seed;
  }
};
}
void GraphDB::Compute(uint n, const NFA::NFA &nfa) const {
  using boost::tuple;
  using boost::make_tuple;

  std::queue<tuple<uint, NFA::State>> Q;
  std::unordered_set<tuple<uint, uint>> V;

  Q.emplace(n, nfa.StartState());
  while (!Q.empty()) {
    uint u = Q.front().get<0>();
    NFA::State q = Q.front().get<1>();
    Q.pop();

    if (nfa.IsAcceptState(q))
      std::cout << SO_.Decode(u) << std::endl;

    for (auto &pair : nfa.neighbors(q)) {
      NFA::State r = pair.get<0>();
      NFA::Symbol symb = pair.get<1>();

      if (symb == NFA::EPS) {
        V.emplace(u, r);
        Q.emplace(u, r);
      } else if (!symb.IsConverse()){
        k2trees_[symb].DirectLinks(u, [=, &V, &Q] (uint v) {
          if (!V.count(make_tuple(v, r))) {
            V.emplace(v, r);
            Q.emplace(v,r);
          }
        });
      } else {
        k2trees_[symb].InverseLinks(u, [=, &V, &Q] (uint v) {
          if (!V.count(make_tuple(v, r))) {
            V.emplace(v, r);
            Q.emplace(v,r);
          }
        });
      }

    }
  }

}
