#include "cuckoo_filter.h"
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string>

//#include <climits>
#include <openssl/evp.h>
#include <functional>
#include <random>
#include <string>

namespace cuckoofilter {

template <class InputType>
CuckooFilter<InputType>::CuckooFilter(const int capacity) {
  table_ = new CuckooHashTable(capacity);
  hasher_ = CuckooHasher(Hash::TIMS);
  bits_per_item_ = 12;
}

template <class InputType>
CuckooFilter<InputType>::CuckooFilter(const int capacity, size_t bits_per_item,
                                      Hash hash) {
  table_ = new CuckooHashTable(capacity);
  hasher_ = CuckooHasher(hash);
  bits_per_item_ = bits_per_item;
}
template <class InputType>
inline uint32_t CuckooFilter<InputType>::Fingerprint(const InputType &item) {
  // partial-key cuckoo hashing
  const uint64_t hash_ = hasher_.hash(item);
  // take lower bits_per_item_ bits of hash
  uint32_t fingerprint = hash_ & ((1ULL << bits_per_item_) - 1);
  fingerprint += (fingerprint == 0);
  return fingerprint;
}

template <class InputType>
inline size_t CuckooFilter<InputType>::Index(const InputType &item) {
  const uint64_t hash_ = hasher_.hash(item);
  return hash_ >> 32;  // take upper 32 bits of hash
}

template <class InputType>
inline size_t CuckooFilter<InputType>::AltIndex(const size_t index,
                                                const uint32_t fingerprint) {
  const uint64_t hash_ = hasher_.hash(fingerprint);
  return index ^ hash_;
}

template <class InputType>
Status CuckooFilter<InputType>::Insert(const InputType &item) {
  uint32_t fingerprint = Fingerprint(item);
  size_t bucket_idx1 = Index(item);
  size_t bucket_idx2 = AltIndex(bucket_idx1, item);

  if (table_->Insert(fingerprint, bucket_idx1)) {
    return Ok;
  } else if (table_->Insert(fingerprint, bucket_idx2)) {
    return Ok;
  }
  // must relocate existing items
  size_t bucket_idx = std::rand() % 2 == 0 ? bucket_idx1 : bucket_idx2;
  uint32_t old_fingerprint = 0;
  for (uint32_t n = 0; n < MaxNumKicks; n++) {
    old_fingerprint = table_->SwapEntries(fingerprint, bucket_idx);
    fingerprint = old_fingerprint;
    bucket_idx = AltIndex(bucket_idx, fingerprint);
    if (table_->Insert(fingerprint, bucket_idx)) {
      return Ok;
    }
  }
  // HashTable is considered full
  return NotEnoughSpace;
}

template <class InputType>
Status CuckooFilter<InputType>::Lookup(const InputType &item) {
  uint32_t fingerprint = Fingerprint(item);
  size_t bucket_idx1 = Index(item);
  size_t bucket_idx2 = AltIndex(bucket_idx1, item);

  if (table_->Contains(fingerprint, bucket_idx1)) {
    return Ok;
  } else if (table_->Contains(fingerprint, bucket_idx2)) {
    return Ok;
  }

  return NotFound;
}

template <class InputType>
Status CuckooFilter<InputType>::Delete(const InputType &item) {
  uint32_t fingerprint = Fingerprint(item);
  size_t bucket_idx1 = Index(item);
  size_t bucket_idx2 = AltIndex(bucket_idx1, item);

  if (table_->Remove(fingerprint, bucket_idx1)) {
    return Ok;
  } else if (table_->Remove(fingerprint, bucket_idx2)) {
    return Ok;
  }

  return NotFound;
}
}  // namespace cuckoofilter