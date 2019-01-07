#ifndef CUCKOO_FILTER_CUCKOO_FILTER_H_
#define CUCKOO_FILTER_CUCKOO_FILTER_H_

#include <string>
#include "cuckoo_hashtable.h"
#include "hashutil.h"

namespace cuckoofilter {

enum Status { Ok = 0, NotFound = 1, NotEnoughSpace = 2 };

const size_t MaxNumKicks = 500;

template <class InputType, class HashType>
class CuckooFilter {
  CuckooHashTable *table_;
  HashType hasher_;

  inline void GenerateFingerprintIndices(const InputType &item,
                                         int *bucket_idx1, int *bucket_idx2,
                                         uint16_t *fingerprint) const {
    // partial-key cuckoo hashing
    const uint64_t hash = hasher_(item);
    // fingerprint is a bit string derived from the item using a hash function
    int bits_per_item = 4;  // TO-DO define this dynamically
    *fingerprint = hash & ((1ULL << bits_per_item) - 1);
    *fingerprint += (*fingerprint == 0);
    // index1 = hash(item)
    *bucket_idx1 = (hash >> 32) & (table_->NumBuckets() - 1);
    // index2 = index1 xor hash(fingerprint)
    *bucket_idx2 = AltIndex(*bucket_idx1, *fingerprint);
  }

  inline int AltIndex(const int index, const uint16_t fingerprint) const {
    // 0x5bd1e995 is the hash constant from MurmurHash2
    return (index ^ (fingerprint * 0x5bd1e995)) & (table_->NumBuckets() - 1);
  }

 public:
  CuckooFilter(const int capacity) { table_ = new CuckooHashTable(capacity); }
  Status Insert(const InputType &item);
  Status Delete(const InputType &item);
  Status Lookup(const InputType &item);
};

template <class InputType, class HashType>
Status CuckooFilter<InputType, HashType>::Insert(const InputType &item) {
  uint16_t fingerprint;
  int bucket_idx1, bucket_idx2;

  GenerateFingerprintIndices(item, &bucket_idx1, &bucket_idx2, &fingerprint);

  if (table_->Insert(fingerprint, bucket_idx1)) {
    return Ok;
  } else if (table_->Insert(fingerprint, bucket_idx2)) {
    return Ok;
  }
  // must relocate existing items
  for (uint32_t n = 0; n < MaxNumKicks; n++) {
    fingerprint = table_->SwapEntries(fingerprint, bucket_idx1);
    bucket_idx1 = AltIndex(bucket_idx1, fingerprint);
    if (table_->Insert(fingerprint, bucket_idx1)) {
      return Ok;
    }
  }
  // HashTable is considered full
  return NotEnoughSpace;
}

template <class InputType, class HashType>
Status CuckooFilter<InputType, HashType>::Lookup(const InputType &item) {
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

template <class InputType, class HashType>
Status CuckooFilter<InputType, HashType>::Delete(const InputType &item) {
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