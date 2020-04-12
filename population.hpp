#ifndef POPULATION_HPP
#define POPULATION_HPP
#include <vector>
#include <string.h>
#include "random.hpp"
#include "person.hpp"

class Population{
    int n_ids;
    int lifespan;
    int fertility_age;
    float fertility_rate; // births/turn

    public:

    std::vector<Person> person; // Public so people can see each other (use friend instead?)

    Population(int initial_n_ppl) : n_ids(initial_n_ppl) {
        // Population-wide traits
        lifespan = 240;
        fertility_age = 64;
        fertility_rate = 2.2/(lifespan-fertility_age);

        // Initialize individuals
        for(int i = 0; i<initial_n_ppl;i++)
            person.push_back(Person(i,i,lifespan,0));
    }

    void do_long_actions(int& food_available) {
        RandPerm rp(person.size());
        for(int i = 0; i<person.size();i++){
            int ri = rp.x[i];
            person[ri].do_long_action(food_available);
        }
    }

    void check_starvation() {
        for(int i = 0; i<person.size();i++){
            if (person[i].wealth==0)
                person[i].will_starve=true;
        }
    }

    void eat() {
        for(int i = 0; i<person.size();i++){
            if (!person[i].will_starve)
                person[i].wealth--; // Eat 1 wealth
        }
    }

    void age() {
        for(int i = 0; i<person.size();i++){
            person[i].age++;
        }
    }

    void die() {
        for(int i = person.size()-1; i>=0;i--){
            if (     person[i].age>person[i].lifespan // Old age
                  || person[i].will_starve  ){        // Starvation
                person.erase(person.begin() + i); // Remove from vector
            }
        }
    }

    int breed() {
        int n_kids = 0;
        for(int i = 0; i<person.size();i++){
            n_kids += person[i].breed(n_ids+n_kids,fertility_age, fertility_rate, person);
        }
        n_ids+=n_kids;
        return n_kids;
    }

    void report(int i_turn){
        int n_male=0; int n_female=0;
        for(int i = 0; i<person.size();i++){
            if (person[i].female) n_female++;
            else n_male++;
        }
        printf("\nTurn %d: live female/male, dead: %d/%d, %d", i_turn, n_female, n_male, n_ids-person.size());
        snap_age("ages",i_turn);
    }

    private:

    // Write properties to file
    void snap_age(const char* filebase, int i_turn){
        std::string turn_str = std::to_string(i_turn);
        char filename[256]="";
        strcat(filename, filebase);
        strcat(filename, turn_str.c_str());
        strcat(filename, ".txt");
        FILE * pFile;
        pFile = fopen (filename,"w");
        for (int i=0 ; i<person.size() ; i++)
        {
            fprintf (pFile, "%d\n",person[i].age);
        }
        fclose (pFile);
    }

};
#endif
