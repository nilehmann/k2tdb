/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_GRAPH_H_
#define INCLUDE_GRAPH_H_

#include <graph_queries_basic.h>
#include <engine/dictionary_encoding.h>
#include <regexp.h>
#include <nfa.h>
#include <fstream>
#include <vector>

#include <boost/filesystem/path.hpp>
#include <compressed_hybrid.h>

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

namespace engine {

struct count_type {
  uint count = 0;
};

template<typename Fun>
struct fun_impl {
  static void Return(Fun) {}
  static void Action(const DictionaryEncoding &dic, uint key, const Fun &fun) {
    fun(dic.Decode(key));
  }
};
template<>
struct fun_impl<count_type> {
  static uint Return(count_type val) {return val.count;}
  static void Action(const DictionaryEncoding &, uint, count_type &val) {
    ++val.count;
  }
};

namespace fs = ::boost::filesystem;

class GraphDB {
 public: GraphDB(fs::path db_name);

  /**
   * Compute the regular expression query starting from the given node.
   * It first encode the node and regular expression using the
   * database dictionary.
   *
   * @param n Number representing the node in the StringTable
   * @param exp Regular expression to compute the query.
   */
  void Compute(std::string n, const regexp::RegExp<std::string> &exp) const;

  uint Count(std::string n, const regexp::RegExp<std::string> &exp) const;

  
  /**
   * Compute the respective query for the given starting node and regular 
   * expression.
   *
   * @param n Number representing a node in the database dictionary.
   * @param exp Regular expression to compute the query. Each symbol is
   * a number representing a node in the database predicate encoding dictionary.
   */
  void Compute(uint n, const regexp::RegExp<uint> &exp) const;
  
  uint Count(uint n, const regexp::RegExp<uint> &exp) const;


  /**
   * Compute the respective query for the given starting node and regular
   * expression corresponding to the given NFA.
   *
   * @param n Number representing a node in the database dictionary.
   * @param nfa NFA corresponding to a regular expression. Each symbol is a
   * number representing a node in the predicate encoding dictionary.
   */
  template<typename Fun = count_type>
  typename std::conditional<std::is_same<Fun, count_type>::value, uint, void>::type
  Compute(uint n, const NFA::NFA &nfa, Fun fun = count_type()) const {
    using boost::tuple;
    using boost::make_tuple;

    std::queue<tuple<uint, NFA::State>> Q;
    std::unordered_set<tuple<uint, NFA::State>> V;

    Q.emplace(n, nfa.StartState());
    while (!Q.empty()) {
      uint u = Q.front().get<0>();
      NFA::State q = Q.front().get<1>();
      Q.pop();

      if (nfa.IsAcceptState(q))
        fun_impl<Fun>::Action(SO_, u, fun);


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
    return fun_impl<Fun>::Return(fun);
  }

 private:
  std::vector<libk2tree::CompressedHybrid> k2trees_;
  DictionaryEncoding SO_;
  DictionaryEncoding P_;
};

}  // namespace engine
#endif  // INCLUDE_GRAPH_H_
