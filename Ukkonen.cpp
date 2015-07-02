// Ukkonen.cpp : Defines the entry point for the console application.
//
#include <iostream>
#include <fstream>
#include <ctime>
#include <suffixtree.h>

using namespace std;

void run_test_on_string(std::string);
void run_test_on_file(std::string);
void run_suffix_prefix_test_on_string(std::vector<std::string>);
void run_test_on_substring(std::string);

int main()
{
	// test ispisa stabla
	SuffixTree n("ABAABACDA");
	n.print_tree(std::cout);

	std::cout << std::endl;
	run_test_on_string("abaab");
	run_test_on_string("abcabxabcd");
	run_test_on_string("abaababaaabaaabbcaa");
	run_test_on_string("abbbaba");

	run_test_on_file("C:/Users/Tomislav/izlaz1000.txt");
	
	std::vector<std::string> rez;
	std::string r1(1000000, 'A');
	std::string r2(100000, 'A');
	rez.push_back("B" + r1);
	rez.push_back(r2);
	run_suffix_prefix_test_on_string(rez);

	run_test_on_substring("abaab");
	system("pause");
}

void run_test_on_string(std::string str) {
	std::cout << "Running test -all suffixes- on: " << str << std::endl;
	SuffixTree n(str);
	std::vector<std::string> suff = n.get_all_suffixes();
	bool ok = true;
	for (std::size_t i = 0; i < str.size(); i++) {
		std::cout << "\t" << suff[i] << std::endl;
		if (str.substr(i) != (suff[i])) {
			ok = false;
		}
	}

	if(!ok) std::cout << "\tTEST FAILED!\n";
	else std::cout << "\tTEST PASSED\n";
}

void run_test_on_file(std::string path) {
	string str;
	ifstream infile;
	infile.open (path);
    getline(infile, str);
	infile.close();
    std::clock_t    start;

	std::cout << "Running test -all suffixes- on " << str.length() << " characters." << std::endl;
    start = std::clock();
	SuffixTree n(str);
	std::cout << "\tTime: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;

	std::vector<std::string> suff = n.get_all_suffixes();
	bool ok = true;
	for (int i = 0; i < str.size(); i++) {
		if (str.substr(i)  != (suff[i])) {
			ok = false;
		}
	}

	if(!ok) std::cout << "\tTEST FAILED!\n";
	else std::cout << "\tTEST PASSED\n";
}

void run_suffix_prefix_test_on_string(std::vector<std::string> strings) {
	static const int limit = 15;
	std::cout << "Running test -all suffix prefix matches- on: " << std::endl;
	for (std::size_t i = 0; i < strings.size(); i++) {
	//	std::cout << "\t" << strings[i] << std::endl;
	}
	std::cout << "Matches: " << std::endl;
	SuffixTree n(strings);

	bool ok = true;
	std::vector< std::pair<int, int> > m = n.get_suffix_prefix_matches();
	if(m.size() > limit) {
		std::cout <<  "\t" << m.size() << " matches found." << std::endl;
	}
	else {
		for (std::size_t i = 0; i < m.size(); i++) {
			std::cout << "\t" << strings[m[i].first].substr(0, m[i].second) << std::endl;
		}
	}
	if(!ok) std::cout << "\tTEST FAILED!\n";
	else std::cout << "\tTEST PASSED\n";
}

void run_test_on_substring(std::string str) {
	std::cout << "Running test -all substrings- on: " << str << std::endl;
	SuffixTree n(str);
	bool ok = true;
	for (int i = 0; i < str.size(); i++) {
		for(int j = i; j < str.size(); j++) {
			std::string substr = str.substr(i, j-i+1);
			std::cout << "\t" << substr << std::endl;
			if(!n.contains_substring(substr)) {
				ok = false;
			}
		}
	}

	if(!ok) std::cout << "\tTEST FAILED!\n";
	else std::cout << "\tTEST PASSED\n";
}
