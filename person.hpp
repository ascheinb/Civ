#ifndef PERSON_HPP
#define PERSON_HPP
#include <vector>
#include <string.h>
#include "random.hpp"

class Person{
    public:
    // fixed
    int id;
    int lifespan;
    bool female;

    // changing
    int age;
    bool will_starve;

    int wealth; // temporary

    // Initial generation
    Person(int id, int age, int lifespan, int wealth) : id(id), age(age), lifespan(lifespan), will_starve(false), wealth(wealth) {
        female = (rand_f1()<0.5); // 50% chance of being female
    }

    // Birth
    Person(int id, Person* mom, Person& dad) : id(id), age(0), will_starve(false), wealth(0) {
        lifespan = (mom->lifespan+dad.lifespan)/2;
        female = (rand_f1()<0.5); // 50% chance of being female
    }


    void do_long_action(int& food_available){
        // FORAGE
        if (food_available>0){
            wealth+=2;
            food_available-=2;
        }
    }

    int breed(int next_id, int fertility_age, float fertility_rate, std::vector<Person>& people) {
        if (female && age>=fertility_age){ // If female and old enough
            if (rand_f1()<fertility_rate){ // If having children
                // Find father at random
                RandPerm rp(people.size());
                int dad_id = -1;
                for (int idad = 0; idad<people.size(); idad++){
                    int ri = rp.x[idad];
                    if(people[ri].age>=fertility_age && !people[ri].female) // adult male, good enough
                        {dad_id=ri; break;}
                }
                if (dad_id>=0) { // found a father
                    // Create kid
                    people.push_back(Person(next_id,this,people[dad_id]));
                    return 1;
                }
            }
        }
        return 0;
    }

};

#endif
