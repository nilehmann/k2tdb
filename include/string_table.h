/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLDUE_STRING_TABLE_H_
#define INCLDUE_STRING_TABLE_H_

#include <unordered_map>
#include <string>

#include <iostream>
typedef unsigned int uint;
class StringTable {
 public:
  uint AddString(const std::string &s) {
    if (map.count(s))
      return map.at(s);

    vec.push_back(s);
    map[s] = vec.size() - 1;
    return vec.size() - 1;
  }

  bool contains(const std::string &s) {
    return map.count(s);
  }

  std::string &GetString(uint index) {
    return vec[index];
  }

 private:
  std::unordered_map<std::string, uint> map;
  std::vector<std::string> vec;  
};
#endif  // INCLDUE_STRING_TABLE_H_
