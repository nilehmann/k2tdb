/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_REGEXP_H_
#define INCLUDE_REGEXP_H_

#include <graph_queries_basic.h>
#include <libk2tree_basic.h>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <array>

namespace regexp {

template<typename OpTag> struct multy_op;
template<typename OpTag> struct unary_op;
struct concat_tag;
struct alternation_tag;
struct kleene_tag;

typedef multy_op<concat_tag> concat;
typedef multy_op<alternation_tag> alternation;
typedef unary_op<kleene_tag> kleene;

typedef boost::variant<
          uint,
          boost::recursive_wrapper<concat>,
          boost::recursive_wrapper<alternation>,
          boost::recursive_wrapper<kleene>
        > RegExp;

void print_expression(const RegExp &expr);



template<typename OpTag>
struct multy_op {
  multy_op() {}
  multy_op(multy_op &&rhs) noexcept {
    children.swap(rhs.children);
  }
  multy_op(const multy_op &rhs): children(rhs.children) {}
  multy_op &operator=(multy_op &&rhs) {
    children.swap(rhs.children);
    return *this;
  }
  void push_expr(RegExp &&expr) {
    children.push_back(std::move(expr));
  }
  std::vector<RegExp> children;
};

template<typename OpTag>
struct unary_op {
  unary_op(unary_op &&rhs) noexcept: expr(std::move(rhs.expr)) {}
  unary_op(const unary_op &rhs): expr(rhs.expr) {}
  unary_op(RegExp &&e) noexcept: expr(std::move(e)) {}
  unary_op &operator=(unary_op &&rhs) {
    std::swap(expr, rhs.expr);
    return *this;
  }
  RegExp expr;
};



}  // namespace regexp

#endif  // INCLUDE_REGEXP_H_

