/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <regular_expressions.h>

namespace regular_expressions {
Expression make_symbol(uint val) {
  return Expression(new VariantExpression(val));
}
print::result_type print::operator()(uint value) const {
  printf("%c", value);
}

print::result_type print::operator()(const binary_op<concat> &binary) const {
  apply_visitor((*this), binary.left);
  apply_visitor((*this), binary.right);
}

print::result_type print::operator()(const binary_op<alternation> &binary) const {
  apply_visitor((*this), binary.left);
  printf(" | ");
  apply_visitor((*this), binary.right);
}

print::result_type print::operator()(const unary_op<kleene> &unary) const {
  printf("(");
  apply_visitor((*this), unary.expr);
  printf(")*");
}
}
