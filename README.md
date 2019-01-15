# cuckoo-filter

Implementation of [Cuckoo Filter](https://www.cs.cmu.edu/~dga/papers/cuckoo-conext2014.pdf), a probabilistic data structure used for fast set membership testing.

This is a project for Bionformatics course at FER, University of Zagreb. You can visit the course website [here](https://www.fer.unizg.hr/predmet/bio).

Cuckoo filter is a Bloom filter replacement for approximated set-membership queries. Unlike Bloom filter, Cuckoo ﬁlters provide the ﬂexibility to add and remove items dynamically. A cuckoo filter is based on cuckoo hashing and it is essentially a cuckoo hash table storing each key's fingerprint. Cuckoo hash tables can be highly compact and can use less space than Bloom ﬁlters, which is useful for applications that require low false positive rates.

Build
-------

To build the example test:
```bash
$ make test
```

To build the benchmarks:
```bash
$ make benchmark
```

To build the main program for genome testing:
```bash
$ make main
```

Cuckoo filter can be tested with command ./main in terminal with next parametrs:
    • Apsolute path to genome txt:string 
    • Length of k-meres:int
    • Algorithm:int (it represents wheater lookup will be done with random generated strings or substrings from uploaded genome):
            ▪ 1 – substrings from genome 
            ▪ else – random strings  
    Program will generate output .txt file that will write :
            ▪ Number of inserted substrings
            ▪ Percentage of inserted substrings
            ▪ Percentage of false positive rates
            ▪ Execution speed 

Authors
-------
- Iva Jurković [Jiva26](https://github.com/Jiva26)
- Tome Radman [tradman1](https://github.com/tradman1)
- Antonio Šajatović [asajatovic](https://github.com/asajatovic)