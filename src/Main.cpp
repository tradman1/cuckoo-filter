#include <iostream>

#include <fstream>

#include <cstdlib>

#include <string>

#include <ctime>

#include "cuckoo_filter.h"

#include "cuckoo_hashtable.cc"

#include "cuckoo_hashtable.h"

#include "hashutil.h"



using namespace std;

using namespace cuckoofilter;



size_t total_items;

CuckooFilter<std::string, StdStringHash> filter(total_items);



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



void fill_cuckoo_filtar(string genome, int k)

{	

	size_t num_inserted = 0;



	for (size_t i = 0; i < (genome.length() - k + 1); i++, num_inserted++)

	{

		std::string k_substring = genome.substr(i, k);

		//std::cout << k_substring << "\n";

		if (filter.Insert(k_substring) != Ok)

		{

			std::cout << "Inserted " << 100.0 * num_inserted / total_items << "% items" << std::endl;

			break;

		}

	}

}



void check_if_in_filtar(string genome, int k) 

{

	size_t false_queries = 0;

	srand(time(0));

	//check if in cuckoo filtar 

	for (size_t i = 0; i < total_items; i++) {



		string random_string = genome.substr((rand() % (genome.length() - k + 1) + 0), k);

		

		if (filter.Lookup(random_string) == Ok) 

		{

			false_queries++;

    	}

	}

	std::cout << "False positive rate = " << 100.0 * false_queries / total_items << "%\n";

}



int main(int argc, char* argv[])

{

	//upload_genome("D:\\3.semestar\\BIOINF\\coli\\GCF_000005845.2_ASM584v2_genomic.fna");

	int k = atoi(argv[1]);

	string genome = upload_genome("/home/jiva26/Documents/bioinf/GCF_000005845.2_ASM584v2_genomic.fna");

	total_items = genome.length() - k + 1;

	fill_cuckoo_filtar(genome, k);

	check_if_in_filtar(genome, k);

	return 0;

}