/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <nfa.h>
#include <algorithm>

namespace NFA {
using boost::make_tuple;

struct RegExp2NFA: public boost::static_visitor<tuple<uint, uint, uint>> {
  std::shared_ptr<Graph> graph;

  RegExp2NFA(): graph(new Graph()) {}

  result_type operator()(uint symbol) const {
    graph->emplace_back();
    uint node = graph->size() - 1;
    return make_tuple(node, node, symbol);
  }
  result_type operator()(const re::multy_op<re::concat> &multy) const {
    uint start, end, symbol;
    result_type prev, curr;

    prev = boost::apply_visitor(*this, multy.children[0]);
    start = prev.get<0>();

    for (uint i = 1; i < multy.children.size(); ++i) {
      curr = boost::apply_visitor(*this, multy.children[i]);

      uint from = prev.get<1>();
      uint symbol = prev.get<2>();
      uint to = curr.get<0>();

      graph->at(from).emplace_back(to, symbol);
      
      std::swap(prev, curr);
    }

    end = prev.get<1>();
    symbol = prev.get<2>();

    return make_tuple(start, end, symbol);
  }

  result_type operator()(const re::multy_op<re::alternation> &multy) const {
    uint start, end;

    graph->emplace_back();
    start = graph->size() - 1;
    graph->emplace_back();
    end = graph->size() - 1;

    for (auto expr : multy.children) {
      auto res = boost::apply_visitor(*this, expr);
      graph->at(start).emplace_back(res.get<0>(), EPS);
      graph->at(res.get<1>()).emplace_back(end, res.get<2>());
    }

    return make_tuple(start, end, EPS);
  }
  result_type operator()(const re::unary_op<re::kleene> &unary) const {
    uint node;
    graph->emplace_back();
    node = graph->size() - 1;

    auto res = boost::apply_visitor(*this, unary.expr);

    graph->at(node).emplace_back(res.get<0>(), EPS);
    graph->at(res.get<1>()).emplace_back(node, res.get<2>());

    return make_tuple(node, node, EPS);
  }
};

NFA::NFA(const re::RegExp &regexp) {
  RegExp2NFA visitor;

  auto res = boost::apply_visitor(visitor, regexp);

  graph_ = visitor.graph;
  start_ = res.get<0>();

  uint symbol = res.get<2>();
  if (symbol != EPS) {
    graph_->emplace_back();
    accept_ = graph_->size() - 1;
    graph_->at(res.get<1>()).emplace_back(accept_, symbol);
  } else {
    accept_ = res.get<1>();
  }
}



}  // namespace NFA
