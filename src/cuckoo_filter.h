#ifndef CUCKOO_FILTER_CUCKOO_FILTER_H_
#define CUCKOO_FILTER_CUCKOO_FILTER_H_

#include <string>
#include "cuckoo_hashtable.h"

namespace cuckoofilter {

enum Status { Ok = 0, NotFound = 1, NotEnoughSpace = 2 };

class CuckooFilter {
  CuckooHashTable* table_;

 public:
  Status insert(const std::string& item);
  Status remove(const std::string& item);
  Status lookup(const std::string& item);
};

};  // namespace cuckoofilter

#endif