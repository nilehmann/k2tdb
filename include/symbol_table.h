/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_SYMBOL_TABLE_H_
#define INCLUDE_SYMBOL_TABLE_H_

#include <unordered_map>
#include <string>
#include <iostream>

class SymbolTable {
 public:
  void SetSymbol(std::string symbol, std::string val) {
    map[symbol] = val;
  }

  std::string LookupSymbol(std::string symbol) const {
    return map.at(symbol);
  }

  bool Contains(std::string symbol) const {
    return map.count(symbol);
  }

 private:
  std::unordered_map<std::string, std::string> map;
  
};
#endif  // INCLUDE_SYMBOL_TABLE_H_

