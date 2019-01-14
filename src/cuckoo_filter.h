#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string>
#include "cuckoo_hasher.h"
#include "cuckoo_hashtable.h"

#include <climits>
#include <functional>
#include <random>
#include <string>

#pragma once

namespace cuckoofilter {

enum Status { Ok = 0, NotFound = 1, NotEnoughSpace = 2 };

const size_t MaxNumKicks = 500;

template <class InputType>
class CuckooFilter {
  CuckooHashTable *table_;
  CuckooHasher hasher_;
  size_t bits_per_item_;
  inline uint16_t Fingerprint(const InputType &item);
  inline size_t Index(const InputType &item);
  inline size_t AltIndex(size_t bucket_idx, uint16_t fingerprint);

 public:
  CuckooFilter(const int capacity);
  CuckooFilter(const int capacity, size_t bits_per_item, Hash hash);

  Status Insert(const InputType &item);
  Status Delete(const InputType &item);
  Status Lookup(const InputType &item);

  void PrintTable() { table_->printTable(); }
  size_t SizeInBytes() { return table_->SizeInBytes(); }
};

}  // namespace cuckoofilter
