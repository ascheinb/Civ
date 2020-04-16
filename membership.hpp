#ifndef MEMBERSHIP_HPP
#define MEMBERSHIP_HPP
#include <vector>
#include <string.h>
#include "random.hpp"
#include "names.hpp"

class Membership{
    public:
    int id;
    float loyalty_to;

    Membership(int id,float loyalty_to) : id(id), loyalty_to(loyalty_to) {}
};

#endif
