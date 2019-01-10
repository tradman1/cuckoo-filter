#ifndef CUCKOO_FILTER_CUCKOO_HASHTABLE_H_
#define CUCKOO_FILTER_CUCKOO_HASHTABLE_H_

#include <vector>
#include <unordered_set>

namespace cuckoofilter {

class CuckooHashTable {
  static const int bucket_size = 4;

  int capacity_;
  int n_buckets_;
  std::vector<std::unordered_set<uint32_t>> buckets_;

 public:
  explicit CuckooHashTable(const int capacity) : capacity_(capacity) {
    n_buckets_ = capacity / bucket_size + 1;
    // create n_buckets buckets
    buckets_.resize(n_buckets_, std::unordered_set<uint32_t>());
  }
  int NumBuckets() const { return n_buckets_; }

  int size() const {return buckets_.size();};

  int capacity() const {return buckets_.capacity();};

  // returns true if insertion was successful
  bool Insert(uint32_t fingerprint, int bucket_idx);
  // returns true if fingerprint at given index in given bucket was deleted
  bool Remove(uint32_t fingerprint, int bucket_idx);
  // returns true if the bucket at given index contains the fingerprint
  bool Contains(uint32_t fingerprint, int bucket_idx);
  // insert new fingerprint in bucket_idx and return the old fingerprint
  uint32_t SwapEntries(uint32_t fingerprint, int bucket_idx);
};

};  // namespace cuckoofilter

#endif