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

struct encode_visitor: public boost::static_visitor<regexp::RegExp> {
  const StringTable *str_table;
  encode_visitor(const StringTable *table): str_table(table) {}

  result_type operator()(uint v) const {
    return v+1;
  }

  template<typename Op>
  result_type operator()(regexp::multy_op<Op> &multy) const {
    auto expr = multy.children.begin();
    for(;expr < multy.children.end(); ++expr) {
      *expr = boost::apply_visitor(*this, *expr);
    }
    return multy;
  }

  template<typename Op>
  result_type operator()(regexp::unary_op<Op> &unary) const {
    unary.expr = boost::apply_visitor(*this, unary.expr);
    return unary;
  }
};

void encode(regexp::RegExp &expr, const StringTable &str_table) {
  expr = boost::apply_visitor(encode_visitor(&str_table), expr);
}



DictionaryEncoding::DictionaryEncoding(const path &base_file)
    : db_(),
      array_(base_file.native() + ".arr", 201){
  db_.open(base_file.native() + ".kch", HashDB::OWRITER | HashDB::OCREATE);
}

bool DictionaryEncoding::Add(const std::string &s) {
  if (db_.check(s))
    return false;
  uint size = array_.PushBack(s);
  db_.add(s.data(), s.size(), reinterpret_cast<char*>(&size), sizeof(uint));
  return true;
}
bool DictionaryEncoding::String2Int(const std::string &key, uint *val) {
  return db_.get(key.data(), key.size(),
                 reinterpret_cast<char*>(val), sizeof(uint)) != -1;
}

std::string DictionaryEncoding::Int2String(uint key) {
  return array_[key];
}

DictionaryEncoding::~DictionaryEncoding() {
  db_.close();
}

