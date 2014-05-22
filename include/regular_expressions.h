/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WAExpression LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_ExpressionGULAR_EXPRESSIONS_H_
#define INCLUDE_ExpressionGULAR_EXPRESSIONS_H_

#include <libk2tree_basic.h>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <memory>

namespace regular_expressions {

using std::shared_ptr;

template<typename OpTag> struct binary_op;
template<typename OpTag> struct unary_op;
struct concat;
struct alternation;
struct kleene;

typedef boost::variant<
          uint,
          boost::recursive_wrapper<binary_op<concat>>,
          boost::recursive_wrapper<binary_op<alternation>>,
          boost::recursive_wrapper<unary_op<kleene>>
        > VariantExpression;
typedef shared_ptr<VariantExpression> Expression;

template<typename OpTag>
struct binary_op {
  const Expression left;
  const Expression right;

  binary_op(const Expression &lhs, const Expression &rhs)
      : left(std::move(lhs)),
        right(std::move(rhs)) {}
};

template<typename OpTag>
Expression make_binary(const Expression &lhs, const Expression &rhs) {
  return Expression(new VariantExpression(
                      binary_op<OpTag>(lhs, rhs)));
}

template<typename OpTag>
Expression make_unary(const Expression &lhs) {
  return Expression(new VariantExpression(
                      unary_op<OpTag>(lhs)));
}

Expression make_symbol(uint val);

template<typename OpTag>
struct unary_op {
  const Expression expr;
  unary_op(const Expression &e)
      : expr(std::move(e)) {}
};

template<typename T>
using visitor = boost::static_visitor<T>;

class print: public visitor<void> {
 public:
  result_type operator()(uint value) const;

  result_type operator()(const binary_op<concat> &binary) const;

  result_type operator()(const binary_op<alternation> &binary) const;

  result_type operator()(const unary_op<kleene> &unary) const;
};

template<typename Visitor>
typename Visitor::result_type apply_visitor(Visitor &visitor,
                                            const Expression &expr) {
  boost::apply_visitor(visitor, *expr);
}
template<typename Visitor>
typename Visitor::result_type apply_visitor(const Visitor &visitor,
                                            const Expression &expr) {
  boost::apply_visitor(visitor, *expr);
}

}  // namespace regular_expressions
#endif  // INCLUDE_REGULAR_EXPRESSIONS_H_
