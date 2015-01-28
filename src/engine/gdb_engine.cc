/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <utils/utils.h>
#include <engine/gdb_engine.h>
#include <engine/dictionary_encoding.h>

#include <boost/tuple/tuple.hpp>
#include <boost/functional/hash.hpp>
#include <boost/tuple/tuple_comparison.hpp>

#include <sys/time.h>
#include <time.h>
namespace k2tdb {
namespace engine {

namespace utils = ::libk2tree::utils;
GDBEngine::GDBEngine(fs::path db_name)
    : k2tdb_in_(db_name.native() + ".k2tdb"),
      k2tdb_(&k2tdb_in_),
      SO_(),
      P_() {
  SO_.Open(db_name.native() + ".so");
  P_.Open(db_name.native() + ".p");
}

uint GDBEngine::Count(std::string n, const regexp::RegExp<std::string> &exp) const {
  regexp::RegExp<uint> encoded = P_.Encode(exp);
  uint val;
  SO_.Encode(n, &val);
  return k2tdb_.Count(val, encoded);
}
bool GDBEngine::ExistsPath(std::string start,
                         std::string end,
                         const regexp::RegExp<std::string> &exp) const {
  regexp::RegExp<uint> encoded = P_.Encode(exp);
  uint int_start, int_end;
  if (not SO_.Encode(start, &int_start) or not SO_.Encode(end, &int_end))
    return false;
  return k2tdb_.ExistsPath(int_start, int_end, encoded);
}

}  // namespace engine
}  // namespace k2tdb
