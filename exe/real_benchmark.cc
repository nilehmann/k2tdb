/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <queries/regexp.h>
#include <benchmark/dblp_benchmark.h>
#include <benchmark/linkedmdb_benchmark.h>
#include <compressed_hybrid.h>


float operator-(timespec t1, timespec t2) {
  float us = (t1.tv_sec - t2.tv_sec)*1000000;
  us += (float) (t1.tv_nsec - t2.tv_nsec) / 1000.0;
  return us;
} 

template<typename Benchmark>
void execute_queries(const Benchmark &k2tdb,
                     int left, int right, int max_query_count) {
  std::cout << "Cold:\n";
  for (int i = left; i <= right; ++i)
    k2tdb.execute_query(i, max_query_count, "cold");

  std::cout << "Hot:\n";
  for (int i = left; i <= right; ++i)
    k2tdb.execute_query(i, max_query_count, "hot");
}

int main(int argc, char *argv[]) {
  std::cout << "--------------------------------\n";
  std::cout << "----- QUERY EXECUTION TEST -----\n";
  std::cout << "--------------------------------\n\n";

  if (argc < 4) {
    std::cout << "Usage: " << argv[0] << " base_path query_numer type [max_query_count]\n";
    return 0;
  }

  int max_query_count = 100;
  if (argc >= 5)
    max_query_count = std::stoi(argv[4]);

  uint q = std::stoi(argv[2]);

  int left = 1;
  int right = 8;
  if (q > 0)
    left = right = q;

  if (std::stoi(argv[3]) == 1) {
    std::cout << "Benchmarking dblp\n";
    k2tdb::DBLPBenchmark<libk2tree::CompressedHybrid> k2tdb1(argv[1]);
    execute_queries(k2tdb1, left, right, max_query_count);
  } else {
    std::cout << "Benchmarking linkedmdb\n";
    k2tdb::LinkedmdbBenchmark<libk2tree::CompressedHybrid> k2tdb2(argv[1]);
    execute_queries(k2tdb2, left, right, max_query_count);
  }


  return 0;
}
