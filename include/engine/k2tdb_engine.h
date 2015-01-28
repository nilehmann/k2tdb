/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_ENGINE_K2TDB_ENGINE_H_
#define INCLUDE_ENGINE_K2TDB_ENGINE_H_

#include <k2tdb_basics.h>
#include <queries/nfa.h>
#include <utils/utils.h>
#include <vector>
#include <queue>
#include <unordered_set>
#include <fstream>
#include <boost/functional/hash.hpp>

namespace std {
template<typename A, typename B>
struct hash<std::pair<A, B>> {
  size_t operator()(const std::pair<A,B> &t) const {
    size_t seed = 0;
    boost::hash_combine(seed, get<0>(t));
    boost::hash_combine(seed, get<1>(t));
    return seed;
  }
};
}

namespace k2tdb {
namespace engine {
namespace utils = ::libk2tree::utils;


template<typename K2Tree>
class K2tdbEngine {
 public:
  K2tdbEngine(std::ifstream *in): k2trees_() {
    uint s = utils::LoadValue<uint>(in);
    k2trees_.reserve(s);
    for (uint i = 0; i < s; ++i)
      k2trees_.emplace_back(in);
  }

  bool ExistsPath(uint s, uint e, const regexp::RegExp<uint> &exp) const {
    return ExistsPath(s, e, NFA::NFA(exp));
  }

  bool ExistsPath(uint s, uint e, const NFA::NFA &nfa) const {
    bool found = false;
    GenericAlgorithm(s, nfa, [&] (uint u) {
      return found = (u == e);
    });
    return found;
  }

  /**
   * Compute the respective query for the given starting node and regular 
   * expression.
   *
   * @param n Number representing a node in the database dictionary.
   * @param exp Regular expression to compute the query. Each symbol is
   * a number representing a node in the database predicate encoding dictionary.
   */
  template<typename Fun>
  void Compute(uint s, const regexp::RegExp<uint> &exp, Fun output) const {
    Compute(s, NFA::NFA(exp), output);
  }

  /**
   * Compute the respective query for the given starting node and regular
   * expression corresponding to the given NFA.
   *
   * @param n Number representing a node in the database dictionary.
   * @param nfa NFA corresponding to a regular expression. Each symbol is a
   * number representing a node in the predicate encoding dictionary.
   */
  template<typename Fun>
  void Compute(uint s, const NFA::NFA &nfa, Fun output) const {
    GenericAlgorithm(s, nfa, [&] (uint u) {
        output(u);
        return false;
    });
  }

  uint Count(uint s, const regexp::RegExp<uint> &exp) const {
    return Count(s, NFA::NFA(exp));
  }

  uint Count(uint s, const NFA::NFA &nfa) const {
    uint count = 0;
    Compute(s, nfa, [&] (uint) {
      count++;
    });
    return count;
  }

 private:
  std::vector<K2Tree> k2trees_;

  template<typename Fun>
  void Neighbors(uint u, NFA::Symbol symb, Fun fun) const {
    if (symb == NFA::EPS)
      fun(u);
    else if (symb.IsConverse())
      k2trees_[symb].InverseLinks(u, fun);
    else
      k2trees_[symb].DirectLinks(u, fun);
  }
  

  template<typename Fun>
  void GenericAlgorithm(uint s, const NFA::NFA &nfa, Fun output) const {
    using std::pair;
    using std::make_pair;
    using std::get;

    std::queue<pair<uint, NFA::State>> Q;
    std::unordered_set<pair<uint, NFA::State>> V;

    Q.emplace(s, nfa.StartState());
    while (!Q.empty()) {
      uint u = get<0>(Q.front());
      NFA::State q = get<1>(Q.front());
      Q.pop();

      if (nfa.IsAcceptState(q))
        if(output(u))
          break;

      for (auto &pair : nfa.neighbors(q)) {
        NFA::State r = get<0>(pair);
        NFA::Symbol symb = get<1>(pair);

        Neighbors(u, symb, [=, &V, &Q] (uint v) {
          if (!V.count(make_pair(v, r))) {
            V.emplace(v, r);
            Q.emplace(v, r);
          }
        });

      }
    }
  }
};

}  // namespace engine
}  // namespace cli

#endif  // INCLUDE_ENGINE_K2TDB_ENGINE_H_
