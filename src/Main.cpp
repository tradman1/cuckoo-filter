// CF.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
using namespace std;

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
	for (int i = 0; i < (genome.length() - k + 1); i++)
	{
		std::string k_substring = genome.substr(i, k);
		cout << k_substring + "\n"; //umjesto printa insertati u cuckoo_filtar 
	}
}

int main(int argc, char* argv[])
{
	string genome = upload_genome("D:\\3.semestar\\BIOINF\\coli\\genom_short_test.fna");
	//string genome = upload_genome("D:\\3.semestar\\BIOINF\\coli\\GCF_000005845.2_ASM584v2_genomic.fna");
	int k = atoi(argv[1]);
	fill_cuckoo_filtar(genome, k);
	return 0;
}
