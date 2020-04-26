#ifndef GROUP_IMPL_CPP
#define GROUP_IMPL_CPP
#include <vector>
#include <string.h>
#include "random.hpp"
#include "names.hpp"
#include "guard.hpp"

void Group::choose_leadership(std::vector<Person>& people){
    int nmembers = memberlist.size();
    if (nmembers==0) return;
    int max_popularity = -1;
    int max_id = -1;
    for (int j=0;j<nmembers;j++){
        int candidate_id = people[memberlist[j]].id;
        int popularity = 0;
        for (int k=0;k<nmembers;k++){
            int rind = people[memberlist[k]].rship_index(candidate_id);
            // If person k knows the candidate, add fondness to popularity
            if (rind>=0){
                popularity+= people[memberlist[k]].rships[rind].fondness_to;
            }
        }
        if (popularity>max_popularity){// (implicit tiebreaker goes to age)
            max_id=j;
            max_popularity=popularity;
        }
    }
    leader = max_id;
}

void Group::will_desire_how_many_guards(std::vector<int> victim_homes,std::vector<int> lused, std::vector<int> lundefended,std::vector<int> guards_left){
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

float Group::will_try_to_raise_how_much(){
    int nguards_desired_total=0;
    for (int i=0;i<nguards_desired.size();i++){
        nguards_desired_total+=nguards_desired[i];
    }
    float desired_expenditure=guard_cost*nguards_desired_total;
    return std::max(desired_expenditure-wealth,0.0f);
}

int Group::will_request_how_many_guards(){
    return (int)(wealth/guard_cost); // rounds down
}

void Group::set_tasks(){
    // Set some to attack
    int nattacks=2; // or less
    for (int i=0;i<guards.size();i++){
        if (i<nattacks){
            guards[i].task=ATTACK;
            guards[i].target=-1;
        }
    }
}

bool Group::will_try_to_defend(){
    return (nused<nguards);
}

// Actions

void Group::assess_defence(std::vector<int> victim_homes,std::vector<int> lused, std::vector<int> lundefended,std::vector<int> guards_left){
    // Updates nguards_desired and guards_desired_loc
    will_desire_how_many_guards(victim_homes,lused,lundefended,guards_left);

    // Reset defence assessment monitoring data
    nused=0;
    nundefended=0;
    used.resize(0);
    undefended.resize(0);
}

void Group::set_wealth_request(){
    float amt_to_raise = will_try_to_raise_how_much();
    wealth_request=req_to_rec*amt_to_raise/npaying; // Requests going out to each member
    npaying=0; // Set this to zero to get an accurate count next time
}

void Group::assess_wealth_request(){
    if (received == 0.0f){
        req_to_rec = 1.0f;
    }else{
        req_to_rec = std::min(wealth_request/received*npaying,10.0f);
    }
    wealth += received;
    received = 0.0f;
}

void Group::set_task_request(){
    guard_request = will_request_how_many_guards(); // Find this many people to hire
    nguards=0; // Assume have to rehire all guards each turn
    guards.resize(0);
}

std::tuple<float,int> Group::provide_defense(int victim_ind, int location){
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
#endif
