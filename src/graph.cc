/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <graph.h>
#include <utils/utils.h>
#include <string_table.h>
#include <dictionary_encoding.h>

#include <boost/tuple/tuple.hpp>
#include <boost/functional/hash.hpp>
#include <boost/tuple/tuple_comparison.hpp>
namespace utils = libk2tree::utils;

struct encode_visitor: public boost::static_visitor<regexp::RegExp> {
  const StringTable &str_table;
  const DictionaryEncoding &P;
  encode_visitor(const StringTable &table, const DictionaryEncoding &P)
    : str_table(table),
      P(P) {}

  result_type operator()(uint v) const {
    const std::string &s = str_table.GetString(v);
    uint val;
    P.String2Int(s, &val);
    return val;
  }

  template<typename Op>
  result_type operator()(regexp::multy_op<Op> &multy) const {
    for (auto &expr : multy.children)
      expr = boost::apply_visitor(*this, expr);
    return multy;
  }

  template<typename Op>
  result_type operator()(regexp::unary_op<Op> &unary) const {
    unary.expr = boost::apply_visitor(*this, unary.expr);
    return unary;
  }
};
GraphDB::GraphDB(fs::path db_name)
    : k2trees_(),
      SO_(db_name.native() + ".so"),
      P_(db_name.native() + ".p") {
  std::ifstream in(db_name.native() + ".k2tdb");
  uint s = utils::LoadValue<uint>(&in);
  k2trees_.reserve(s);
  for (uint i = 0; i < s; ++i)
    k2trees_.emplace_back(&in);
}

void GraphDB::Encode(regexp::RegExp &exp, const StringTable &table) const {
  regexp::print_expression(exp);
  exp = boost::apply_visitor(encode_visitor(table, P_), exp);
  regexp::print_expression(exp);
}

void GraphDB::Compute(uint n,
                      regexp::RegExp &exp,
                      const StringTable &table) const {
  Encode(exp, table);
  const std::string &s = table.GetString(n);
  uint val;
  SO_.String2Int(s, &val);
  std::cerr << "node: " << s << " " << val << std::endl;
  Compute(val, exp);
}

void GraphDB::Compute(uint n, const regexp::RegExp &exp) const {
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

  std::queue<tuple<uint, uint>> Q;
  std::unordered_set<tuple<uint, uint>> V;

  Q.emplace(n, nfa.StartState());
  while (!Q.empty()) {
    uint u = Q.front().get<0>();
    uint q = Q.front().get<1>();
    std::cerr << "(" << u << ", " << q << ")" << std::endl;
    Q.pop();

    if (nfa.IsAcceptState(q))
      std::cout << SO_.Int2String(u) << std::endl;

    for (auto &pair : nfa.neighbors(q)) {
      uint r = pair.get<0>();
      uint c = pair.get<1>();
      std::cerr << "\t(" << r << ", " << c << ")" << std::endl;

      if (c == NFA::EPS) {
        V.emplace(u, r);
        Q.emplace(u, r);
      } else {
        k2trees_[c].DirectLinks(u, [=, &V, &Q] (uint v) {
          if (!V.count(make_tuple(v, r))) {
            V.emplace(v, r);
            Q.emplace(v,r);
          }
        });
      }

    }
  }

}
