#ifndef RELATIONSHIP_HPP
#define RELATIONSHIP_HPP
#include <vector>
#include <string.h>
#include "random.hpp"

class Relationship{
    public:
    int person_id;
    int fondness_to;
    int fondness_of;
    bool child;

    Relationship(int person_id) : person_id(person_id), child(false) {}
};

#endif
