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
    int max_rships;

    public:

    std::vector<Person> person; // Public so people can see each other (use friend instead?)
    std::vector<int> id2ind; // map from id to index

    Population(int initial_n_ppl) : n_ids(initial_n_ppl) {
        // Population-wide traits
        lifespan = 240;
        fertility_age = 64;
        fertility_rate = 4.2/(lifespan-fertility_age);
        max_rships=50;

        // Initialize individuals
        for(int i = 0; i<initial_n_ppl;i++)
            person.push_back(Person(i,(i*lifespan)/initial_n_ppl,lifespan,0));

        // Initialize id to index mapping
        for(int i = 0; i<initial_n_ppl;i++)
            id2ind.push_back(i);
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

    void feed_friends() {
        for(int i = 0; i<person.size();i++)
            person[i].feed_friends(person,id2ind);
    }

    void socialize() {
        for(int i = 0; i<person.size();i++)
            person[i].socialize(person,id2ind);
    }

    void eat() {
        for(int i = 0; i<person.size();i++){
            if (!person[i].will_starve)
                //person[i].wealth--; // Eat 1 wealth
                person[i].wealth=0; // Eat ALL wealth
        }
    }

    void age() {
        for(int i = 0; i<person.size();i++){
            person[i].age++;
            if (person[i].watch && person[i].age%40==0) printf("\n%s turned %d.",names[person[i].name].c_str(),person[i].age/4);
        }
    }

    std::tuple<int,int> die() {
        // Check deaths twice, first in forward order to adjust id2ind...
        int deaths_so_far=0;
        for(int i = 0; i<person.size();i++){
            if (     person[i].age>person[i].lifespan // Old age
                  || person[i].will_starve  ){        // Starvation
                deaths_so_far++;
                id2ind[person[i].id] = -1;
            } else {
                id2ind[person[i].id] = i-deaths_so_far;
            }
        }
        // ...then backwards to remove
        int n_died=0;
        int n_starved=0;
        for(int i = person.size()-1; i>=0;i--){
            bool died=false;
            if (person[i].age>person[i].lifespan){ // Old age
                died=true;
                if (person[i].watch) printf("\n%s died of old age",names[person[i].name].c_str());
            } else if (person[i].will_starve){        // Starvation
                died=true;
                if (person[i].watch) printf("\n%s died of starvation",names[person[i].name].c_str());
                n_starved++;
            }
            if (died){
                n_died++;
                // Remove from vector
                person.erase(person.begin() + i);
            }
        }
        return std::make_tuple(n_died,n_starved);
    }

    void purge_rships(){
        for(int i = 0; i<person.size();i++){
            person[i].purge_rships(max_rships, person, id2ind);
        }
    }

    int breed() {
        int n_kids = 0;
        for(int i = 0; i<person.size();i++){
            n_kids += person[i].breed(n_ids+n_kids,fertility_age, fertility_rate, person);
        }
        // Updated id to index mapping
        for(int i = person.size()-n_kids; i<person.size();i++)
            id2ind.push_back(i);
        n_ids+=n_kids;
        return n_kids;
    }

    void report(int i_turn){
        int n_male=0; int n_female=0;
        for(int i = 0; i<person.size();i++){
            if (person[i].female) n_female++;
            else n_male++;
        }
        printf("\nTurn %d: live female/male, dead: %d/%d, %lu", i_turn, n_female, n_male, n_ids-person.size());
//        snap_age("ages",i_turn);
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
