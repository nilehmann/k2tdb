/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <engine/dictionary_encoding.h>

namespace k2tdb {
namespace engine {

using kyotocabinet::HashDB;
using boost::filesystem::path;

DictionaryEncoding::DictionaryEncoding() {}

DictionaryEncoding::DictionaryEncoding(const boost::filesystem::path &base_name,
                                       uint expected) {
  Create(base_name, expected);
}
DictionaryEncoding::DictionaryEncoding(const boost::filesystem::path &base_name) {
  Open(base_name);
}

void DictionaryEncoding::Create(const boost::filesystem::path &base_name,
                                uint expected) {
  auto opts = HashDB::OWRITER | HashDB::OTRUNCATE | HashDB::OCREATE;
  db_.tune_buckets(2L * expected);
  db_.open(base_name.native() + "s2i.kch", opts);
  inverse_db_.open(base_name.native() + "i2s.kch", opts);
}
void DictionaryEncoding::Open(const boost::filesystem::path &base_name) {
  auto opts = HashDB::OREADER;
  db_.open(base_name.native() + "s2i.kch", opts);
  inverse_db_.open(base_name.native() + "i2s.kch", opts);
}


bool DictionaryEncoding::Add(const std::string &s) {
  if (db_.check(s) != -1)
    return false;
  //uint pos = array_.PushBack(s) - 1;
  uint pos = db_.count();
  db_.add(s.data(), s.size(), reinterpret_cast<char*>(&pos), sizeof(uint));
  inverse_db_.add(reinterpret_cast<char*>(&pos), sizeof(uint),
                  s.data(), s.size());
  return true;
}
bool DictionaryEncoding::Encode(const std::string &key, uint *val) const {
  return db_.get(
      key.data(), key.size(),
      reinterpret_cast<char*>(val), sizeof(uint)) != -1;
}
uint DictionaryEncoding::Count() const {
  return db_.count();
}

std::string DictionaryEncoding::Decode(uint key) const {
  //return array_[key];
  size_t size;
  char *arr = inverse_db_.get(
      reinterpret_cast<char*>(&key), sizeof(uint),
      &size);
  std::string s(arr);
  delete [] arr;
  return s;
}

DictionaryEncoding::~DictionaryEncoding() {
  db_.close();
  inverse_db_.close();
}

struct encode_visitor: public boost::static_visitor<regexp::RegExp<uint>> {
  const DictionaryEncoding *P_;
  encode_visitor(const DictionaryEncoding *P): P_(P) {}

  result_type operator()(const std::string &s) const {
    uint val;
    if (not P_->Encode(s, &val)) {
      std::cerr << s << " is not a valid link label\n";
      exit(0);
    }
    return val;
  }

  template<typename Op>
  result_type operator()(const regexp::multy_op<Op, std::string> &multy) const {
    auto new_multy= regexp::multy_op<Op, uint>();
    for (auto &expr : multy.children)
      new_multy.push_expr(boost::apply_visitor(encode_visitor(P_), expr));
    return new_multy;
  }

  template<typename Op>
  result_type operator()(const regexp::unary_op<Op, std::string> &unary) const {
    return regexp::unary_op<Op, uint>(boost::apply_visitor(encode_visitor(P_), unary.expr));
  }

  result_type operator()(const regexp::repetition<std::string> &rep) const {
    return regexp::repetition<uint>(
        boost::apply_visitor(encode_visitor(P_), rep.expr), rep.min, rep.max);
  }
};

regexp::RegExp<uint> DictionaryEncoding::Encode(
    const regexp::RegExp<std::string> &exp) const {
  return boost::apply_visitor(encode_visitor(this), exp);
}

}  // namespace engine
}  // namespace k2tdb
