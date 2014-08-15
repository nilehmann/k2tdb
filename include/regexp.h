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

#include <iostream>

namespace regexp {

template<typename OpTag, typename T> struct multy_op;
template<typename OpTag, typename T> struct unary_op;
template<typename T> struct repetition;
struct concat_tag;
struct alternation_tag;
struct kleene_tag;
struct converse_tag;
struct repetition_tag;

template<typename T> using concat = multy_op<concat_tag, T>;
template<typename T> using alternation = multy_op<alternation_tag, T>;
template<typename T> using kleene = unary_op<kleene_tag, T>;
template<typename T> using converse = unary_op<converse_tag, T>;

template<typename T>
using RegExp = boost::variant<
  T,
  boost::recursive_wrapper<concat<T>>,
  boost::recursive_wrapper<alternation<T>>,
  boost::recursive_wrapper<kleene<T>>,
  boost::recursive_wrapper<converse<T>>,
  boost::recursive_wrapper<repetition<T>>
  >;



template<typename T>
class print: public boost::static_visitor<void> {
 public:
  result_type operator()(T value) const {
    std::cout << value;
  }

  result_type operator()(const concat<T> &op) const {
    for (auto &expr : op.children)
      boost::apply_visitor(*this, expr);
  }

  result_type operator()(const alternation<T> &op) const {
    printf("(");
    auto expr = op.children.begin();
    boost::apply_visitor(*this, *expr);
    ++expr;
    for (;expr < op.children.end(); ++expr) {
      printf(" | ");
      boost::apply_visitor(*this, *expr);
    }
    printf(")");
  }

  result_type operator()(const kleene<T> &unary) const {
    printf("(");
    boost::apply_visitor(*this, unary.expr);
    printf(")*");
  }

  result_type operator()(const converse<T> &unary) const {
    printf("^(");
    boost::apply_visitor(*this, unary.expr);
    printf(")");
  }

  result_type operator()(const repetition<T> &repeat) const {
    printf("^(");
    boost::apply_visitor(*this, repeat.expr);
    printf("){%u,%u}", repeat.min, repeat.max);
  }
};

template<typename T>
void print_expression(const RegExp<T> &expr) {
  boost::apply_visitor(print<T>(), expr);
  std::cout << std::endl;
}

template<typename OpTag, typename T>
struct multy_op {
  multy_op() {}
  multy_op(RegExp<T> &&e1, RegExp<T> &&e2): children() {
    children.push_back(std::move(e1));
    children.push_back(std::move(e2));
  }
  multy_op(multy_op &&rhs) noexcept {
    children.swap(rhs.children);
  }
  multy_op(const multy_op &rhs): children(rhs.children) {}
  multy_op &operator=(multy_op &&rhs) {
    children.swap(rhs.children);
    return *this;
  }
  void push_expr(RegExp<T> &&expr) {
    children.push_back(std::move(expr));
  }
  std::vector<RegExp<T>> children;
};

template<typename OpTag, typename T>
struct unary_op {
  unary_op(unary_op &&rhs) noexcept: expr(std::move(rhs.expr)) {}
  unary_op(const unary_op &rhs): expr(rhs.expr) {}
  unary_op(RegExp<T> &&e) noexcept: expr(std::move(e)) {}
  unary_op &operator=(unary_op &&rhs) {
    std::swap(expr, rhs.expr);
    return *this;
  }
  RegExp<T> expr;
};

template<typename T>
struct repetition: unary_op<repetition_tag, T> {
  repetition(repetition &&rhs) noexcept
    : repetition(std::move(rhs.expr), rhs.min, rhs.max) {}
  repetition(const repetition &rhs)
    : unary_op<repetition_tag, T>(rhs),
      min(rhs.min),
      max(rhs.max) {}
  repetition(RegExp<T> &&e, uint min, uint max) noexcept
    : unary_op<repetition_tag, T>(std::move(e)),
      min(min),
      max(max) {}
  repetition &operator=(repetition &&rhs) {
    std::swap(this->expr, rhs.expr);
    return *this;
  }
  uint min;
  uint max;
};



}  // namespace regexp

#endif  // INCLUDE_REGEXP_H_

