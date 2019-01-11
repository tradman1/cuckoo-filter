//#include <openssl/evp.h>
#include <assert.h>
#include <iostream>
#include "cuckoo_filter.h"
#include "cuckoo_hashtable.cc"
#include "cuckoo_hashtable.h"
#include "hashutil.h"

using namespace cuckoofilter;

int main() {
  size_t total_items = 1000000;
  CuckooFilter<size_t, TwoIndependentMultiplyShift> filter(total_items);

  // Insert items to this cuckoo filter
  size_t num_inserted = 0;
  for (size_t i = 0; i < total_items; i++, num_inserted++) {
    std::cout << "inserting item " << i << std::endl;
    if (filter.Insert(i) != Ok) {
      std::cout << "inserted " << 100.0 * num_inserted / total_items
                << "% items %\n" << std::endl;
      break;
    }
  }

  // Check if previously inserted items are in the filter, expected
  // true for all items
  for (size_t i = 0; i < num_inserted; i++) {
    std::cout << "looking for item " << i << std::endl;
    assert(filter.Lookup(i) == Ok);
  }

  // Check non-existing items, a few false positives expected
  size_t total_queries = 0;
  size_t false_queries = 0;
  for (size_t i = total_items; i < 2 * total_items; i++) {
    if (filter.Lookup(i) == Ok) {
      false_queries++;
    }
    total_queries++;
  }

  // Output the measured false positive rate
  std::cout << "false positive rate is "
            << 100.0 * false_queries / total_queries << "%\n";

  return 0;

  return 0;
}