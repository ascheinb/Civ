#ifndef PERSON_HPP
#define PERSON_HPP
#include <vector>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include "random.hpp"
#include "relationship.hpp"
#include "membership.hpp"
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

    float old_contentedness;
    float old_workrate;
    float workrate;
    float contentedness;

    // Personality
    int extroversion;

    // Relationships
    std::vector<Relationship> rships;
    std::vector<Membership> mships;

    float wealth; // temporary

    // Simulation
    bool watch;

    // Initial generation
    Person(int id, int age, int lifespan, float wealth) : id(id), age(age), lifespan(lifespan), will_starve(false), wealth(wealth), watch(false) {
        female = (rand_f1()<0.5); // 50% chance of being female
        name = female ? (rand_f1()*NF_NAMES) : NF_NAMES + (rand_f1()*NM_NAMES);
        extroversion = 8+ rand_f1()*16;
        workrate=1.0f;
        old_workrate=1.0f;
        old_contentedness=0.0f;
    }

    // Birth
    Person(int id, Person* mom, Person& dad) : id(id), age(0), will_starve(false), wealth(0.0), watch(false) {
        int mutation_rate = 2;
        lifespan = (mom->lifespan+dad.lifespan)/2;
        female = (rand_f1()<0.5); // 50% chance of being female
        name = female ? (rand_f1()*NF_NAMES) : NF_NAMES + (rand_f1()*NM_NAMES);
        int randfix=rand_f1()*2; // prevents bias from rounding down
        extroversion = (mom->extroversion+dad.extroversion+randfix)/2 + (int)(rand_f1()*(1+2*mutation_rate))-mutation_rate;
        workrate=(mom->workrate + dad.workrate)/2.0f;
        old_workrate=workrate;
        old_contentedness=0.0f;

        // Create relationships
        rships.push_back(Relationship(mom->id));
        mom->rships.push_back(Relationship(id));
        rships.push_back(Relationship(dad.id));
        dad.rships.push_back(Relationship(id));

        mom->rships[mom->rships.size()-1].child=true;
        dad.rships[dad.rships.size()-1].child=true;

        // Set group membership: patrilinear for now
        mships.push_back(Membership(dad.mships[0]));
    }

    void evaluate_choices(){
        if (contentedness > old_contentedness){ // This is better, change workrate!
            old_workrate=workrate;
        }

        // Reset contentedness counter
        old_contentedness = contentedness;
        contentedness=0.0f;
    }

    void do_long_action(float& food_available){
        // DO NOTHING (BUT GROW) IF YOUNG
        if (age<48) return;
        if (watch && age==48) printf("\n%s is foraging independently for the first time :)",names[name].c_str());

        // Choose workrate
        workrate = old_workrate+rand_f1()*0.1f-0.05f; // Try a bit more or less work
        workrate = std::min(1.0f,std::max(0.0f,workrate));

        // FORAGE
        float food_haul=std::min(food_available, 3.0f*workrate);
        if (watch && food_available<1.0) printf("\nUh oh, %s didn't find enough food!",names[name].c_str());
        wealth+=food_haul;
        food_available-=food_haul;


        // ENJOY FREE TIME
        contentedness+=(1.0f-workrate)*2.0;
        if (watch) printf("\n%s's cness after workrate %.3f: %.3f", names[name].c_str(),workrate, contentedness);
    }

    void feed_friends(std::vector<Person>& people, std::vector<int>& id2ind){
        for (int i_rship=0;i_rship<rships.size();i_rship++){
            if (rships[i_rship].fondness_to<5 && !rships[i_rship].child) continue; // Skip not fond friends unless its your kid

            if (wealth>1.5){ // If you have extra food
                int friend_id = rships[i_rship].person_id;
                int friend_ind = id2ind[friend_id];
                if (people[friend_ind].wealth<1.0){ // And a friend is hungry
                    // Feed friend
                    float transfer_amt = std::min(wealth-1.5,1.0-people[friend_ind].wealth);
                    wealth-=transfer_amt;
                    people[friend_ind].wealth+=transfer_amt;

                    // Feel good about it
                    if (rships[i_rship].child){
                        contentedness+=2*transfer_amt;
                    }else{
                        contentedness+=transfer_amt;
                    }

                    if ((watch || people[friend_ind].watch) && !(rships[i_rship].child && people[friend_ind].age<48)) printf("\n%s gave food to %s!",names[name].c_str(),names[people[friend_ind].name].c_str());
                }
            }else{
                break; // No need to continue if no wealth to give
            }
        }
    }

    void socialize(std::vector<Person>& people, std::vector<int>& id2ind){
        // Strategy 1: Spend time with a friend
        // Choose a pre-existing relationship at random
        if (rships.size()==0) return;
        int friend_rind = rand_f1()*rships.size();
        int friend_ind = id2ind[rships[friend_rind].person_id];
        int this_rind;
        for (int i_rship=0;i_rship<people[friend_ind].rships.size();i_rship++){
            if (people[friend_ind].rships[i_rship].person_id==id) {this_rind = i_rship; break;}
        }

        // Strengthen relationship with friend
        rships[friend_rind].fondness_to = std::min(rships[friend_rind].fondness_to+1,32);
        rships[friend_rind].fondness_of = std::min(rships[friend_rind].fondness_of+1,32);
        people[friend_ind].rships[this_rind].fondness_to = std::min(people[friend_ind].rships[this_rind].fondness_to+1,32);
        people[friend_ind].rships[this_rind].fondness_of = std::min(people[friend_ind].rships[this_rind].fondness_of+1,32);

        if (rand_f1()*32>extroversion) return; // Too shy
        // Strategy 2: Branch out
        // Choose one of their friends at random
        int fof_rind = rand_f1()*people[friend_ind].rships.size();
        if (fof_rind == this_rind) return; // Chose yourself, whatever
        int fof_ind = id2ind[people[friend_ind].rships[fof_rind].person_id];

        // Create relationship with the friend of friend
        // Check if relationship already exists
        for (int i_rship=0;i_rship<rships.size();i_rship++){
            if (rships[i_rship].person_id==people[fof_ind].id) return;
        }
        // Otherwise, create the relationship
        if (rand_f1()*32>people[fof_ind].extroversion) return; // Friend is too shy
        rships.push_back(Relationship(people[fof_ind].id));
        people[fof_ind].rships.push_back(Relationship(id));

        if (watch) printf("\n%s met %s through %s.",names[name].c_str(),names[people[fof_ind].name].c_str(),names[people[friend_ind].name].c_str());
        if (!watch && people[fof_ind].watch) printf("\n%s met %s through %s.",names[people[fof_ind].name].c_str(),names[name].c_str(),names[people[friend_ind].name].c_str());
    }

    void purge_rships(int max_rships, std::vector<Person>& people, std::vector<int>& id2ind){
        // Remove dead friends
        int n_rships = rships.size();
        for(int i_rship = n_rships-1; i_rship>=0;i_rship--){
            if (id2ind[rships[i_rship].person_id]==-1)
                rships.erase(rships.begin() + i_rship);
        }

        // Remove most recent friends until back to <max_rships
        n_rships = rships.size();
        for(int i_rship = n_rships-1; i_rship>=max_rships;i_rship--){
            int friend_ind = id2ind[rships[i_rship].person_id];
            rships.erase(rships.begin() + i_rship); // Remove rship
            for (int j_rship = 0; j_rship<people[friend_ind].rships.size();j_rship++){
                if (people[friend_ind].rships[j_rship].person_id==id){
                    people[friend_ind].rships.erase(people[friend_ind].rships.begin() + j_rship); // Reciprocate
                    break;
                }
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
                    // Check if relationship already exists
                    bool prev_rel=false;
                    for (int i_rship=0;i_rship<rships.size();i_rship++){
                        if (rships[i_rship].person_id==people[dad_ind].id) prev_rel=true;
                    }
                    if (!prev_rel){
                        rships.push_back(Relationship(people[dad_ind].id));
                        people[dad_ind].rships.push_back(Relationship(id));
                    }

                    // Bump workrate: they know kids are expensive.
                    old_workrate=1.0f; workrate=1.0f;
                    people[dad_ind].old_workrate = 1.0f; people[dad_ind].workrate = 1.0f;

                    if (watch || people[dad_ind].watch){
                        char him_or_her[3]="";
                        strcat(him_or_her, people[people.size()-1].female ? "her" : "him");
                        if (watch)
                            printf("\n%s had a kid with %s! They named %s %s.", names[name].c_str(), names[people[dad_ind].name].c_str(), him_or_her, names[people[people.size()-1].name].c_str());
                        else
                            printf("\n%s had a kid with %s! They named %s %s.", names[people[dad_ind].name].c_str(), names[name].c_str(), him_or_her, names[people[people.size()-1].name].c_str());
                        char yn[2];
                        printf("\nFollow %s? (y/n)",names[people[people.size()-1].name].c_str());
                        std::cin >> yn;
                        people[people.size()-1].watch=(strcmp(yn,"y")==0);
                    }
                    return 1;
                }
            }
        }
        return 0;
    }

};

#endif
