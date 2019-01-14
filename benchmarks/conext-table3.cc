// This benchmark reproduces the CoNEXT 2014 results found in "Table 3: Space efficiency
// and construction speed." It takes about two minutes to run on an Intel(R) Core(TM)
// i7-4790 CPU @ 3.60GHz.
//
// Results:
//
// metrics                                    CF     ss-CF
// # of items (million)                   127.82    127.90
// bits per item                           12.60     12.59
// false positive rate                     0.18%     0.09%
// constr. speed (million keys/sec)         5.86      4.10

// compile: g++ -fno-strict-aliasing -Wall -std=c++11 -I. -I../src/ -O3 -DNDEBUG -march=core-avx2 conext-table3.cc -o conext-table3.exe ../src/cuckoo_filter.cc ../src/cuckoo_hashtable.cc ../src/cuckoo_hasher.cc -Wall -lpthread -lssl -lcrypto

#include <climits>
#include <iomanip>
#include <vector>

#include "cuckoo_filter.h"
#include "cuckoo_filter.cc"
#include "random.h"
#include "timing.h"

using namespace std;

using namespace cuckoofilter;

// The number of items sampled when determining the false positive rate
const size_t FPR_SAMPLE_SIZE = 1000 * 1000;

struct Metrics {
  double add_count;  // # of items (million)
  double space;      // bits per item
  double fpr;        // false positive rate (%)
  double speed;      // const. speed (million keys/sec)
};

template<typename Table>
Metrics CuckooBenchmark(size_t add_count, const vector<uint64_t>& input) {
  Table cuckoo(add_count, (size_t)12, Hash::TIMS);
  auto start_time = NowNanos();

  // Insert until failure:
  size_t inserted = 0;
  while (inserted < input.size() && Ok == cuckoo.Insert(input[inserted])) ++inserted;

  auto constr_time = NowNanos() - start_time;

  // Count false positives:
  size_t false_positive_count = 0;
  size_t absent = 0;
  for (; inserted + absent < input.size() && absent < FPR_SAMPLE_SIZE; ++absent) {
    false_positive_count += (Ok == cuckoo.Lookup(input[inserted + absent]));
  }

  // Calculate metrics:
  const auto time = constr_time / static_cast<double>(1000 * 1000 * 1000);
  Metrics result;
  result.add_count = static_cast<double>(inserted) / (1000 * 1000);
  result.space = static_cast<double>(CHAR_BIT * cuckoo.SizeInBytes()) / inserted;
  result.fpr = (100.0 * false_positive_count) / absent;
  result.speed = (inserted / time) / (1000 * 1000);
  return result;
}

int main() {
  // Number of distinct values, used only for the constructor of CuckooFilter, which does
  // not allow the caller to specify the space usage directly. The actual number of
  // distinct items inserted depends on how many fit until an insert failure occurs.
  const size_t add_count = 127.78 * 1000 * 1000;

  // Overestimate add_count so we don't run out of random data:
  const size_t max_add_count = 2 * add_count;
  const vector<uint64_t> input = GenerateRandom64(max_add_count + FPR_SAMPLE_SIZE);

  // Calculate metrics:
  const auto cf = CuckooBenchmark<CuckooFilter<uint64_t>>(add_count, input);

  cout << setw(35) << left << "metrics " << setw(10) << right << "CF" << setw(10)
       << "ss-CF" << endl
       << fixed << setprecision(2) << setw(35) << left << "# of items (million) "
       << setw(10) << right << cf.add_count << setw(10) << "###" << endl
       << setw(35) << left << "bits per item " << setw(10) << right << cf.space
       << setw(10) << "###" << endl
       << setw(35) << left << "false positive rate " << setw(9) << right << cf.fpr << "%"
       << setw(9) << "###" << "%" << endl
       << setw(35) << left << "constr. speed (million keys/sec) " << setw(10) << right
       << cf.speed << setw(10) << "###" << endl;
}
