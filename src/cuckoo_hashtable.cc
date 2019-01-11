#include "cuckoo_hashtable.h"
#include <cstdint>
#include <random>

namespace cuckoofilter {

bool CuckooHashTable::Insert(uint32_t fingerprint, size_t bucket_idx) {
  bucket_idx %= this->NumBuckets();
  std::vector<uint32_t> bucket = buckets_[bucket_idx];

  if (bucket.size() >= bucket_size) {
    return false;
  }
  bucket.push_back(fingerprint);
  return true;
}

bool CuckooHashTable::Remove(uint32_t fingerprint, size_t bucket_idx) {
  bucket_idx %= this->NumBuckets();
  std::vector<uint32_t> bucket = buckets_[bucket_idx];

  for (size_t i; i < bucket.size(); i++) {
    if (bucket[i] == fingerprint) {
      bucket.erase(bucket.begin() + i);
      return true;
    }
  }
  return false;
}

bool CuckooHashTable::Contains(uint32_t fingerprint, size_t bucket_idx) {
  bucket_idx %= this->NumBuckets();
  std::vector<uint32_t> bucket = buckets_[bucket_idx];

  for (auto entry : bucket) {
    if (entry == fingerprint) {
      return true;
    }
  }
  return false;
}

uint32_t CuckooHashTable::SwapEntries(uint32_t fingerprint, size_t bucket_idx) {
  bucket_idx %= this->NumBuckets();
  std::vector<uint32_t> bucket = buckets_[bucket_idx];

  uint32_t old_entry = bucket.at(0);
  bucket.at(0) = fingerprint;
  return old_entry;
}
}  // namespace cuckoofilter