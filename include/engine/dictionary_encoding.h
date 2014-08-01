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
#include <secondary_storage/fixed_array.h>
#include <kchashdb.h>  // kyoto cabinet
#include <boost/filesystem.hpp>


class DictionaryEncoding {
 public:
  void Create(const boost::filesystem::path &base_name, uint expected);
  void Open(const boost::filesystem::path &base_name);
  /**
   * Encode each symbol in the regular expression using this dictionary.
   *
   * @param exp Regular expression to be encoded.
   * @result Regular expression where each symbol is the encoding of the string
   * in the original expression.
   */
  regexp::RegExp<uint> Encode(regexp::RegExp<std::string> &exp) const;
  /**
   * Check if exists and returns the encoding of the given string.
   * @param key The string to encode.
   * @param val The encoding result is stored in this pointer.
   * 
   * @result true if the string exists in the dictionary, false otherwise.
   */
  bool Encode(const std::string &key, uint *val) const;
  /**
   * Decode the given id.
   *
   * @param key Id to be decoded. Calling this method with an id not in the
   * dictionary range yields an unspecified behavior
   *
   * @result The corresponding string to the given id.
   */
  std::string Decode(uint key) const;
  bool Check(const std::string &s) {return db_.check(s);}

  /**
   * Adds a string to the dictionary. The string gets the next id as encoding.
   *
   * @param s String
   *
   * @return false if the string was already in the dictionary, true in case
   * the string was actually added to the dictionary.
   */
  bool Add(const std::string &s);
  uint Count();
  ~DictionaryEncoding();
 private:
  mutable kyotocabinet::HashDB db_;
  //secondary_storage::FixedArray<std::string> array_;
  mutable kyotocabinet::HashDB inverse_db_;
};


#endif  // INCLUDE_DICTIONARY_ENCODING_H_
