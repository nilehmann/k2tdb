/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_GRAPH_H_
#define INCLUDE_GRAPH_H_

#include <graph_queries_basic.h>
#include <fstream>
#include <vector>

#include <compressed_hybrid.h>
class Graph {
 public:
  Graph(std::ifstream *in);

  template<typename Function>
  void AdjacencyList(uint p, uint d, Function fun) {
    k2trees[d].DirectLinks(p, fun);
  }
  template<typename Function>
  void ReverseList(uint q, uint d, Function fun) {
    k2trees[d].InverseLinks(q, fun);
  }

 private:
  std::vector<libk2tree::CompressedHybrid> k2trees;
};
#endif  // INCLUDE_GRAPH_H_
