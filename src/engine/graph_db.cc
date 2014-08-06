/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <utils/utils.h>
#include <engine/graph_db.h>
#include <engine/dictionary_encoding.h>

#include <boost/tuple/tuple.hpp>
#include <boost/functional/hash.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <boost/timer/timer.hpp>
namespace engine {
namespace utils = ::libk2tree::utils;
GraphDB::GraphDB(fs::path db_name)
    : k2trees_(),
      SO_(),
      P_() {
  SO_.Open(db_name.native() + ".so");
  P_.Open(db_name.native() + ".p");
  std::ifstream in(db_name.native() + ".k2tdb");
  uint s = utils::LoadValue<uint>(&in);
  k2trees_.reserve(s);
  for (uint i = 0; i < s; ++i)
    k2trees_.emplace_back(&in);
}

void GraphDB::Compute(std::string n, const regexp::RegExp<std::string> &exp) const {
  regexp::RegExp<uint> encoded = P_.Encode(exp);
  uint val;
  SO_.Encode(n, &val);
  Compute(val, encoded);
}

uint GraphDB::Count(std::string n, const regexp::RegExp<std::string> &exp) const {
  boost::timer::auto_cpu_timer timer;
  regexp::RegExp<uint> encoded = P_.Encode(exp);
  uint val;
  SO_.Encode(n, &val);
  return Count(val, encoded);
}

uint GraphDB::Count(uint n, const regexp::RegExp<uint> &exp) const {
  return Compute(n, NFA::NFA(exp));
}

void GraphDB::Compute(uint n, const regexp::RegExp<uint> &exp) const {
  Compute(n, NFA::NFA(exp), [] (const std::string &s) {
    std::cout << s << std::endl;
  });
}
}
