#include <cstdint>
#include <random>
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
  if(bucket.find(fingerprint) != bucket.end()) {
    bucket.erase(fingerprint);
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
  std::unordered_set<uint16_t> bucket = buckets_[bucket_idx];
  std::copy(bucket.begin(), bucket.end(), bucket_cpy.begin());

  // random generator
  std::random_device rd;     // only used once to initialise (seed) engine
  std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
  std::uniform_int_distribution<int> uni(0, bucket.size() - 1); // guaranteed unbiased
  int swap_idx = uni(rng);

  uint16_t old_entry = bucket_cpy[swap_idx];
  bucket.erase(old_entry);
  bucket.insert(fingerprint);
  return old_entry;
}
}