#ifndef CUCKOO_FILTER_CUCKOO_FILTER_H_
#define CUCKOO_FILTER_CUCKOO_FILTER_H_

#include <string>
#include "cuckoo_hashtable.h"

namespace cuckoofilter {

enum Status { Ok = 0, NotFound = 1, NotEnoughSpace = 2 };

template <class InputType>
class CuckooFilter {
  CuckooHashTable *table_;

  inline void GenerateFingerprintIndices(const InputType &item,
                                         int *bucket_idx1, int *bucket_idx2,
                                         uint16_t *fingerprint) const {
    // partial-key cuckoo hashing
    int bits_per_item = 4;    // how to define this dynamically?
    const uint64_t hash = 0;  // calculate the real hash here
    // generate fingerprint from item hash
    *fingerprint = hash & ((1ULL << bits_per_item) - 1);
    *fingerprint += (fingerprint == 0);
    // generate index1 from item hash
    *bucket_idx1 = (hash >> 32) & (table_->NumBuckets() - 1);
    // generate index2 from index1 and fingerprint
    // 0x5bd1e995 is the hash constant from MurmurHash2
    *bucket_idx2 = (*bucket_idx1 ^ (*fingerprint * 0x5bd1e995)) &
                   (table_->NumBuckets() - 1);
  }

 public:
  CuckooFilter(const int capacity) { table_ = new CuckooHashTable(capacity); }
  Status Insert(const InputType &item);
  Status Delete(const InputType &item);
  Status Lookup(const InputType &item);
};

template <class InputType>
Status CuckooFilter<InputType>::Insert(const InputType &item) {
  uint16_t fingerprint;
  int bucket_idx1, bucket_idx2;

  GenerateFingerprintIndices(item, &bucket_idx1, &bucket_idx2, &fingerprint);

  if (table_->Insert(fingerprint, bucket_idx1)) {
    return Ok;
  } else if (table_->Insert(fingerprint, bucket_idx2)) {
    return Ok;
  }
  // must relocate existing items
  /*
    TO-DO
  */
  // HashTable is considered full
  return NotEnoughSpace;
}

template <class InputType>
Status CuckooFilter<InputType>::Lookup(const InputType &item) {
  uint16_t fingerprint;
  int bucket_idx1, bucket_idx2;

  GenerateFingerprintIndices(item, &bucket_idx1, &bucket_idx2, &fingerprint);

  if (table_->Contains(fingerprint, bucket_idx1)) {
    return Ok;
  } else if (table_->Contains(fingerprint, bucket_idx2)) {
    return Ok;
  }

  return NotFound;
}

template <class InputType>
Status CuckooFilter<InputType>::Delete(const InputType &item) {
  uint16_t fingerprint;
  int bucket_idx1, bucket_idx2;

  GenerateFingerprintIndices(item, &bucket_idx1, &bucket_idx2, &fingerprint);

  if (table_->Remove(fingerprint, bucket_idx1)) {
    return Ok;
  } else if (table_->Remove(fingerprint, bucket_idx2)) {
    return Ok;
  }

  return NotFound;
}
}  // namespace cuckoofilter

#endif