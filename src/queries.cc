/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <queries.h>
#include <queue>
#include <unordered_set>
#include <boost/tuple/tuple.hpp>
#include <boost/functional/hash.hpp>
#include <boost/tuple/tuple_comparison.hpp>

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

void compute(uint node, NFA::NFA &nfa, Graph &graph) {
  using boost::tuple;
  using boost::make_tuple;

  std::queue<tuple<uint, uint>> Q;
  std::unordered_set<tuple<uint, uint>> V;

  Q.emplace(node, nfa.StartState());
  while (!Q.empty()) {
    uint u = Q.front().get<0>();
    uint q = Q.front().get<1>();
    Q.pop();

    if (nfa.IsAcceptState(q))
      printf("nodo %d", u);

    for (auto &pair : nfa.neighbors(q)) {
      uint r = pair.get<0>();
      uint c = pair.get<1>();

      graph.AdjacencyList(u, c, [=, &V, &Q] (uint v) {
        if (!V.count(make_tuple(v, r))) {
          V.emplace(v, r);
          Q.emplace(v,r);
        }
      });

    }
  }

}
