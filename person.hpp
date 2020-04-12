#ifndef PERSON_HPP
#define PERSON_HPP
#include <vector>
#include <string.h>
#include "random.hpp"
#include "relationship.hpp"
#include "names.hpp"

class Person{
    public:
    // fixed
    int id;
    int lifespan;
    bool female;
    int name;

    // changing
    int age;
    bool will_starve;

    // Relationships
    std::vector<Relationship> rships;

    int wealth; // temporary

    // Simulation
    bool watch;

    // Initial generation
    Person(int id, int age, int lifespan, int wealth) : id(id), age(age), lifespan(lifespan), will_starve(false), wealth(wealth), watch(false) {
        female = (rand_f1()<0.5); // 50% chance of being female
        name = female ? (rand_f1()*NF_NAMES) : NF_NAMES + (rand_f1()*NM_NAMES);
    }

    // Birth
    Person(int id, Person* mom, Person& dad) : id(id), age(0), will_starve(false), wealth(0), watch(false) {
        lifespan = (mom->lifespan+dad.lifespan)/2;
        female = (rand_f1()<0.5); // 50% chance of being female
        name = female ? (rand_f1()*NF_NAMES) : NF_NAMES + (rand_f1()*NM_NAMES);

        // Create relationships
        rships.push_back(Relationship(mom->id));
        mom->rships.push_back(Relationship(id));
        rships.push_back(Relationship(dad.id));
        dad.rships.push_back(Relationship(id));
        mom->rships[mom->rships.size()-1].child=true;
        dad.rships[dad.rships.size()-1].child=true;
    }


    void do_long_action(int& food_available){
        // DO NOTHING (BUT GROW) IF YOUNG
        if (age<48) return;
        if (watch && age==48) printf("\n%s is foraging independently for the first time :)",names[name].c_str());

        // FORAGE
        if (watch && food_available==0) printf("\nUh oh, %s didn't find food!",names[name].c_str());
        if (food_available>0){
            wealth+=2;
            food_available-=2;
        }
    }

    void feed_friends(std::vector<Person>& people, std::vector<int>& id2ind){
        for (int i_rship=0;i_rship<rships.size();i_rship++){
            if (wealth>1){ // If you have extra food
                int friend_id = rships[i_rship].person_id;
                int friend_ind = id2ind[friend_id];
                if (friend_ind==-1) continue; // Friend is dead
                if (people[friend_ind].will_starve){ // And a friend is hungry
                    // Feed friend
                    wealth--;
                    people[friend_ind].wealth++;
                    people[friend_ind].will_starve=false;

                    if ((watch || people[friend_ind].watch) && !(rships[i_rship].child && people[friend_ind].age<48)) printf("\n%s gave food to %s!",names[name].c_str(),names[people[friend_ind].name].c_str());
                }
            }else{
                break; // No need to continue if no wealth to give
            }
        }
    }

    int breed(int next_id, int fertility_age, float fertility_rate, std::vector<Person>& people) {
        if (female && age>=fertility_age){ // If female and old enough
            if (rand_f1()<fertility_rate){ // If having children
                if (watch) printf("\n%s wants a kid.", names[name].c_str());
                // Find father at random
                RandPerm rp(people.size());
                int dad_ind = -1;
                for (int idad = 0; idad<people.size(); idad++){
                    int ri = rp.x[idad];
                    if(people[ri].age>=fertility_age && !people[ri].female) // adult male, good enough
                        {dad_ind=ri; break;}
                }
                if (dad_ind>=0) { // found a father
                    // Create kid
                    people.push_back(Person(next_id,this,people[dad_ind]));

                    // Create relationship with father
                    rships.push_back(Relationship(people[dad_ind].id));
                    people[dad_ind].rships.push_back(Relationship(id));

                    if (watch){
                        char him_or_her[3]="";
                        strcat(him_or_her, people[people.size()-1].female ? "her" : "him");
                        printf("\n%s had a kid with %s! They named %s %s.", names[name].c_str(), names[people[dad_ind].name].c_str(), him_or_her, names[people[people.size()-1].name].c_str());
                        people[people.size()-1].watch=true;
                    } else if (people[dad_ind].watch){
                        char him_or_her[3]="";
                        strcat(him_or_her, people[people.size()-1].female ? "her" : "him");
                        printf("\n%s had a kid with %s! They named %s %s.", names[people[dad_ind].name].c_str(), names[name].c_str(), him_or_her, names[people[people.size()-1].name].c_str());
                        people[people.size()-1].watch=true;
                    }
                    return 1;
                }
            }
        }
        return 0;
    }

};

#endif
