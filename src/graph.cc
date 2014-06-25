/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <graph.h>
#include <utils/utils.h>

namespace utils = libk2tree::utils;

Graph::Graph(std::ifstream *in): k2trees() {
  uint s = utils::LoadValue<uint>(in);
  k2trees.reserve(s);
  for (uint i = 0; i < s; ++i)
    k2trees.emplace_back(in);
}
