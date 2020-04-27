#ifndef RELATIONSHIP_HPP
#define RELATIONSHIP_HPP
#include <vector>
#include <string.h>
#include <numeric>
#include "random.hpp"

using std::vector;

class Relationship{
    public:
    int person_id;
    int fondness_to;
    int fondness_of;
    bool child;

    Relationship(int person_id) : person_id(person_id), child(false), fondness_to(0), fondness_of(0) {}
};

/*template <typename Proc>
    float frac(Proc p){
        float sum=0.0;
        for(int i = 0; i<person.size();i++){
            sum += p(person[i]) ? 1.0 : 0.0;
        }
        return sum/person.size();
    }
*/
//template <typename Proc>
struct Perm{
    vector<int> x;

//    Perm(vector<Relationship>& rships, Proc p) : x(rships.size()) {
    Perm(vector<Relationship>& rships) : x(rships.size()) {
        // initialize original index locations
        std::iota(x.begin(), x.end(), 0);

        std::stable_sort(x.begin(), x.end(),
                [&rships](size_t i1, size_t i2) {
                    
                    return rships[i1].fondness_to > rships[i2].fondness_to;
                });
    }
};

#endif
