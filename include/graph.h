/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_GRAPH_H_
#define INCLUDE_GRAPH_H_

#include <graph_queries_basic.h>
#include <dictionary_encoding.h>
#include <string_table.h>
#include <regexp.h>
#include <nfa.h>
#include <fstream>
#include <vector>

#include <boost/filesystem/path.hpp>

namespace fs = boost::filesystem;

#include <compressed_hybrid.h>
class GraphDB {
 public:
  GraphDB(fs::path db_name);

  /**
   * Encode each symbol in the regular expression using the predicate
   * encoding dictionary.
   *
   * @param exp Regular expression to be encoded. Each symbol is a number
   * representing a string in the given string table.
   * @param table Table containing the strings.
   */
  void Encode(regexp::RegExp &exp, const StringTable &table) const;

  /**
   * Compute the regular expression query starting from the given node.
   * It first encode the node and regular expression using the
   * database dictionary.
   *
   * @param n Number representing the node in the StringTable
   * @param exp Regular expression to compute the query. Each symbol in
   * the regular expression is a number that represent a string in the 
   * StringTable.
   * @param table Table containing the strings.
   */
  void Compute(uint n,
               regexp::RegExp &exp,
               const StringTable &table) const;

  
  /**
   * Compute the respective query for the given stating node and regular
   * expression.
   *
   * @param n Number representing a node in the database dictionary.
   * @param exp Regular expression to compute the query. Each symbol is
   * a number representing a node in the database predicate encoding dictionary.
   */
  void Compute(uint n, const regexp::RegExp &exp) const;


  /**
   * Compute the respective query for the given starting node and regular
   * expression corresponding to the given NFA.
   *
   * @param n Number representing a node in the database dictionary.
   * @param nfa NFA corresponding to a regular expression. Each symbol is a
   * number representing a node in the database predicate eincoding dictionary.
   */
  void Compute(uint n, const NFA::NFA &nfa) const;

 private:
  std::vector<libk2tree::CompressedHybrid> k2trees_;
  DictionaryEncoding SO_;
  DictionaryEncoding P_;
};
#endif  // INCLUDE_GRAPH_H_
