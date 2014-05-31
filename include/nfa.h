/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_NFA_H_
#define INCLUDE_NFA_H_
#include <graph_queries_basic.h>
#include <regular_expressions.h>
#include <boost/tuple/tuple.hpp>
#include <vector>
#include <memory>

namespace NFA {
namespace re = regular_expressions;
using boost::tuple;

typedef std::vector<tuple<uint, uint>> Neighbors;
typedef std::vector<Neighbors> Graph;
const uint EPS = -1;


class NFA {
 public:
  NFA(const re::RegExp &regexp);

  const Neighbors& neighbors(uint q) const {
    return graph_->at(q);
  }
 private:
  std::shared_ptr<Graph> graph_;
  uint start_;
  uint accept_;
};



}  // namespace NFA
#endif  // INCLUDE_NFA_H_
