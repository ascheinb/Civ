#ifndef MYUTILS_HPP
#define MYUTILS_HPP
#include <vector>

template<typename T>
inline int get_index(std::vector<T> v, int idx){
    for (int k=0;k<v.size();k++){
        if (v[k]==idx){
            return k;
        }
    }
    return -1;
}

#endif
