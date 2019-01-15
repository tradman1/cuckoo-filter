CXX=g++
CXXFLAGS=-Wall -std=c++11 -w
SOURCES=src/cuckoo_filter.cc  src/cuckoo_hasher.cc  src/cuckoo_hashtable.cc  src/main.cc  src/test.cc

all: test main benchmark

test: src/cuckoo_filter.cc  src/cuckoo_hasher.cc  src/cuckoo_hashtable.cc  src/test.cc
	$(CXX) $^ -o test $(CXXFLAGS)

main: src/cuckoo_filter.cc  src/cuckoo_hasher.cc  src/cuckoo_hashtable.cc  src/main.cc
	$(CXX) $^ -o main $(CXXFLAGS)

benchmark: benchmarks/benchmarks.cc src/cuckoo_hashtable.cc src/cuckoo_hasher.cc src/cuckoo_filter.cc 
	$(CXX) $^ -o benchmark $(CXXFLAGS)