/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_QUERIES_H_
#define INCLUDE_QUERIES_H_

#include <nfa.h>
#include <graph.h>

void compute(uint node, regexp::RegExp &expr, Graph &graph);
#endif  // INCLUDE_QUERIES_H_
