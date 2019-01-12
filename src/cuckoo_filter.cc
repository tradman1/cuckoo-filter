#include "cuckoo_filter.h"
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string>

#include <climits>  //#include <openssl/evp.h>
#include <functional>
#include <random>
#include <string>

namespace cuckoofilter {

template <class InputType>
CuckooFilter<InputType>::CuckooFilter(const int capacity) {
  table_ = new CuckooHashTable(capacity);
  CuckooHasher hasher_(Hash::TIMS);  // hasher_ = new
                                     // CuckooHasher(Hash::TIMS);
  bits_per_item_ = 12;
}

template <class InputType>
CuckooFilter<InputType>::CuckooFilter(const int capacity, size_t bits_per_item,
                                      Hash hash) {
  table_ = new CuckooHashTable(capacity);
  hasher_ = new CuckooHasher(hash);
  bits_per_item_ = bits_per_item;
}
template <class InputType>
inline void CuckooFilter<InputType>::GenerateFingerprintIndices(
    const InputType &item, size_t *bucket_idx1, size_t *bucket_idx2,
    uint32_t *fingerprint) {
  // partial-key cuckoo hashing
  const uint64_t hash_ = hasher_.hash(item);
  // fingerprint is a bit string derived from the item using a hash function
  size_t bits_per_item = bits_per_item_;  // TO-DO define this dynamically
  *fingerprint = hash_ % ((1ULL << bits_per_item) - 1);
  *fingerprint += (*fingerprint == 0);
  // index1 = hash(item)
  *bucket_idx1 = hash_;
  // index2 = index1 xor hash(fingerprint)
  *bucket_idx2 = AltIndex(*bucket_idx1, *fingerprint);

  bool debug = false;

  if (debug) {
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
}
template <class InputType>
inline size_t CuckooFilter<InputType>::AltIndex(const size_t index,
                                                const uint32_t fingerprint) {
  const uint64_t hash_ = hasher_.hash((uint64_t)fingerprint);
  return index ^ hash_;
}

template <class InputType>
Status CuckooFilter<InputType>::Insert(const InputType &item) {
  uint32_t fingerprint;
  size_t bucket_idx1, bucket_idx2;

  GenerateFingerprintIndices(item, &bucket_idx1, &bucket_idx2, &fingerprint);
  std::cout << "tryin to insert fingerprint " << fingerprint << " at index1 "
            << bucket_idx1 << " whose mod is "
            << bucket_idx1 % table_->NumBuckets() << " or at index2 "
            << bucket_idx2 << " whose mod is "
            << bucket_idx2 % table_->NumBuckets() << std::endl;
  if (table_->Insert(fingerprint, bucket_idx1)) {
    std::cout << "inserted fingerprint " << fingerprint << " at index1 "
              << bucket_idx1 << " whose mod is "
              << bucket_idx1 % table_->NumBuckets() << std::endl;
    return Ok;
  } else if (table_->Insert(fingerprint, bucket_idx2)) {
    std::cout << "inserted fingerprint " << fingerprint << " at index2 "
              << bucket_idx2 << " whose mod is "
              << bucket_idx2 % table_->NumBuckets() << std::endl;
    return Ok;
  }
  // must relocate existing items
  size_t bucket_idx = std::rand() % 2 == 0 ? bucket_idx1 : bucket_idx2;
  uint32_t old_fingerprint = 0;
  for (uint32_t n = 0; n < MaxNumKicks; n++) {
    bool debug = false;

    if (debug) {
      std::cout << " RELOCATING " << std::endl;
      std::cout << " fingerprint is " << fingerprint << std::endl;
      std::cout << " bucket_idx1 is " << bucket_idx1 << std::endl;
      std::cout << " bucket_idx1 mod is " << bucket_idx1 % table_->NumBuckets()
                << std::endl;
      std::cout << " bucket_idx2 is " << bucket_idx2 << std::endl;
      std::cout << " bucket_idx2 mod is " << bucket_idx2 % table_->NumBuckets()
                << std::endl;

      size_t idx1 = AltIndex(bucket_idx2, fingerprint);
      std::cout << " AltIndex of bucket_idx2 is " << idx1 << " and should be "
                << bucket_idx1 << std::endl;
      std::cout << " AltIndex of bucket_idx2 mod is "
                << idx1 % table_->NumBuckets() << " and should be "
                << bucket_idx1 % table_->NumBuckets() << std::endl;

      size_t idx2 = AltIndex(bucket_idx1, fingerprint);
      std::cout << " AltIndex of bucket_idx1 is " << idx2 << " and should be "
                << bucket_idx2 << std::endl;
      std::cout << " AltIndex of bucket_idx1 mod is "
                << idx2 % table_->NumBuckets() << " and should be "
                << bucket_idx2 % table_->NumBuckets() << std::endl;

      std::cout << " asserting ... ";
      assert(bucket_idx1 == AltIndex(bucket_idx2, fingerprint));
      std::cout << " ... passed" << std::endl;
    }

    std::cout << "moved fingerprint " << fingerprint << " to index "
              << bucket_idx << " whose mod is "
              << bucket_idx % table_->NumBuckets() << std::endl;

    old_fingerprint = table_->SwapEntries(fingerprint, bucket_idx);
    fingerprint = old_fingerprint;

    std::cout << "now moving fingerprint " << fingerprint << " to index ";

    bucket_idx = AltIndex(bucket_idx, fingerprint);

    std::cout << bucket_idx << " whose mod is "
              << bucket_idx % table_->NumBuckets() << std::endl;

    if (table_->Insert(fingerprint, bucket_idx)) {
      std::cout << "inserted fingerprint " << fingerprint << " at index "
                << bucket_idx << " whose mod is "
                << bucket_idx % table_->NumBuckets() << std::endl;

      return Ok;
    }
  }
  // HashTable is considered full
  return NotEnoughSpace;
}

template <class InputType>
Status CuckooFilter<InputType>::Lookup(const InputType &item) {
  uint32_t fingerprint;
  size_t bucket_idx1, bucket_idx2;

  GenerateFingerprintIndices(item, &bucket_idx1, &bucket_idx2, &fingerprint);

  std::cout << "searching fingerprint " << fingerprint
            << " on index1 = " << bucket_idx1
            << " with mod = " << bucket_idx1 % table_->NumBuckets()
            << " and on index2 = " << bucket_idx2
            << " with mod = " << bucket_idx2 % table_->NumBuckets()
            << std::endl;

  if (table_->Contains(fingerprint, bucket_idx1)) {
    return Ok;
  } else if (table_->Contains(fingerprint, bucket_idx2)) {
    return Ok;
  }

  return NotFound;
}

template <class InputType>
Status CuckooFilter<InputType>::Delete(const InputType &item) {
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