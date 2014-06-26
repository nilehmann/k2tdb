/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_DICTIONARY_ENCODING_H_
#define INCLUDE_DICTIONARY_ENCODING_H_

#include <graph_queries_basic.h>
#include <regexp.h>
#include <string_table.h>
#include <secondary_storage/fixed_array.h>
#include <kchashdb.h>  // kyoto cabinet
#include <boost/filesystem.hpp>


class DictionaryEncoding {
 public:
  DictionaryEncoding(const boost::filesystem::path &base_file,
                     bool trunc = false);
  bool Check(const std::string &s) {return db_.check(s);}
  bool Add(const std::string &s);
  bool String2Int(const std::string &key, uint *val);
  std::string Int2String(uint key);
  uint Count();
  ~DictionaryEncoding();
 private:
  kyotocabinet::HashDB db_;
  secondary_storage::FixedArray<std::string> array_;
};

void encode(regexp::RegExp &expr, const StringTable &str_table);

#endif  // INCLUDE_DICTIONARY_ENCODING_H_
