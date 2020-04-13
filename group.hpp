#ifndef GROUP_HPP
#define GROUP_HPP
#include <vector>
#include <string.h>
#include "random.hpp"
#include "person.hpp"
#include "names.hpp"

class Group{
    public:
    // fixed
    int id;
    int name;

    // changing
    float wealth;
    int land;
    float deployment;
    int ndeployments;
    int nundefended;

    Group(int id, int land) : id(id), wealth(0), land(land), deployment(20),ndeployments(0) {
        name = (rand_f1()*NGROUP_NAMES);
    }

    
};

#endif
