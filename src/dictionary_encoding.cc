/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <dictionary_encoding.h>
using kyotocabinet::HashDB;
using boost::filesystem::path;





DictionaryEncoding::DictionaryEncoding(const path &base_file,
                                       bool trunc)
    : db_(),
      array_(base_file.native() + ".arr", 201, trunc){
  int opts = HashDB::OWRITER | HashDB::OCREATE;
  if (trunc)
    opts |= HashDB::OTRUNCATE;
  db_.open(base_file.native() + ".kch", opts);
}

bool DictionaryEncoding::Add(const std::string &s) {
  if (db_.check(s) != -1)
    return false;
  uint pos = array_.PushBack(s) - 1;
  db_.add(s.data(), s.size(), reinterpret_cast<char*>(&pos), sizeof(uint));
  return true;
}
bool DictionaryEncoding::String2Int(const std::string &key, uint *val) const {
  return const_cast<HashDB&>(db_).get(key.data(), key.size(),
                 reinterpret_cast<char*>(val), sizeof(uint)) != -1;
}
uint DictionaryEncoding::Count() {
  return db_.count();
}

std::string DictionaryEncoding::Int2String(uint key) const {
  return array_[key];
}

DictionaryEncoding::~DictionaryEncoding() {
  db_.close();
}

