#ifndef NAMES_HPP
#define NAMES_HPP
#include <vector>
#include <string.h>
#include <fstream>
#include <iostream>
#define NF_NAMES 100
#define NM_NAMES 100
#define NGROUP_NAMES 10

#include "random.hpp"

using std::vector;
using std::string;
using std::ifstream;
using std::getline;

vector<string> names;
vector<string> gnames; // groups

float v_cumul_freq[6];
float c_cumul_freq[20];
char ucons[21] = "BCDFGHJKLMNPQRSTVWXZ";
char lcons[21] = "bcdfghjklmnpqrstvwxz";
char uvows[7] = "AEIOUY";
char lvows[7] = "aeiouy";

void fill_names(){
	ifstream file("names.txt");
	string line;
	while(getline(file, line)) {
        names.push_back(line);
    }

    ifstream file2("gnames.txt");
    while(getline(file2, line)) {
        gnames.push_back(line);
    }

    // Initialize name generator
    // Letter frequency

    float v_letter_frequency[] = {8.167, 12.702, 6.966, 7.507, 2.758, 1.994};
    float c_letter_frequency[] = {1.492, 2.202, 4.253, 2.228, 2.015, 6.094, 0.153, 1.292, 4.025, 2.406, 6.749, 1.929, 0.095, 5.987, 6.327, 9.356, 0.978, 2.560, 0.150, 0.077};

    float lsum=0.0f;
    for (int i=0;i<6;i++) lsum+=v_letter_frequency[i];
    for (int i=0;i<6;i++) v_letter_frequency[i]/=lsum;
    v_cumul_freq[0]=v_letter_frequency[0];
    for (int i=1;i<6;i++){
        v_cumul_freq[i]=v_cumul_freq[i-1]+v_letter_frequency[i];
    }

    lsum=0.0f;
    for (int i=0;i<20;i++) lsum+=c_letter_frequency[i];
    for (int i=0;i<20;i++) c_letter_frequency[i]/=lsum;
    c_cumul_freq[0]=c_letter_frequency[0];
    for (int i=1;i<20;i++){
        c_cumul_freq[i]=c_cumul_freq[i-1]+c_letter_frequency[i];
    }
}

int generate_letter(int nletters,float* cumul_freq){
    float rnum = rand_f1(); // between 0.0 and 1.0
    for (int i=0;i<nletters;i++){
        if (rnum<cumul_freq[i]){
            return i;
        }
    }
    return nletters-1;
}

string generate_name(){
    string new_name;
    int namelength=5+rand_int(3);
    bool wasvow=true;
    bool isvow=false;
    for (int i=0;i<namelength;i++){
        if (!wasvow && !isvow) // CC
            {wasvow=isvow;isvow=true;}
        else if (!wasvow && isvow) // CA
            {wasvow=isvow;isvow=false;}
        else if (i==namelength-1) // AC (dont end in ACC)
            {wasvow=isvow;isvow=true;}
        else // AC
            {wasvow=isvow;isvow=(rand_int(3)==0);} // Prefer ACCA to ACA by 2 to 1

        if (i==0){
            if (isvow) new_name += uvows[generate_letter(6,v_cumul_freq)];
            else new_name += ucons[generate_letter(20,c_cumul_freq)];
        } else {
            if (isvow) new_name += lvows[generate_letter(6,v_cumul_freq)];
            else new_name += lcons [generate_letter(20,c_cumul_freq)];
        }
    }
    return new_name;
}

#endif
