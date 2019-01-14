#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <iostream>
#include <vector>

#include <climits>
#include <functional>
#include <random>
#include <string>

#pragma once

namespace cuckoofilter {

class CuckooHashTable {
  static const size_t bucket_size = 4;

  size_t capacity_;
  size_t n_buckets_;
  std::vector<std::vector<uint16_t>> buckets_;

  // http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2Float
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
  CuckooHashTable(size_t capacity);
  size_t NumBuckets();
  size_t size();
  size_t capacity();
  size_t SizeInBytes();
  void printTable();

  // returns true if insertion was successful
  bool Insert(uint16_t fingerprint, size_t bucket_idx);
  // returns true if fingerprsize_t at given index in given bucket was deleted
  bool Remove(uint16_t fingerprint, size_t bucket_idx);
  // returns true if the bucket at given index contains the fingerprint
  bool Contains(uint16_t fingerprint, size_t bucket_idx);
  // insert new fingerprsize_t in bucket_idx and return the old fingerprint
  uint16_t SwapEntries(uint16_t fingerprint, size_t bucket_idx);
};

}  // namespace cuckoofilter
