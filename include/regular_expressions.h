/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_REGULAR_EXPRESSIONS_H_
#define INCLUDE_REGULAR_EXPRESSIONS_H_

#include <graph_queries_basic.h>
#include <libk2tree_basic.h>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/apply_visitor.hpp>

namespace regular_expressions {

template<typename OpTag> struct multy_op;
template<typename OpTag> struct unary_op;
struct concat;
struct alternation;
struct kleene;
struct converse;

typedef boost::variant<
          uint,
          boost::recursive_wrapper<multy_op<concat>>,
          boost::recursive_wrapper<multy_op<alternation>>,
          boost::recursive_wrapper<unary_op<kleene>>
        > RegExp;

template<typename OpTag>
struct multy_op {
  std::vector<RegExp> children;
};


template<typename OpTag>
struct unary_op {
  RegExp expr;
};


class print: public boost::static_visitor<void> {
 public:
  result_type operator()(uint value) const;

  result_type operator()(const multy_op<concat> &binary) const;

  result_type operator()(const multy_op<alternation> &binary) const;

  result_type operator()(const unary_op<kleene> &unary) const;
};

void print_expression(const RegExp &expr);

}  // namespace regular_expressions
#endif  // INCLUDE_REGULAR_EXPRESSIONS_H_
