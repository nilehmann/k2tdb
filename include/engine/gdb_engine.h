/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_ENGINE_GDB_ENGINE_H_
#define INCLUDE_ENGINE_GDB_ENGINE_H_

#include <k2tdb_basics.h>
#include <compressed_hybrid.h>
#include <engine/k2tdb_engine.h>
#include <engine/dictionary_encoding.h>
#include <queries/regexp.h>
#include <queries/nfa.h>
#include <fstream>
#include <vector>

#include <boost/filesystem/path.hpp>



namespace k2tdb {
namespace engine {

namespace fs = ::boost::filesystem;

class GDBEngine {
 public: 
  GDBEngine(fs::path db_name);

  /**
   * Compute the regular expression query starting from the given node.
   * It first encode the node and regular expression using the
   * database dictionary.
   *
   * @param n Number representing the node in the StringTable
   * @param exp Regular expression to compute the query.
   */
  template<typename Fun>
  void Compute(std::string n, const regexp::RegExp<std::string> &exp,
               Fun output) const {
    regexp::RegExp<uint> encoded = P_.Encode(exp);
    uint val;
    if (not SO_.Encode(n, &val))
      return;
    k2tdb_.Compute(val, encoded, [&](uint u) {
      output(SO_.Decode(u));
    });
  }

  uint Count(std::string n, const regexp::RegExp<std::string> &exp) const;

  bool ExistsPath(std::string start,
                  std::string end,
                  const regexp::RegExp<std::string> &exp) const;

 private:
  std::ifstream k2tdb_in_;
  K2tdbEngine<libk2tree::CompressedHybrid> k2tdb_;
  DictionaryEncoding SO_;
  DictionaryEncoding P_;
};

}  // namespace engine
}  // namespace k2tdb
#endif  // INCLUDE_ENGINE_GDB_ENGINE_H_
