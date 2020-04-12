#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <algorithm>
#include <vector>
#include <cstdlib>

float rand_f1(){
    return (float)(rand())/(float)(RAND_MAX);
}

struct RandPerm{
    std::vector<int> x;

    RandPerm(int n) : x(n) {
        for (int i=0; i<n; i++) x[i]=i;
        std::random_shuffle ( x.begin(), x.end() );
    }
};
#endif
