#include "cuckoo_hasher.h"
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

#include <climits>
#include <functional>
#include <random>
#include <string>

namespace cuckoofilter {

CuckooHasher::CuckooHasher() { hash_function_ = Hash::STL; };

CuckooHasher::CuckooHasher(Hash hash_function) {
  hash_function_ = hash_function;

  // initialization for twoIndependentMultiplyShift
  if (hash_function == Hash::TIMS) {
    ::std::random_device random;
    for (auto v : {&multiply_, &add_}) {
      *v = random();
      for (int i = 1; i <= 4; ++i) {
        *v = *v << 32;
        *v |= random();
      }
    }
  }

  // // initialization for simpleTabulation
  if (hash_function == Hash::ST) {
    ::std::random_device random;
    for (unsigned i = 0; i < sizeof(uint64_t); ++i) {
      for (int j = 0; j < (1 << CHAR_BIT); ++j) {
        tables_[i][j] = random() | ((static_cast<uint64_t>(random())) << 32);
      }
    }
  }
};

uint64_t CuckooHasher::hash(uint64_t item) {
  if (hash_function_ == Hash::STL) {
    return std::hash<std::uint64_t>{}(item);
  }

  if (hash_function_ == Hash::ST) {
    return simpleTabulation(item);
  }

  if (hash_function_ == Hash::TIMS) {
    return twoIndependentMultiplyShift(item);
  }
  return item;
}

uint64_t CuckooHasher::hash(std::string item) {
  if (hash_function_ == Hash::DJB2) {
    return djb2(item);
  }
  if (hash_function_ == Hash::STL) {
    return std::hash<std::string>{}(item);
  }
}

uint64_t CuckooHasher::djb2(std::string &item) {
  uint64_t hash = 5381;
  for (auto c : item) {
    hash = (hash << 5) + hash + c; /* hash * 33 + c */
  }
  return hash;
}

uint64_t CuckooHasher::twoIndependentMultiplyShift(uint64_t key) {
  return (add_ + multiply_ * static_cast<decltype(multiply_)>(key)) >> 64;
}

uint64_t CuckooHasher::simpleTabulation(uint64_t key) {
  uint64_t result = 0;
  for (unsigned i = 0; i < sizeof(key); ++i) {
    result ^= tables_[i][reinterpret_cast<uint8_t *>(&key)[i]];
  }
  return result;
};
}  // namespace cuckoofilter