#ifndef CUCKOO_FILTER_HASHUTIL_H_
#define CUCKOO_FILTER_HASHUTIL_H_

#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

#include <climits>  //#include <openssl/evp.h>
#include <functional>
#include <random>
#include <string>

namespace cuckoofilter {
//  string based hashes

//  Default C++ string hashing algorithm
class StdStringHash {
  std::hash<std::string> hasher_;

 public:
  uint64_t operator()(const std::string &item) const { return hasher_(item); }
};

//  djb2 hashing algorithm
class DJB2Hash {
 public:
  uint64_t operator()(const std::string &item) const {
    uint64_t hash = 5381;
    for (auto c : item) {
      hash = (hash << 5) + hash + c; /* hash * 33 + c */
    }
    return hash;
  }
};

// uint based hashes

// See Martin Dietzfelbinger, "Universal hashing and k-wise independent random
// variables via integer arithmetic without primes".
class TwoIndependentMultiplyShift {
  unsigned __int128 multiply_;
  unsigned __int128 add_;
  // u128 multiply_, add_;

 public:
  TwoIndependentMultiplyShift() {
    ::std::random_device random;
    for (auto v : {&multiply_, &add_}) {
      *v = random();
      for (int i = 1; i <= 4; ++i) {
        *v = *v << 32;
        *v |= random();
      }
    }
  }

  uint64_t operator()(uint64_t key) const {
    return (add_ + multiply_ * static_cast<decltype(multiply_)>(key)) >> 64;
  }
};

// See Patrascu and Thorup's "The Power of Simple Tabulation Hashing"
class SimpleTabulation {
  uint64_t tables_[sizeof(uint64_t)][1 << CHAR_BIT];

 public:
  SimpleTabulation() {
    ::std::random_device random;
    for (unsigned i = 0; i < sizeof(uint64_t); ++i) {
      for (int j = 0; j < (1 << CHAR_BIT); ++j) {
        tables_[i][j] = random() | ((static_cast<uint64_t>(random())) << 32);
      }
    }
  }

  uint64_t operator()(uint64_t key) const {
    uint64_t result = 0;
    for (unsigned i = 0; i < sizeof(key); ++i) {
      result ^= tables_[i][reinterpret_cast<uint8_t *>(&key)[i]];
    }
    return result;
  }
};
}  // namespace cuckoofilter

#endif  // CUCKOO_FILTER_HASHUTIL_H_