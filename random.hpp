#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <algorithm>
#include <vector>
#include <cstdlib>

float rand_f1(){
    float x = (float)(rand())/(float)(RAND_MAX);
    return ((x == 1.0) ? (1.0-1.0e-10) : x);
}

int rand_int(int cap){ // Range from 0 to cap-1
    return (int)(rand_f1()*cap);
}

bool chance(float probability){
    return rand_f1()<probability;
}

struct RandPerm{
    std::vector<int> x;

    RandPerm(int n) : x(n) {
        for (int i=0; i<n; i++) x[i]=i;
        std::random_shuffle ( x.begin(), x.end() );
    }
};
#endif
