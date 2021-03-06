//
// author: Tome Radman
//
#include "cuckoo_hashtable.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <cstdint>
#include <random>

#include <climits>
#include <functional>
#include <random>
#include <string>

namespace cuckoofilter {

CuckooHashTable::CuckooHashTable(size_t capacity) : capacity_(capacity) {
  // for cuckoo filter to work, number of buckets must be power of 2
  n_buckets_ = upperpower2((uint64_t)capacity / bucket_size);
  double load_factor = (double)capacity / n_buckets_ / bucket_size;
  if (load_factor > 0.96) {
    n_buckets_ *= 2;
  }
  buckets_.resize(n_buckets_);  // allocate space for n_buckets buckets
}

size_t CuckooHashTable::NumBuckets() { return n_buckets_; }

size_t CuckooHashTable::size() {
  int size = 0;
  for (size_t i = 0; i < n_buckets_; ++i) {
    size += buckets_[i].size();
  }
  return size;
};

size_t CuckooHashTable::SizeInBytes() { return size() * sizeof(uint16_t); }

size_t CuckooHashTable::capacity() { return capacity_; };

void CuckooHashTable::printTable() {
  for (size_t i = 0; i < buckets_.size(); i++) {
    std::cout << i << ": ";
    if (buckets_[i].size() > 0) {
      for (auto y : buckets_[i]) {
        std::cout << y << " ";
      }
      std::cout << std::endl;
    }
  }
};

bool CuckooHashTable::Insert(uint16_t fingerprint, size_t bucket_idx) {
  bucket_idx &= n_buckets_ - 1;
  if (buckets_[bucket_idx].size() >= bucket_size) {
    return false;
  }
  buckets_[bucket_idx].push_back(fingerprint);
  return true;
}

bool CuckooHashTable::Remove(uint16_t fingerprint, size_t bucket_idx) {
  bucket_idx &= n_buckets_ - 1;
  for (size_t i; i < buckets_[bucket_idx].size(); i++) {
    if (buckets_[bucket_idx][i] == fingerprint) {
      buckets_[bucket_idx].erase(buckets_[bucket_idx].begin() + i);
      return true;
    }
  }
  return false;
}

bool CuckooHashTable::Contains(uint16_t fingerprint, size_t bucket_idx) {
  bucket_idx &= n_buckets_ - 1;
  for (auto entry : buckets_[bucket_idx]) {
    if (entry == fingerprint) {
      return true;
    }
  }
  return false;
}

uint16_t CuckooHashTable::SwapEntries(uint16_t fingerprint, size_t bucket_idx) {
  bucket_idx &= n_buckets_ - 1;

  size_t i = std::rand() % buckets_[bucket_idx].size();
  uint16_t old_entry = buckets_[bucket_idx].at(i);
  buckets_[bucket_idx].at(i) = fingerprint;

  return old_entry;
}
}  // namespace cuckoofilter