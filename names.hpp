#ifndef NAMES_HPP
#define NAMES_HPP
#include <vector>
#include <string.h>
#include <fstream>
#include <iostream>
#define NF_NAMES 100
#define NM_NAMES 100

std::vector<std::string> names;

void fill_names(){
	std::ifstream file("names.txt");
	std::string line;
	while(std::getline(file, line)) {
        names.push_back(line);
    }
}

#endif
