//
// Tome Radman
//
#include "../benchmarks/timing.h"
#include "cuckoofilter.h"

#include <assert.h>
#include <math.h>

#include <iostream>
#include <vector>

using cuckoofilter::CuckooFilter;

int main(int argc, char **argv) {
  size_t total_items = atoi(argv[1]);

  // Create a cuckoo filter where each item is of type size_t and
  // use 12 bits for each item:
  //    CuckooFilter<size_t, 12> filter(total_items);
  // To enable semi-sorting, define the storage of cuckoo filter to be
  // PackedTable, accepting keys of size_t type and making 13 bits
  // for each key:
  //   CuckooFilter<size_t, 13, cuckoofilter::PackedTable> filter(total_items);
  CuckooFilter<size_t, 12> filter(total_items);

  auto start_time = NowNanos();

  // Insert items to this cuckoo filter
  size_t num_inserted = 0;
  for (size_t i = 0; i < total_items; i++, num_inserted++) {
    if (filter.Add(i) != cuckoofilter::Ok) {
      break;
    }
  }

  auto insert_time = NowNanos() - start_time;

  // Check if previously inserted items are in the filter, expected
  // true for all items
  start_time = NowNanos();
  for (size_t i = 0; i < num_inserted; i++) {
    assert(filter.Contain(i) == cuckoofilter::Ok);
  }
  auto lookup_time = NowNanos() - start_time;

  // Check non-existing items, a few false positives expected
  size_t total_queries = 0;
  size_t false_queries = 0;
  for (size_t i = total_items; i < 2 * total_items; i++) {
    if (filter.Contain(i) == cuckoofilter::Ok) {
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

  return 0;
}
