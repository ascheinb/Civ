#ifndef PERSON_HPP
#define PERSON_HPP
#include <vector>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include "random.hpp"
#include "relationship.hpp"
#include "membership.hpp"
#include "group.hpp"
#include "nature.hpp"
#include "names.hpp"

#define GROW 0
#define FORAGE 1
#define GUARD 2
#define TRAITMAX 32

// Age of adulthood
#define ADULT 48
// Agreeableness min to reset workrate when having kids
#define KIDCARE 8
// Level of fondness needed to share food (unless your kid)
#define FONDSHARE 5
// Agreeableness below which you are ok with stealing
#define THIEF 9
// Size of friend group required to form an official group
#define SIZEFORMGROUP 15
// Level of fondness required to qualify as friend group
#define FONDFORMGROUP 4

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

    int home;
    int worktype;
    float old_contentedness;
    float old_workrate;
    float workrate;
    float contentedness;

    // Personality
    int extroversion; // Controls branch-out socialization
    int agreeableness; // Controls theft and caretaking

    // Relationships
    std::vector<Relationship> rships;
    std::vector<Membership> mships;

    float wealth; // temporary

    // Simulation
    bool watch;

    // Initial generation
    Person(int id, int age, int lifespan, int home ) : id(id), age(age), lifespan(lifespan), will_starve(false), home(home), wealth(0), watch(false) {
        female = (rand_f1()<0.5); // 50% chance of being female
        name = female ? (rand_f1()*NF_NAMES) : NF_NAMES + (rand_f1()*NM_NAMES);
        extroversion = 8+8 + rand_f1()*16;
        agreeableness = 8 + rand_f1()*16;
        worktype=FORAGE;
        workrate=1.0f;
        old_workrate=1.0f;
        old_contentedness=0.0f;
    }

    // Birth
    Person(int id, Person* mom, Person& dad) : id(id), age(0), will_starve(false), wealth(0.0), watch(false) {
        lifespan = (mom->lifespan+dad.lifespan)/2;
        female = (rand_f1()<0.5); // 50% chance of being female
        name = female ? (rand_f1()*NF_NAMES) : NF_NAMES + (rand_f1()*NM_NAMES);

        int mutation_rate = 2; // parents avg +/- 0,1,or 2
        extroversion =  (mom->extroversion +dad.extroversion +rand_f1()*2)/2 + (int)(rand_f1()*(1+2*mutation_rate))-mutation_rate;
        int one_parent=rand_f1()*2;
        agreeableness = (mom->agreeableness*one_parent+dad.agreeableness*(1-one_parent)) + (int)(rand_f1()*(1+2*mutation_rate))-mutation_rate;

        // Stay in range
        extroversion=std::max(std::min(extroversion,TRAITMAX),0);
        agreeableness=std::max(std::min(agreeableness,TRAITMAX),0);

        // Home
        home = dad.home; // Patrilineal home for now

        // Learn work habits from parents
        worktype=GROW;
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
        if (worktype==FORAGE){
            if (contentedness > old_contentedness){ // This is better, change workrate!
                old_workrate=workrate;
            }
            // Try a bit more or less work
            workrate = old_workrate+rand_f1()*0.1f-0.05f;
            workrate = std::min(1.0f,std::max(0.0f,workrate));
        }

        // Reset contentedness counter
        old_contentedness = contentedness;
        contentedness=0.0f;

        // Return to being a forager
        worktype=FORAGE;

        if (age<ADULT) worktype=GROW;
        if (watch && age==ADULT) printf("\n%s is working independently for the first time :)",names[name].c_str());
    }

    void do_long_action(Nature& nature){
        if (worktype==GROW){
            // Do nothing, you're growing!
        } else if (worktype==FORAGE){
            // Find food
            float food_haul=std::min(nature.map[home].food_available, 3.0f*workrate);
            if (nature.map[home].food_available<1.0) {
                if (watch) printf("\nUh oh, %s didn't find enough food!",names[name].c_str());
                // Change home to an adjacent tile
                int adj_tile = nature.neighbor(home,(int)(rand_f1()*6.0f));
                int oldhome=home;
                if (adj_tile!=-1) home = adj_tile;
                if (watch) printf("\nThey moved from Tile %d to Tile %d",oldhome,home);
            }
            wealth+=food_haul;
            nature.map[home].food_available-=food_haul;

            // ENJOY FREE TIME
            contentedness+=(1.0f-workrate)*2.0;
            if (watch) printf("\n%s's cness after workrate %.3f: %.3f", names[name].c_str(),workrate, contentedness);
        } else if (worktype==GUARD){
            // Nothing in here yet, just responds to take_by_force
        }
    }

    void take_by_force(std::vector<Person>& people, std::vector<Group>& groups){
        if (agreeableness>THIEF) return; // Too agreeable, won't steal
        int target_ind = rand_f1()*people.size();
        if (people[target_ind].id==id) return; // Can't steal from yourself smart guy
        
        float defense=0.0f;
        for (int i=0;i<people[target_ind].mships.size();i++){
            // Muster defense
            int group_id=people[target_ind].mships[i].id;
            if (groups[group_id].nused<groups[group_id].nguards){
                defense+=groups[group_id].guard_strength;
                groups[group_id].nused +=1;
            } else {
                groups[group_id].nundefended +=1;
            }
        }
        float success_rate=0.99f/(defense+1.0f)+0.01f; // starts at 1, approaches 0.01
        // Rational thieving decisions:
        float amt_to_steal = people[target_ind].wealth; // Steal all, for now
        float amt_to_lose = wealth; // Group rule: If you're caught, they take all your money
        float expected_value = amt_to_steal*success_rate - (1-success_rate)*wealth;
        if (expected_value<0) return; // Not worth the risk!
        if (rand_f1()<success_rate){ // Successful theft
            float amt_to_steal = people[target_ind].wealth; // Steal all, for now
            wealth += amt_to_steal;
            people[target_ind].wealth -= amt_to_steal;
            if (watch) printf("\n%s successfully stole %.3f from %s %s",names[name].c_str(),amt_to_steal,names[people[target_ind].name].c_str(), gnames[groups[people[target_ind].mships[0].id].name].c_str());
        } else { // Caught
            groups[people[target_ind].mships[0].id].wealth += wealth; // First group gets all, for now
            wealth=0.0f;
            if (watch) printf("\n%s failed to steal from %s %s",names[name].c_str(),names[people[target_ind].name].c_str(), gnames[groups[people[target_ind].mships[0].id].name].c_str());
        }
    }

    void feed_friends(std::vector<Person>& people, std::vector<int>& id2ind){
        for (int i_rship=0;i_rship<rships.size();i_rship++){
            if (rships[i_rship].fondness_to<FONDSHARE && !rships[i_rship].child) continue; // Skip not fond friends unless its your kid

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
                        contentedness+=2*transfer_amt*(float)agreeableness/TRAITMAX;
                    }else{
                        contentedness+=transfer_amt*(float)agreeableness/TRAITMAX;
                    }

                    if ((watch || people[friend_ind].watch) && !(rships[i_rship].child && people[friend_ind].age<ADULT))
                        printf("\n%s gave food to %s!",names[name].c_str(),names[people[friend_ind].name].c_str());
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
        rships[friend_rind].fondness_to = std::min(rships[friend_rind].fondness_to+1,TRAITMAX);
        rships[friend_rind].fondness_of = std::min(rships[friend_rind].fondness_of+1,TRAITMAX);
        people[friend_ind].rships[this_rind].fondness_to = std::min(people[friend_ind].rships[this_rind].fondness_to+1,TRAITMAX);
        people[friend_ind].rships[this_rind].fondness_of = std::min(people[friend_ind].rships[this_rind].fondness_of+1,TRAITMAX);

        if (rand_f1()*TRAITMAX>extroversion) return; // Too shy
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
        if (rand_f1()*TRAITMAX>people[fof_ind].extroversion) return; // Friend is too shy
        rships.push_back(Relationship(people[fof_ind].id));
        people[fof_ind].rships.push_back(Relationship(id));

        if (watch) printf("\n%s met %s through %s.",names[name].c_str(),names[people[fof_ind].name].c_str(),names[people[friend_ind].name].c_str());
        if (!watch && people[fof_ind].watch) printf("\n%s met %s through %s.",names[people[fof_ind].name].c_str(),names[name].c_str(),names[people[friend_ind].name].c_str());
    }

    void respond_to_wealth_requests(std::vector<Group>& groups){
        for (int i=0;i<mships.size();i++){
            float req = groups[mships[i].id].wealth_request;
            // Accept automatically, for now
            float transfer_amt = std::min(wealth,req);
            wealth-=transfer_amt;
            groups[mships[i].id].received+=transfer_amt;
            groups[mships[i].id].npaying+=1;
        }
    }

    void respond_to_task_requests(std::vector<Group>& groups){
        if (age<ADULT) return; // Only adults
        for (int i=0;i<mships.size();i++){
            if (groups[mships[i].id].nguards<groups[mships[i].id].guard_request){ // need more guards
                // Accept automatically, for now
                worktype = GUARD;
                groups[mships[i].id].nguards++;
                groups[mships[i].id].wealth-=groups[mships[i].id].guard_cost;
                wealth+=groups[mships[i].id].guard_cost;
            }
        }
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

    int breed(int next_id, int fertility_age, float fertility_rate, std::vector<Person>& people, std::vector<int>& id2ind) {
        if (female && age>=fertility_age){ // If female and old enough
            if (rand_f1()<fertility_rate){ // If having children
                if (watch) printf("\n%s wants a kid.", names[name].c_str());
                // Find father
                int dad_ind = -1;
                if (true){ // Method 2: Choose randomly among pre-existing relationships
                    if (rships.size()>0){
                        RandPerm rp(rships.size());
                        for (int ridad = 0; ridad<rships.size(); ridad++){
                            int ri = id2ind[rships[rp.x[ridad]].person_id];
                            if(people[ri].age>=fertility_age && !people[ri].female) // known adult male, good enough
                                {dad_ind=ri; break;}
                        }
                    }
                }
                if (dad_ind==-1){ // If Method 2 failed, revert to Method 1: Random 
                    RandPerm rp(people.size());
                    for (int idad = 0; idad<people.size(); idad++){
                        int ri = rp.x[idad];
                        if(people[ri].age>=fertility_age && !people[ri].female) // adult male, good enough
                            {dad_ind=ri; break;}
                    }
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
                    if (agreeableness>KIDCARE) {old_workrate=1.0f; workrate=1.0f;}
                    if (people[dad_ind].agreeableness>KIDCARE) {people[dad_ind].old_workrate = 1.0f; people[dad_ind].workrate = 1.0f;}

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
