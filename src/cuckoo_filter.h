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
                                         size_t *bucket_idx1,
                                         size_t *bucket_idx2,
                                         uint32_t *fingerprint) const {
    // partial-key cuckoo hashing
    const uint64_t hash = hasher_(item);
    // fingerprint is a bit string derived from the item using a hash function
    int bits_per_item = 16;  // TO-DO define this dynamically
    *fingerprint = hash % ((1ULL << bits_per_item) - 1);
    *fingerprint += (*fingerprint == 0);
    // index1 = hash(item)
    *bucket_idx1 = hash;
    // index2 = index1 xor hash(fingerprint)
    *bucket_idx2 = AltIndex(*bucket_idx1, *fingerprint);

    std::cout << " fingerprint is " << *fingerprint << std::endl;
    std::cout << " bucket_idx1 is " << *bucket_idx1 << std::endl;
    std::cout << " bucket_idx1 mod is " << *bucket_idx1 % table_->NumBuckets()
              << std::endl;
    std::cout << " bucket_idx2 is " << *bucket_idx2 << std::endl;
    std::cout << " bucket_idx2 mod is " << *bucket_idx2 % table_->NumBuckets()
              << std::endl;

    size_t idx1 = AltIndex(*bucket_idx2, *fingerprint);
    std::cout << " AltIndex of bucket_idx2 is " << idx1 << " and should be "
              << *bucket_idx1 << std::endl;
    std::cout << " AltIndex of bucket_idx2 mod is "
              << idx1 % table_->NumBuckets() << " and should be "
              << *bucket_idx1 % table_->NumBuckets() << std::endl;

    size_t idx2 = AltIndex(*bucket_idx1, *fingerprint);
    std::cout << " AltIndex of bucket_idx1 is " << idx2 << " and should be "
              << *bucket_idx2 << std::endl;
    std::cout << " AltIndex of bucket_idx1 mod is "
              << idx2 % table_->NumBuckets() << " and should be "
              << *bucket_idx2 % table_->NumBuckets() << std::endl;

    std::cout << " asserting ... ";
    assert(*bucket_idx1 == AltIndex(*bucket_idx2, *fingerprint));
    std::cout << " ... passed" << std::endl;
  }

  inline size_t AltIndex(const size_t index, const uint32_t fingerprint) const {
    // 0x5bd1e995 is the hash constant from MurmurHash2
    const uint64_t hash = hasher_(fingerprint);
    return (index ^ hash);
  }

 public:
  CuckooFilter(const int capacity) { table_ = new CuckooHashTable(capacity); }
  Status Insert(const InputType &item);
  Status Delete(const InputType &item);
  Status Lookup(const InputType &item);
};

template <class InputType, class HashType>
Status CuckooFilter<InputType, HashType>::Insert(const InputType &item) {
  uint32_t fingerprint;
  size_t bucket_idx1, bucket_idx2;

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
  uint32_t fingerprint;
  size_t bucket_idx1, bucket_idx2;

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
  uint32_t fingerprint;
  size_t bucket_idx1, bucket_idx2;

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