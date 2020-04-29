#ifndef MYUTILS_HPP
#define MYUTILS_HPP
#include <vector>
#include <string.h>

using std::vector;
using std::string;

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


#include <sys/time.h>
typedef unsigned long long timestamp_t;

static timestamp_t
get_timestamp ()
{
  struct timeval now;
  gettimeofday (&now, NULL);
  return  now.tv_usec + (timestamp_t)now.tv_sec * 1000000;
}

struct Timer{
    string name;
    timestamp_t tstamp_start;
    double time_spent;

    Timer(string str) : name(str),time_spent(0.0) {}

    void start(){
        tstamp_start=get_timestamp();
    }

    void stop(){
        time_spent+=(get_timestamp()-tstamp_start)/1000000.0L;
    }
};

class TimerManager
{
    vector<Timer> timers;
    int i_timer;
    bool entered_scope=false;
    timestamp_t tstamp_scope;
    double time_in_scope;

    public:

    void scope(){
        i_timer=0;
        if (entered_scope){
            time_in_scope+=(get_timestamp()-tstamp_scope)/1000000.0L;
        } else {
            entered_scope=true;
        }
        tstamp_scope=get_timestamp();
    }

    void start(string str){
        if (i_timer>=timers.size()){ // New timers
            timers.push_back(str);
        }
        timers[i_timer].start();
    }

    void stop(){
        timers[i_timer].stop();
        i_timer++;
    }

    void print(){
        printf("\nTimers: ");
        double total_timed=0.0;
        for (int i=0;i<timers.size();i++){
            total_timed+=timers[i].time_spent;
            printf("\n   %s: %.2f/%.2f = %.1f%%; ",timers[i].name.c_str(),timers[i].time_spent,time_in_scope,timers[i].time_spent/time_in_scope*100);
        }
        printf("\n   Coverage: %.1f%%",total_timed/time_in_scope*100);
    }
};

#endif
