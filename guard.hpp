#ifndef GUARD_HPP
#define GUARD_HPP
#include <vector>
#include <string.h>
#include "random.hpp"
#include "person.hpp"
#include "names.hpp"

#define DEFEND 0
#define ATTACK 1

class Guard{
    public:

    int ind; // index in person list
    int nactions;
    int task;
    int target;
    int station;

    Guard(){}

    Guard(int ind,int nactions,int task, int station)
        : ind(ind), nactions(nactions), task(task), target(-1), station(station) {}

};

#endif
