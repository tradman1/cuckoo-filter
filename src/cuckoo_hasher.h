#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

#include <climits>
//#include <openssl/evp.h>
#include <functional>
#include <random>
#include <string>

#pragma once

namespace cuckoofilter {

enum class Hash { STL, DJB2, TIMS, ST };

class CuckooHasher {
 private:
  Hash hash_function_;
  unsigned __int128 multiply_;
  unsigned __int128 add_;
  uint64_t tables_[sizeof(uint64_t)][1 << CHAR_BIT];

 public:
  CuckooHasher();
  CuckooHasher(Hash hash_function);

  // uint64_t operator()(std::string item) { return hash(item); }
  // uint64_t operator()(uint64_t item) { return hash(item); }

  uint64_t hash(std::string item);
  uint64_t hash(uint64_t item);

  // http://www.cse.yorku.ca/~oz/hash.html
  uint64_t djb2(std::string &item);

  // See Martin Dietzfelbinger, "Universal hashing and k-wise independent random
  // variables via integer arithmetic without primes".
  uint64_t twoIndependentMultiplyShift(uint64_t key);

  // See Patrascu and Thorup's "The Power of Simple Tabulation Hashing"
  uint64_t simpleTabulation(uint64_t key);
};
}  // namespace cuckoofilter