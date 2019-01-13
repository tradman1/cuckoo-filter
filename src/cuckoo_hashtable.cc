#include "cuckoo_hashtable.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <cstdint>
#include <random>

#include <climits>
//#include <openssl/evp.h>
#include <functional>
#include <random>
#include <string>

namespace cuckoofilter {

CuckooHashTable::CuckooHashTable(size_t capacity) : capacity_(capacity) {
  // for cuckoo filter to work, it must have number of buckets that is power of
  // 2
  n_buckets_ = upperpower2((uint64_t)capacity / bucket_size);
  double load_factor = (double)capacity / n_buckets_ / bucket_size;
  if (load_factor > 0.95) {
    n_buckets_ *= 2;
  }
  std::cout << "Load factor is " << load_factor << std::endl;
  std::cout << "Number of buckets is: " << n_buckets_ << std::endl;
  buckets_.resize(n_buckets_);  // allocate space for n_buckets buckets
}

size_t CuckooHashTable::NumBuckets() { return n_buckets_; }

size_t CuckooHashTable::size() { return buckets_.size(); };

size_t CuckooHashTable::capacity() { return buckets_.capacity(); };

void CuckooHashTable::printTable() {
  // for (std::vector<std::unordered_set<uint32_t> >::iterator it =
  // buckets_.begin(); it != buckets_.end(); it++) {
  for (int i = 0; i < buckets_.size(); i++) {
    std::cout << i << ": ";
    // for (auto x : buckets_) {
    // if (x.size() > 0) {
    if (buckets_[i].size() > 0) {
      for (auto y : buckets_[i]) {
        std::cout << y << " ";
      }
      std::cout << std::endl;
    }
  }
};

bool CuckooHashTable::Insert(uint32_t fingerprint, size_t bucket_idx) {
  bucket_idx %= n_buckets_;
  // bucket_idx = bucket_idx & (n_buckets_ - 1);
  if (buckets_[bucket_idx].size() >= bucket_size) {
    return false;
  }
  buckets_[bucket_idx].push_back(fingerprint);
  return true;
}

bool CuckooHashTable::Remove(uint32_t fingerprint, size_t bucket_idx) {
  bucket_idx %= n_buckets_;
  // bucket_idx = bucket_idx & (n_buckets_ - 1);
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
  // bucket_idx = bucket_idx & (n_buckets_ - 1);
  for (auto entry : buckets_[bucket_idx]) {
    if (entry == fingerprint) {
      return true;
    }
  }
  return false;
}

uint32_t CuckooHashTable::SwapEntries(uint32_t fingerprint, size_t bucket_idx) {
  // std::cout << "number of buckets is " << n_buckets_ << std::endl;
  // assert(bucket_idx % n_buckets_ == bucket_idx & (n_buckets_ - 1));
  // bucket_idx = bucket_idx & (n_buckets_ - 1);
  bucket_idx %= n_buckets_;

  size_t i = std::rand() % buckets_[bucket_idx].size();
  uint32_t old_entry = buckets_[bucket_idx].at(i);
  buckets_[bucket_idx].at(i) = fingerprint;

  std::cout << "swapping fingerprint " << fingerprint << " with " << old_entry
            << " at index " << bucket_idx << " whose mod is "
            << bucket_idx % NumBuckets() << std::endl;
  return old_entry;
}
}  // namespace cuckoofilter