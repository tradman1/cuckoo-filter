#include <assert.h>
#include <iostream>
#include "../src/cuckoo_filter.h"
#include "../src/cuckoo_hasher.h"
#include "../src/cuckoo_hashtable.h"
#include "../src/cuckoo_filter.cc"
#include "timing.h"

using namespace cuckoofilter;

int main() {
  size_t total_items = 1000;
  CuckooFilter<size_t> filter(total_items);

  auto start_time = NowNanos();

  // Insert items to this cuckoo filter
  size_t num_inserted = 0;
  for (size_t i = 0; i < total_items; i++, num_inserted++) {
    // std::cout << "inserting item " << i << std::endl;
    if (filter.Insert(i) != Ok) {
      std::cout << "inserted " << 100.0 * num_inserted / total_items
                << "% items %\n"
                << std::endl;
      break;
    }
  }

  assert(num_inserted >= 1);

  auto insert_time = NowNanos() - start_time; 

  // std::cout << "Print table" << std::endl;
  // filter.PrintTable();

  // Check if previously inserted items are in the filter, expected
  // true for all items
  start_time = NowNanos();
  for (size_t i = 0; i < num_inserted; i++) {
    // std::cout << "looking for item " << i << std::endl;
    assert(filter.Lookup(i) == Ok);
  }
  auto lookup_time = NowNanos() - start_time;

  // Check non-existing items, a few false positives expected
  size_t total_queries = 0;
  size_t false_queries = 0;
  for (size_t i = total_items; i < 2 * total_items; i++) {
    if (filter.Lookup(i) == Ok) {
      false_queries++;
    }
    total_queries++;
  }

  std::cout << "Insert time: " << insert_time / (double)1e9 << "s" << std::endl;
  std::cout << "Lookup time: " << lookup_time / (double)1e9 << "s" << std::endl;
  std::cout << "Memory: " << filter.SizeInBytes() << " bytes" << std::endl;
  // Output the measured false positive rate
  std::cout << "false positive rate is "
            << 100.0 * false_queries / total_queries << "%\n";

  std::cout << "PASSED THE TEST!!!" << std::endl;

  return 0;
}