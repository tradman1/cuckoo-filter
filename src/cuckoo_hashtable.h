#ifndef CUCKOO_FILTER_CUCKOO_HASHTABLE_H_
#define CUCKOO_FILTER_CUCKOO_HASHTABLE_H_

#include <vector>

namespace cuckoofilter {

class CuckooHashTable {
  static const int bucket_size = 4;

  int capacity_;
  int n_buckets_;
  std::vector<std::vector<uint16_t>> buckets_;

 public:
  explicit CuckooHashTable(const int capacity) : capacity_(capacity) {
    n_buckets_ = capacity / bucket_size + 1;
    // initialize buckets to be of size [n_buckets, bucket_size]
    buckets_.resize(n_buckets_, std::vector<uint16_t>(bucket_size));
  }
  int NumBuckets() const {
    return n_buckets_;
  }

  // returns true if insertion was successful
  bool Insert(uint16_t fingerprint, int bucket_idx);
  // returns true if fingerprint at given index in given bucket was succesfully deleted
  bool Remove(uint16_t fingerprint, int bucket_idx);
  // returns true if the bucket at given index contains the fingerprint
  bool Contains(uint16_t fingerprint, int bucket_idx);

};

};  // namespace cuckoofilter

#endif