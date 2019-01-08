#include <cstdint>
#include <random>
#include <iostream>
#include "cuckoo_hashtable.h"

namespace cuckoofilter {

bool CuckooHashTable::Insert(uint16_t fingerprint, int bucket_idx) {
  if (buckets_[bucket_idx].size() >= bucket_size) {
    return false;
  }
  return buckets_[bucket_idx].insert(fingerprint).second;
}

bool CuckooHashTable::Remove(uint16_t fingerprint, int bucket_idx) {
  std::unordered_set<uint16_t> bucket = buckets_[bucket_idx];
  if(buckets_[bucket_idx].find(fingerprint) != buckets_[bucket_idx].end()) {
    buckets_[bucket_idx].erase(fingerprint);
    return true;
  } else {  
    return false;
  }
}

bool CuckooHashTable::Contains(uint16_t fingerprint, int bucket_idx) {
  return buckets_[bucket_idx].find(fingerprint) != buckets_[bucket_idx].end();
}

uint16_t CuckooHashTable::SwapEntries(uint16_t fingerprint, int bucket_idx) {
  std::vector<uint16_t> bucket_cpy;
  std::copy(buckets_[bucket_idx].begin(), buckets_[bucket_idx].end(), bucket_cpy.begin());
  std::cout << "Created vector copy" << std::endl;

  // random generator
  std::random_device rd;     // only used once to initialise (seed) engine
  std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
  std::uniform_int_distribution<int> uni(0, buckets_[bucket_idx].size() - 1); // guaranteed unbiased
  int swap_idx = uni(rng);

  uint16_t old_entry = bucket_cpy[swap_idx];
  buckets_[bucket_idx].erase(old_entry);
  buckets_[bucket_idx].insert(fingerprint);
  return old_entry;
}
}