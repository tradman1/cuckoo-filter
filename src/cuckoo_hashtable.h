#ifndef CUCKOO_FILTER_CUCKOO_HASHTABLE_H_
#define CUCKOO_FILTER_CUCKOO_HASHTABLE_H_

#include <unordered_set>
#include <vector>

namespace cuckoofilter {

class CuckooHashTable {
  static const size_t bucket_size = 4;

  size_t capacity_;
  size_t n_buckets_;
  std::vector<std::unordered_set<uint32_t>> buckets_;

 public:
  explicit CuckooHashTable(const size_t capacity) : capacity_(capacity) {
    n_buckets_ = capacity / bucket_size + 1;
    // create n_buckets buckets
    buckets_.resize(n_buckets_, std::unordered_set<uint32_t>());
  }
  size_t NumBuckets() const { return n_buckets_; }

  size_t size() const { return buckets_.size(); };

  size_t capacity() const { return buckets_.capacity(); };

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