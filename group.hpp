#ifndef GROUP_HPP
#define GROUP_HPP
#include <vector>
#include <string.h>
#include "random.hpp"
#include "person.hpp"
#include "names.hpp"
#include "guard.hpp"

class Group{
    public:
    // fixed
    int id;
    int name;

    // changing
    float wealth;
    int land;
    float guard_strength;
    float guard_cost;
    int nguards;
    int nused;
    int nundefended;
    std::vector<int> used;
    std::vector<int> undefended;
    std::vector<int> nguards_desired;
    std::vector<int> guards_desired_loc;
    int npaying;
    float req_to_rec; // ratio of requested/received

    int age;
    int prevsize;

    // Tentative: member list
    std::vector<int> memberlist;

    // Guard list
    std::vector<Guard> guards;

    // Will be a function later
    float wealth_request;
    float received;

    int guard_request;

    Group(int id, int land, int npaying) : age(0), id(id), wealth(0), land(land), npaying(npaying),
        guard_strength(20),guard_cost(2),nguards(0),nused(0),nundefended(0),req_to_rec(0.0f),received(0.0f),
        guard_request(0)
    {
        name = id;
        if (id>=NGROUP_NAMES){
            std::string new_gname = generate_name();
            gnames.push_back(new_gname);
        }
    }

    // Decisions

    void will_desire_how_many_guards(std::vector<int> victim_homes,std::vector<int> lused, std::vector<int> lundefended,std::vector<int> guards_left){
        nguards_desired.resize(0);
        guards_desired_loc.resize(0);
        // Great, now have a list of used/defended, by tile
        for (int j=0;j<victim_homes.size();j++){
            // Apply the adjustments to decide how many guards to request in each location
            int nlast_turn = guards_left[j]+lused[j];
            int adjustment = 0;
            if (guards_left[j]>0){ // Not all guards were used
                adjustment= -1; // Reduce guard request by one
            } else { // All guards were used
                if (lundefended[j]>0) { // not enough guards
                    adjustment=4; // Not enough guards->aggressively hire guards
                }
            }
            nguards_desired.push_back(nlast_turn + adjustment);
            guards_desired_loc.push_back(victim_homes[j]);
        }
    }

    float will_try_to_raise_how_much(){
        int nguards_desired_total=0;
        for (int i=0;i<nguards_desired.size();i++){
            nguards_desired_total+=nguards_desired[i];
        }
        float desired_expenditure=guard_cost*nguards_desired_total;
        return std::max(desired_expenditure-wealth,0.0f);
    }

    int will_request_how_many_guards(){
        return (int)(wealth/guard_cost); // rounds down
    }

    void set_tasks(){
        // Set some to attack
        int nattacks=2; // or less
        for (int i=0;i<guards.size();i++){
            if (i<nattacks){
                guards[i].task=ATTACK;
                guards[i].target=-1;
            }
        }
    }

    bool will_try_to_defend(){
        return (nused<nguards);
    }

    // Actions

    void assess_defence(std::vector<int> victim_homes,std::vector<int> lused, std::vector<int> lundefended,std::vector<int> guards_left){
        // Updates nguards_desired and guards_desired_loc
        will_desire_how_many_guards(victim_homes,lused,lundefended,guards_left);

        // Reset defence assessment monitoring data
        nused=0;
        nundefended=0;
        used.resize(0);
        undefended.resize(0);
    }

    void set_wealth_request(){
        float amt_to_raise = will_try_to_raise_how_much();
        wealth_request=req_to_rec*amt_to_raise/npaying; // Requests going out to each member
        npaying=0; // Set this to zero to get an accurate count next time
    }

    void assess_wealth_request(){
        if (received == 0.0f){
            req_to_rec = 1.0f;
        }else{
            req_to_rec = std::min(wealth_request/received*npaying,10.0f);
        }
        wealth += received;
        received = 0.0f;
    }

    void set_task_request(){
        guard_request = will_request_how_many_guards(); // Find this many people to hire
        nguards=0; // Assume have to rehire all guards each turn
        guards.resize(0);
    }

    std::tuple<float,int> provide_defense(int victim_ind, int location){
        if (will_try_to_defend()){
            // Check if local defender available
            for (int j=0;j<guards.size();j++){
                if (guards[j].station==location // local
                    && guards[j].task==DEFEND // defender
                    && guards[j].nactions>0) // available
                {
                    nused +=1;
                    used.push_back(victim_ind);
                    guards[j].nactions--;
                    return std::make_tuple(guard_strength,j);
                }
            }
        }
        return std::make_tuple(0.0f,-1); // Couldn't provide defense
    }
};

#endif
