/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <queries/regexp.h>
#include <benchmark/benchmark.h>
#include <compressed_hybrid.h>


template<typename K2Tree>
void execute_query(const k2tdb::Benchmark<K2Tree> &k2tdb,
                   int i, int max_query_count, std::string scenario) {
  k2tdb.execute_query(i, max_query_count, scenario);
}

int main(int argc, char *argv[]) {
  std::cout << "--------------------------------\n";
  std::cout << "----- QUERY EXECUTION TEST -----\n";
  std::cout << "--------------------------------\n\n";

  if (argc < 3) {
    std::cout << "Usage: " << argv[0] << " base_path query_numer [max_query_count]\n";
    return 0;
  }

  int max_query_count = 100;
  if (argc >= 4)
    max_query_count = std::stoi(argv[3]);

  k2tdb::Benchmark<libk2tree::CompressedHybrid> k2tdb(argv[1]);
  uint q = std::stoi(argv[2]);

  int left = 1;
  int right = 8;
  if (q > 0)
    left = right = q;

  std::cout << "Cold:\n";
  for (int i = left; i <= right; ++i)
    execute_query(k2tdb, i, max_query_count, "cold");

  std::cout << "Hot:\n";
  for (int i = left; i <= right; ++i)
    execute_query(k2tdb, i, max_query_count, "hot");

  return 0;
}

