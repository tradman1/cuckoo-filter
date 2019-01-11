#include "cuckoo_hashtable.h"
#include <cstdint>
#include <random>

namespace cuckoofilter {

bool CuckooHashTable::Insert(uint32_t fingerprint, size_t bucket_idx) {
  bucket_idx %= n_buckets_;
  if (buckets_[bucket_idx].size() >= bucket_size) {
    return false;
  }
  buckets_[bucket_idx].push_back(fingerprint);
  return true;
}

bool CuckooHashTable::Remove(uint32_t fingerprint, size_t bucket_idx) {
  bucket_idx %= n_buckets_;
  for (size_t i; i < buckets_[bucket_idx].size(); i++) {
    if (buckets_[bucket_idx][i] == fingerprint) {
      buckets_[bucket_idx].erase(buckets_[bucket_idx].begin() + i);
      return true;
    }
  }
  return false;
}

bool CuckooHashTable::Contains(uint32_t fingerprint, size_t bucket_idx) {
  bucket_idx %= n_buckets_;
  for (auto entry : buckets_[bucket_idx]) {
    if (entry == fingerprint) {
      return true;
    }
  }
  return false;
}

uint32_t CuckooHashTable::SwapEntries(uint32_t fingerprint, size_t bucket_idx) {
  bucket_idx %= n_buckets_;

  uint32_t old_entry = buckets_[bucket_idx].at(0);
  buckets_[bucket_idx].at(0) = fingerprint;
  return old_entry;
}
}  // namespace cuckoofilter