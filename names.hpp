#ifndef NAMES_HPP
#define NAMES_HPP
#include <vector>
#include <string.h>
#include <fstream>
#include <iostream>
#define NF_NAMES 100
#define NM_NAMES 100
#define NGROUP_NAMES 10

std::vector<std::string> names;
std::vector<std::string> gnames; // groups

void fill_names(){
	std::ifstream file("names.txt");
	std::string line;
	while(std::getline(file, line)) {
        names.push_back(line);
    }

    std::ifstream file2("gnames.txt");
    while(std::getline(file2, line)) {
        gnames.push_back(line);
    }
}

#endif
