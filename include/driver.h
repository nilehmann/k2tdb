/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_DRIVER_H_
#define INCLUDE_DRIVER_H_

#include <engine/graph_db.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <parsing/location.h>
#include <symbol_table.h>
#include <regexp.h>

// forward declaration
class Scanner;


/** The Driver class brings together all components. It creates an instance of
 * the Parser and Scanner classes and connects them. Then the input stream is
 * fed into the scanner object and the parser gets it's token
 * sequence. Furthermore the driver object is available in the grammar rules as
 * a parameter. Therefore the driver class contains a reference to the
 * structure into which the parsed data is saved. */
class Driver {
public:
  /// construct a new parser driver context
  Driver(const engine::GraphDB &db);
  /// enable debug output in the flex scanner
  bool trace_scanning;

  /// enable debug output in the bison parser
  bool trace_parsing;

  /// stream name (file or input stream) used for error messages.
  std::string streamname;

  /** Invoke the scanner and parser for a stream.
   * @param in	input stream
   * @param sname	stream name for error messages
   * @return		true if successfully parsed
   */
  bool parse_stream(std::istream& in,
        const std::string& sname = "stream input");

  bool parse_stdio();

  /** Invoke the scanner and parser on an input string.
   * @param input	input string
   * @param sname	stream name for error messages
   * @return		true if successfully parsed
   */
  bool parse_string(const std::string& input,
        const std::string& sname = "string stream");

  /** Invoke the scanner and parser on a file. Use parse_stream with a
   * std::ifstream if detection of file reading errors is required.
   * @param filename	input file name
   * @return		true if successfully parsed
   */
  bool parse_file(const std::string& filename);

  void query(std::string node,
             const regexp::RegExp<std::string> &expr,
             bool count = false);

  void query(std::string start,
             std::string end,
             const regexp::RegExp<std::string> &expr);

  // To demonstrate pure handling of parse errors, instead of
  // simply dumping them on the standard error output, we will pass
  // them to the driver using the following two member functions.

  /** Error handling with associated line number. This can be modified to
   * output the error e.g. to a dialog box. */
  void error(const parser::location& l, const std::string& m) const;

  /** General error handling. This can be modified to output the error
   * e.g. to a dialog box. */
  void error(const std::string& m) const;


  /** Pointer to the current lexer instance, this is used to connect the
   * parser to the scanner. It is used in the yylex macro. */
  Scanner *lexer;

  SymbolTable sym_table;

  const engine::GraphDB &db_;
};

#endif  // INCLUDE_DRIVER_H_
