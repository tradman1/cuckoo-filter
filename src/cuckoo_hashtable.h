#ifndef CUCKOO_FILTER_CUCKOO_HASHTABLE_H_
#define CUCKOO_FILTER_CUCKOO_HASHTABLE_H_

#include <unordered_set>
#include <vector>
#include <iostream>

namespace cuckoofilter {

class CuckooHashTable {
  static const size_t bucket_size = 4;

  size_t capacity_;
  size_t n_buckets_;
  std::vector<std::vector<uint32_t>> buckets_;

  inline size_t upperpower2(size_t x) {
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x |= x >> 32;
    x++;
  return x;
}


 public:
  explicit CuckooHashTable(size_t capacity) : capacity_(capacity) {
    capacity = upperpower2(capacity); // mandatory for cuckoo filter
    n_buckets_ = capacity / bucket_size;
    // create n_buckets buckets
    buckets_.resize(n_buckets_);
  }
  size_t NumBuckets() const { return n_buckets_; }

  size_t size() const { return buckets_.size(); };

  size_t capacity() const { return buckets_.capacity(); };

  void printTable() {
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

  // returns true if insertion was successful
  bool Insert(uint32_t fingerprint, size_t bucket_idx);
  // returns true if fingerprsize_t at given index in given bucket was deleted
  bool Remove(uint32_t fingerprint, size_t bucket_idx);
  // returns true if the bucket at given index contains the fingerprint
  bool Contains(uint32_t fingerprint, size_t bucket_idx);
  // insert new fingerprsize_t in bucket_idx and return the old fingerprint
  uint32_t SwapEntries(uint32_t fingerprint, size_t bucket_idx);
};

};  // namespace cuckoofilter

#endif