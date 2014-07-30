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
#include <regexp.h>
#include <vector>
#include <memory>
#include <boost/tuple/tuple.hpp>

namespace NFA {
namespace re = regexp;

using boost::tuple;



class Symbol {
 public:
  Symbol(uint id, bool converse = false): id_(id), converse_(converse) {}
  operator uint() const {
    return id_;
  }
  bool IsConverse() const {
    return converse_;
  }
  bool operator==(const Symbol &rhs) {
    return id_ == rhs.id_ && converse_ == rhs.converse_;
  }
 private:
  uint id_;
  bool converse_;
};
const Symbol EPS(-1);
typedef uint State;
typedef std::vector<tuple<State, Symbol>> Neighbors;
typedef std::vector<Neighbors> Graph;

class NFA {
 public:
  NFA(const re::RegExp<uint> &regexp);

  const Neighbors& neighbors(uint q) const {
    return graph_[q];
  }

  uint StartState() const {
    return start_;
  }

  bool IsAcceptState(uint q) const {
    return accept_ == q;
  }

 private:
  Graph graph_;
  uint start_;
  uint accept_;
};



}  // namespace NFA
#endif  // INCLUDE_NFA_H_
