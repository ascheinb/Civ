#ifndef MYUTILS_HPP
#define MYUTILS_HPP
#include <vector>

using std::vector;

template<typename T>
inline int get_index(vector<T> v, int idx){
    for (int k=0;k<v.size();k++){
        if (v[k]==idx){
            return k;
        }
    }
    return -1;
}

#include <iostream>
#include <numeric>      // std::iota
#include <algorithm>    // std::sort, std::stable_sort

template <typename T>
vector<size_t> sort_indexes(const vector<T> &v) {

  // initialize original index locations
  vector<size_t> idx(v.size());
  std::iota(idx.begin(), idx.end(), 0);

  // sort indexes based on comparing values in v
  // using std::stable_sort instead of std::sort
  // to avoid unnecessary index re-orderings
  // when v contains elements of equal values 
  std::stable_sort(idx.begin(), idx.end(),
       [&v](size_t i1, size_t i2) {return v[i1] < v[i2];});

  return idx;
}

#endif
