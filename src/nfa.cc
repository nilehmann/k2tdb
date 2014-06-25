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

struct RegExp2NFA: public boost::static_visitor<uint> {
  Graph *graph_;
  uint end_;

  RegExp2NFA(Graph *g, uint e)
      : graph_(g),
        end_(e) {}

  result_type operator()(uint symbol) const {
    graph_->emplace_back();
    uint start = graph_->size() - 1;
    graph_->at(start).emplace_back(end_, symbol);
    return start;
  }
  result_type operator()(const re::concat &multy) const {
    uint end = end_;
    for (int i = multy.children.size() - 1; i >= 0; --i){
      auto &expr = multy.children[i];
      uint start = boost::apply_visitor(RegExp2NFA(graph_, end), expr);
      end = start;
    }
    return end;
  }

  result_type operator()(const re::alternation &multy) const {
    graph_->emplace_back();
    uint start = graph_->size() - 1;
    for (auto &expr : multy.children) {
      uint start2 = boost::apply_visitor(RegExp2NFA(graph_, end_), expr);
      graph_->at(start).emplace_back(start2, EPS);
    }
    return start;
  }
  result_type operator()(const re::kleene &unary) const {
    graph_->emplace_back();
    uint start = graph_->size() - 1;
    uint start2 = boost::apply_visitor(RegExp2NFA(graph_, start), unary.expr);

    graph_->at(start).emplace_back(start2, EPS);
    graph_->at(start).emplace_back(end_, EPS);

    return start;
  }
};

NFA::NFA(const re::RegExp &regexp): graph_(), start_(), accept_() {
  accept_ = 0;
  graph_.emplace_back();

  start_ = boost::apply_visitor(RegExp2NFA(&graph_, accept_), regexp);
}



}  // namespace NFA
