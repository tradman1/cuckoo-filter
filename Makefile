CXX=g++
CXXFLAGS=-Wall -std=c++17 -w
SOURCES=src/cuckoo_filter.cc  src/cuckoo_hasher.cc  src/cuckoo_hashtable.cc  src/main.cc  src/test.cc

test: src/cuckoo_filter.cc  src/cuckoo_hasher.cc  src/cuckoo_hashtable.cc  src/test.cc
	$(CXX) $^ -o test $(CXXFLAGS)

main: src/cuckoo_filter.cc  src/cuckoo_hasher.cc  src/cuckoo_hashtable.cc  src/main.cc
	$(CXX) $^ -o main $(CXXFLAGS)