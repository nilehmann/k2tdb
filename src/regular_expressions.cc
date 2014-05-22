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
print::result_type print::operator()(uint value) const {
  printf("%c", value);
}

print::result_type print::operator()(const multy_op<concat> &op) const {
  for (auto expr : op.children)
    boost::apply_visitor(*this, expr);
}

print::result_type print::operator()(const multy_op<alternation> &op) const {
  auto expr = op.children.begin();
  apply_visitor(*this, *expr);
  ++expr;
  for (;expr < op.children.end(); ++expr) {
    printf(" | ");
    boost::apply_visitor(*this, *expr);
  }
}

print::result_type print::operator()(const unary_op<kleene> &unary) const {
  printf("(");
  boost::apply_visitor(*this, unary.expr);
  printf(")*");
}
}
