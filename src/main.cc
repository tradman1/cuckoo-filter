#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <cstdlib>
#include <string>
#include <ctime>
#include <functional>
#include <algorithm> 
#include <assert.h>
#include "cuckoo_filter.h"
#include "cuckoo_hasher.h"
#include "cuckoo_hashtable.h"
#include "cuckoo_filter.cc"
#include "random.h"
#include "timing.h"

using namespace std;

using namespace cuckoofilter;

ofstream outFile;

size_t total_items;
size_t bits = 12;

//upload string from file 
string upload_genome(string path)
{
	string genome = "";
	string line;
	ifstream myfile;
	myfile.open(path, ios::in);
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			genome += line;
		}
		myfile.close();
	}
	else cout << "Unable to open file";
	return genome;
}

std::string generate_random_string (int length)
{
    auto randchar = []() -> char
    {
        const char charset[] =
        "ACGT";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(length,0);
    std::generate_n( str.begin(), length, randchar );
    return str;
}

int main(int argc, char* argv[])
{
	//upload_genome("D:\\3.semestar\\BIOINF\\coli\\GCF_000005845.2_ASM584v2_genomic.fna");
	int k = atoi(argv[2]);
    int algorithm = atoi(argv[3]);
	//string genome = upload_genome("/home/jiva26/Documents/bioinf/GCF_000005845.2_ASM584v2_genomic.fna");
	string genome = upload_genome(argv[1]);
	total_items = genome.length()/2 - k + 1;
    string genome1 = genome.substr(0, genome.length()/2);
    string genome2 = genome.substr(genome.length()/2, genome.length()/2);
	outFile.open("results_new2.txt", ios::out | ios::app );
	//outFile << "New test...uploaded genome from file: " << argv[1] << ", k = " << k << ", total_items = " << total_items << ", algorithm = " << algorithm <<endl;
	CuckooFilter<std::string> filter(total_items, bits, Hash::STL);
    auto start_time = NowNanos();
    //Fill Cuckoo Filtar
    size_t num_inserted = 0;
    outFile << k << ";" << algorithm << ";";
	for (size_t i = 0; i < total_items; i++, num_inserted++)
	{
		std::string k_substring = genome1.substr(i, k);
		//std::cout << k_substring << "\n";
		if (filter.Insert(k_substring) != Ok)
		{
            //outFile << "Number of inserted: " << num_inserted << endl;
			//outFile << "Inserted: " << 100.0 * num_inserted / total_items << "% items" << endl;
            break;
		}
	}
    outFile << num_inserted << ";" << 100.0 * num_inserted / total_items << ";";
    auto constr_time = NowNanos() - start_time;
	//Check if string is in filtar
    size_t false_queries = 0;
	srand(time(NULL));
	//check if in cuckoo filtar
	for (size_t i = 0; i < total_items; i++) {
        std::string random_string;
        if (algorithm == 1) {
            random_string = genome2.substr(i, k);
        } else {
            random_string = generate_random_string(k);
        }
		if (filter.Lookup(random_string) == Ok) 
		{
			false_queries++;
    	}
	}
	//outFile << "False positive rate = " << 100.0 * false_queries / total_items << "%\n";
	const auto time = constr_time / static_cast<double>(1000 * 1000 * 1000);
    //outFile << "Speed: " << (num_inserted / time) / (1000 * 1000) << " million keys/sec" << endl;
    //cout << (float) false_queries / num_inserted * 100;
    outFile << (float) false_queries / num_inserted * 100 << ";" << (num_inserted / time) / (1000 * 1000) << endl;
    //outFile << "" << endl;
    return 0;
}