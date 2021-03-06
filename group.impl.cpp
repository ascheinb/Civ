#ifndef GROUP_IMPL_CPP
#define GROUP_IMPL_CPP
#include <vector>
#include <string.h>
#include "random.hpp"
#include "names.hpp"
#include "guard.hpp"

using std::vector;
using std::string;
using std::tuple;
using std::make_tuple;
using std::min;
using std::max;

void Group::choose_leadership(vector<Person>& people){
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
            max_id=people[memberlist[j]].id;
            max_popularity=popularity;
        }
    }
    leader = max_id;
}

void Group::will_desire_how_many_guards(vector<int> victim_homes,vector<int> lused, vector<int> lundefended,vector<int> guards_left,
                                        vector<int> lused_soldier, vector<int> lunused_soldier){
    nguards_desired.resize(0);
    nsoldiers_desired.resize(0);
    tile_inds.resize(0);
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
        tile_inds.push_back(victim_homes[j]);

        // Desire one attacker as well
        if (lunused_soldier[j]>0) { // too many soldiers here
            nsoldiers_desired.push_back(lused_soldier[j]);
        } else { // Add another soldier here
            nsoldiers_desired.push_back(lused_soldier[j]+1);
        }
    }
}

float Group::will_try_to_raise_how_much(){
    int nguards_desired_total=0;
    // Defense
    for (int i=0;i<nguards_desired.size();i++){
        nguards_desired_total+=nguards_desired[i];
    }
    // Offense
    for (int i=0;i<nsoldiers_desired.size();i++){
        nguards_desired_total+=nsoldiers_desired[i];
    }
    float desired_expenditure=guard_cost*nguards_desired_total;
    return max(desired_expenditure-wealth,0.0f);
}

int Group::will_request_how_many_guards(){
    return (int)(wealth/guard_cost); // rounds down
}

void Group::set_tasks(Person& pleader){
    // Set some to attack
/*    int nattacks=pleader.how_many_attackers(*this); // or less
    for (int i=0;i<guards.size();i++){
        if (i<nattacks){
            guards[i].task=ATTACK;
            guards[i].target=-1;
        }
    }
    */
}

bool Group::will_try_to_defend(){
    return (nused<nguards);
}

// Actions

void Group::assess_defence(vector<int> victim_homes,vector<int> lused, vector<int> lundefended,vector<int> guards_left,
                           vector<int> lused_soldier, vector<int> lunused_soldier){
    // Updates nguards_desired, nsoldiers_desired and tile_inds
    will_desire_how_many_guards(victim_homes,lused,lundefended,guards_left, lused_soldier, lunused_soldier);

    // Reset defence assessment monitoring data
    nused=0;
    nundefended=0;
    used.resize(0);
    undefended.resize(0);

    // Reset attack assessment
    unused_soldier.resize(0);
    used_soldier.resize(0);
}

void Group::set_wealth_request(){
    float amt_to_raise = will_try_to_raise_how_much();
    wealth_request=req_to_rec*amt_to_raise/max(npaying,1); // Requests going out to each member
    npaying=0; // Set this to zero to get an accurate count next time
}

void Group::assess_wealth_request(){
    if (received == 0.0f){
        req_to_rec = 1.0f;
    }else{
        req_to_rec = min(wealth_request/received*npaying,10.0f);
    }
    wealth += received;
    received = 0.0f;
}

void Group::set_task_request(Person& pleader){
    // Since this is also the budgeting function for now...
    float paycheck = pleader.how_much_will_skim(*this);
    pleader.wealth+=paycheck;
    wealth-=paycheck;
    guard_request = will_request_how_many_guards(); // Find this many people to hire
    nguards=0; // Assume have to rehire all guards each turn
    guards.resize(0);
}

tuple<float,int> Group::provide_defense(int victim_ind, int location){
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
                return make_tuple(guard_strength,j);
            }
        }
    }
    return make_tuple(0.0f,-1); // Couldn't provide defense
}
#endif
