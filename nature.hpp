#ifndef NATURE_HPP
#define NATURE_HPP
#include <vector>
#include <string.h>
#include "random.hpp"

class Nature{
    float min_food_gen;
    float max_food_gen;

    public:
    float food_available;

    Nature(float min_food_gen, float max_food_gen) : max_food_gen(max_food_gen), min_food_gen(min_food_gen) {}

    void generate_food(){
        food_available = min_food_gen+rand_f1()*(max_food_gen-min_food_gen);
    }


};

#endif
