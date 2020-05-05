#ifndef PERSON_IMPL_CPP
#define PERSON_IMPL_CPP
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

using std::vector;
using std::min;
using std::max;
using std::is_same;
using std::tie;
using std::cin;
using std::strcat;

// Useful utilities
bool Person::is_member(int group_id){
    for (int k = 0; k<mships.size();k++)
        if (mships[k].id==group_id)
            return true;
    return false;
}

int Person::mship_index(int group_id){
    for (int k = 0; k<mships.size();k++)
        if (mships[k].id==group_id)
            return k;
    return -1;
}

bool Person::knows(int person_id){
    for (int k = 0; k<rships.size();k++)
        if (rships[k].person_id==person_id)
            return true;
    return false;
}

int Person::rship_index(int person_id){
    for (int k = 0; k<rships.size();k++)
        if (rships[k].person_id==person_id)
            return k;
    return -1;
}

int Person::random_local_friend(vector<Person>& people, vector<int>& id2ind){
    RandPerm rp(rships.size());
    for (int i=0;i<rships.size();i++){
        int ri=rp.x[i];
        int friend_ind = id2ind[rships[ri].person_id];
        if (people[friend_ind].home==home) return ri;
    }
    return -1;
}

// Ask question and wait for UI thread to get answer
template<typename T>
T Person::decision(const char* question){
    printf("\n%s ",question);

    T num;
    ctrl.info_id = id; // Localize on this person
    if (is_same<T, float>::value){
        ctrl.needs_float=true;
        while (ctrl.needs_float){
            usleep(50); // Check for updates every 50 ms
        }
        return ctrl.input_float;
    }
    if (is_same<T, int>::value){
        ctrl.needs_int=true;
        while (ctrl.needs_int){
            usleep(50); // Check for updates every 50 ms
        }
        return ctrl.input_int;
    }
    if (is_same<T, bool>::value){
        ctrl.needs_bool=true;
        while (ctrl.needs_bool){
            usleep(50); // Check for updates every 50 ms
        }
        return ctrl.input_bool;
    }

    return num;
}

// UI with acceptable range
template<typename T>
T Person::decision(const char* question, T tmin, T tmax){
    printf("\n%s ",question);
    
    T num;
    ctrl.info_id = id; // Localize on this person
    if (is_same<T, float>::value){
        ctrl.floatmin=tmin;
        ctrl.floatmax=tmax;
        ctrl.range=true;
        ctrl.needs_float=true;
        while (ctrl.needs_float){
            usleep(50); // Check for updates every 50 ms
        }
        ctrl.range=false;
        return ctrl.input_float;
    }
    if (is_same<T, int>::value){
        ctrl.intmin=tmin;
        ctrl.intmax=tmax;
        ctrl.range=true;
        ctrl.needs_int=true;
        while (ctrl.needs_int){
            usleep(50); // Check for updates every 50 ms
        }
        ctrl.range=false;
        return ctrl.input_int;
    }
    
    return num;
}

// Decisions

int Person::will_choose_which_father(int fertility_age, vector<Person>& people, vector<int>& id2ind,Nature& nature){
    if (play) return decision<int>("Which person do you choose?");
    if (true){ // Method 2: Choose randomly among pre-existing relationships
        if (rships.size()>0){
            RandPerm rp(rships.size());
            for (int ridad = 0; ridad<rships.size(); ridad++){
                int ri = id2ind[rships[rp.x[ridad]].person_id];
                if(people[ri].age>=fertility_age && !people[ri].female) // known adult male, good enough
                    return ri;
            }
        }
    }
    // If Method 2 failed, revert to Method 1b: Random among locals
    int nlocals = nature.map[home].residents.size();
    RandPerm rp(nlocals);
    for (int idad = 0; idad<nlocals; idad++){
        int ri = nature.map[home].residents[rp.x[idad]];
        if(people[ri].age>=fertility_age && !people[ri].female) // adult male, good enough
            return ri;
    }
    return -1;
}

float Person::how_hard_will_work(){
    if (play) return decision<float>("How hard do you want to work this turn? (Scale of 0 to 1)",0.0,1.0);
    if (contentedness > old_contentedness){
        // Last turn was happy, so use last turn's workrate as the new base workrate
        old_workrate=workrate;
        //old_luxrate=luxrate;
    }

    // Try a bit more or less work
    float new_workrate = old_workrate+rand_f1()*0.1f-0.05f;
    return min(1.0f,max(0.0f,new_workrate));
}

bool Person::will_move(){
    if (play) return decision<bool>("Move tiles? (y/n)");
    return chance((float)openness/TRAITMAX);
}

int Person::where_will_move(Nature& nature){
    if (play) return decision<int>("Where to move? (0-5)");
    return nature.neighbor(home,random_dir());
}

bool Person::will_take(bool ordered){
    if (play) return decision<bool>("Take wealth by force? (y/n)");
    return agreeableness<=THIEF || ordered;
}

int Person::whom_will_take_from(vector<Person>& people, vector<Group>& groups,Nature& nature, int ordered){
    if (play) return decision<int>("Who to target?");
    if (false){ // Take from someone anywhere
        return rand_int(people.size());
    } else { // Take from someone local
        int nlocals = nature.map[home].residents.size();
        if (ordered){ // Attack whoever your employer wants you to
            if (groups[employer].guards[employee_id].target==-1) { // If target not specified
                // Target anyone not in your group
                RandPerm rp(nlocals);
                for (int i=0;i<nlocals;i++){ // Loop randomly over residents
                    int ptarget = nature.map[home].residents[rp.x[i]];
                    if (!people[ptarget].is_member(employer)){ // Check they're not in your group
                        groups[employer].used_soldier.push_back(home);
                        return ptarget;
                    }
                }
            }
            // If no target found, note as unused soldier
            groups[employer].unused_soldier.push_back(home);
        } else { // Attack random local resident
            return nature.map[home].residents[rand_int(nlocals)];
        }
    }
    return -1;
}

bool Person::will_risk_taking(float amt_to_steal, float success_rate, bool ordered){
    if (play) return decision<bool>("Go through with force attempt? (y/n)");
    float expected_value = amt_to_steal*success_rate - (1-success_rate)*wealth;
    return (expected_value>0.0f || ordered);
}

bool Person::will_give_food(int i_rship,Person& p){
    if (play) {string str("Give food to "); str+=names[p.name]+"? (y/n)"; return decision<bool>(str.c_str());}
    return (rships[i_rship].fondness_to>=FONDSHARE || rships[i_rship].reltype==Child);
}

float Person::how_much_food_will_give(Person& p){
    if (play) {string str("How much to give to "); str+=names[p.name]+"?"; return decision<float>(str.c_str(),0.0,wealth);}
    return min(wealth-(FOOD_TO_SURVIVE+STD_LUX),FOOD_TO_SURVIVE-p.wealth);
}

int Person::will_choose_which_friend(vector<Person>& people, vector<int>& id2ind){
    if (play) return decision<int>("Which friend to hang out with?");
    // Choose a pre-existing relationship at random
    if (false){ // Doesn't matter where friend is
        return rand_int(rships.size());
    }else{ // Only socialize with locals
        return random_local_friend(people,id2ind);
    }
}

bool Person::will_branch_out(){
    if (play) return decision<bool>("Meet a friend of your friend? (y/n)");
    return (rand_f1()*TRAITMAX<=extroversion);
}

float Person::how_much_will_consume(){
    if (play) return decision<float>("How much to spend on luxury?",0.0f,wealth);
    // Try a bit more or less luxury
//    luxrate = max(0.0f,old_luxrate+rand_f1()*0.1f-0.05f);
    luxrate = min(MAX_LUX,wealth*((float)(TRAITMAX-neuroticism)/TRAITMAX));
    return min(wealth,luxrate);
}

float Person::how_much_will_tithe(float req, Group& group){
    if (play){string str("How much will you give to "); str+=gnames[group.name]+"?"; return decision<float>(str.c_str(),0.0f,wealth);}
    return min(wealth,req);
}

bool Person::will_accept_task(){
    if (play) return decision<bool>("Accept task? (y/n)");
    return true;
}

bool Person::will_bump_workrate(){
    if (play) return false; // Manual control
    return (agreeableness>KIDCARE);
}

// Leadership decisions

float Person::how_much_will_skim(Group& group){
    if (play){string str("How much to skim off "); str+=gnames[group.name]+"?"; return decision<float>(str.c_str(),0.0f,group.wealth);}
    // Leader takes 10% over 20 people
    int nmembers = group.memberlist.size();
    float skim = group.wealth*(float)(max(0,nmembers-20))/nmembers*0.10;
    return skim;
}

int Person::how_many_attackers(Group& group){
    if (play){string str("How many of "); str+=gnames[group.name]+"'s forces should attack?"; return decision<int>(str.c_str(),0,(int)group.guards.size());}
    return 2;
}

// Actions

void Person::evaluate_choices(){
    if (worktype==FORAGE){
        workrate = how_hard_will_work();
    }

    // Reset contentedness counter
    old_contentedness = contentedness;
    contentedness=0.0f;

    // Return to being a forager
    worktype=FORAGE;

    if (age<ADULT) worktype=GROW;
    if (watch && age==ADULT) printf("\n%s is working independently for the first time :)",names[name].c_str());
}

void Person::move_tiles(Nature& nature, int new_home){
    if (new_home!=-1){
        if (nature.map[new_home].terrain!=WATER && nature.map[new_home].terrain!=MOUNTAIN){
            if (watch){
                char him_or_her[3]=""; strcpy(him_or_her, female ? "She" : "He");
                printf("\n%s moved from Tile %d to Tile %d",him_or_her,home,new_home);
            }
            home = new_home;
        }
    }
}

void Person::do_long_action(Nature& nature){
    if (worktype==GROW){
        // Do nothing, you're growing!
    } else if (worktype==FORAGE){
        // Find food
        float food_haul=min(nature.map[home].food_available, 3.0f*workrate);
        if (nature.map[home].food_available<FOOD_TO_SURVIVE) {
            if (watch) printf("\nUh oh, %s didn't find enough food!",names[name].c_str());
            if (will_move()){
                move_tiles(nature,where_will_move(nature));
            }
        }
        wealth+=food_haul;
        nature.map[home].food_available-=food_haul;

        // ENJOY FREE TIME
        contentedness+=(1.0f-workrate)*FREE_TIME_CNTDNESS;
        //if (watch) printf("\n%s's cness after workrate %.3f: %.3f", names[name].c_str(),workrate, contentedness);
    } else if (worktype==GUARD){
        // Nothing in here yet, just responds to take_by_force
    }
}

void Person::take_by_force(vector<Person>& people, vector<Group>& groups,Nature& nature){
    bool ordered=false;
    if (worktype==GUARD){
        int task = groups[employer].guards[employee_id].task;
        if (task==ATTACK)
            ordered=true;
    }

    if (!will_take(ordered)) return; // Too agreeable, won't steal (if not ordered)
    int target_ind = whom_will_take_from(people,groups,nature,ordered);

    if (target_ind==-1) return; // Couldn't find target
    if (people[target_ind].id==id) return; // Can't steal from yourself smart guy
    
    float defense=0.0f;
    int defender=-1;
    for (int i=0;i<people[target_ind].mships.size();i++){
        // Muster defense
        int group_id=people[target_ind].mships[i].id;
        int local_guard =-1;
        float gdefense;
        tie(gdefense,local_guard) = groups[group_id].provide_defense(target_ind,home);
        if (local_guard>=0){ // If local defender is available, have them defend
            defense+=gdefense;
            defender=i;
            break;
        }
    }
    if (defender==-1){ // Every group notes failure
        for (int i=0;i<people[target_ind].mships.size();i++){
            int group_id=people[target_ind].mships[i].id;
            groups[group_id].nundefended +=1;
            groups[group_id].undefended.push_back(target_ind);
        }
    }

    float success_rate=0.99f/(defense+1.0f)+0.01f; // starts at 1, approaches 0.01
    float amt_to_steal = people[target_ind].wealth; // Steal all, for now
    if (!will_risk_taking(amt_to_steal, success_rate, ordered)) return; // Not worth the risk! (unless ordered, for now)

    if (chance(success_rate)){ // Successful theft
        float amt_to_steal = people[target_ind].wealth; // Steal all, for now
        if (ordered){ // group gets the cash
            groups[employer].wealth += amt_to_steal;
        }else{
            wealth += amt_to_steal;
        }
        people[target_ind].wealth -= amt_to_steal;
        if (watch) printf("\n%s successfully stole %.3f from %s %s",names[name].c_str(),amt_to_steal,names[people[target_ind].name].c_str(), gnames[groups[people[target_ind].mships[0].id].name].c_str());
    } else { // Caught
        if (defender==-1){
            people[target_ind].wealth += wealth; // If no defenders, target gets the cash
            wealth=0.0f;
        } else {
            if (ordered){
                // No exchange
            } else {
                groups[people[target_ind].mships[defender].id].wealth += wealth; // Defending group gets all, for now
                wealth=0.0f;
                people[target_ind].mships[defender].loyalty_to+=LOYALTY_DEFENDED; // Person is more loyal to group!
            }
        }
        if (watch || people[target_ind].watch) printf("\n%s was caught stealing from %s %s by %s",names[name].c_str(),names[people[target_ind].name].c_str(), gnames[groups[people[target_ind].mships[0].id].name].c_str(),gnames[groups[people[target_ind].mships[defender].id].name].c_str());
    }
}

void Person::feed_friends(vector<Person>& people, vector<int>& id2ind){
    for (int i_rship=0;i_rship<rships.size();i_rship++){
        if (wealth>FOOD_TO_SURVIVE+STD_LUX){ // If you have extra food
            int friend_ind = id2ind[rships[i_rship].person_id];
            if (people[friend_ind].wealth<FOOD_TO_SURVIVE // And a friend is hungry
                    && will_give_food(i_rship,people[friend_ind])){ // and you're willing to give
                // Feed friend
                float transfer_amt = how_much_food_will_give(people[friend_ind]);
                wealth-=transfer_amt;
                people[friend_ind].wealth+=transfer_amt;

                // Feel good about it
                if (rships[i_rship].reltype==Child){
                    contentedness+=transfer_amt*CHILD_FEED_CNTDNESS*(float)agreeableness/TRAITMAX;
                }else{
                    contentedness+=transfer_amt*FRIEND_FEED_CNTDNESS*(float)agreeableness/TRAITMAX;
                }

                if ((watch || people[friend_ind].watch) && !(rships[i_rship].reltype==Child && people[friend_ind].age<ADULT))
                    printf("\n%s gave food to %s!",names[name].c_str(),names[people[friend_ind].name].c_str());
            }
        }else{
            break; // No need to continue if no wealth to give
        }
    }
}

void Person::socialize(vector<Person>& people, vector<int>& id2ind, vector<Group>& groups){
//        if (play) printf("\nentered socialize");
    if (rships.size()==0) return; // Skip if no existing relationships
    // Strategy 1: Spend time with a friend
    int friend_rind = will_choose_which_friend(people,id2ind);
    if (friend_rind==-1) return; // No friends, can't socialize
    int friend_ind = id2ind[rships[friend_rind].person_id];
    int this_rind = people[friend_ind].rship_index(id);

    // Strengthen relationship with friend
    rships[friend_rind].fondness_to = min(rships[friend_rind].fondness_to+1,TRAITMAX);
    rships[friend_rind].fondness_of = min(rships[friend_rind].fondness_of+1,TRAITMAX);
    people[friend_ind].rships[this_rind].fondness_to = min(people[friend_ind].rships[this_rind].fondness_to+1,TRAITMAX);
    people[friend_ind].rships[this_rind].fondness_of = min(people[friend_ind].rships[this_rind].fondness_of+1,TRAITMAX);

    // Join a random group of your friends but at low loyalty
    if (people[friend_ind].mships.size()>0){
        int rand_ind = rand_int(people[friend_ind].mships.size());
        int rand_id = people[friend_ind].mships[rand_ind].id;
        if (!is_member(rand_id)){ // If not already member
            if (watch) printf("\n%s joined %s", names[name].c_str(),gnames[groups[rand_id].name].c_str());
            mships.push_back(Membership(rand_id,INTRO_LOYALTY));
        }
    }

    // Adjust loyalty to groups
    for (int i=0;i<mships.size();i++){
        for (int j=0;j<people[friend_ind].mships.size();j++){
            if (mships[i].id==people[friend_ind].mships[j].id){ // group is common
                mships[i].loyalty_to+=SOCIAL_LOYALTY; // So socializing increases loyalty to the group
                people[friend_ind].mships[j].loyalty_to+=SOCIAL_LOYALTY;
                break;
            }
        }
    }

    // Strategy 2: Branch out
    if (!will_branch_out()) return; // Too shy
    // Choose one of their friends at random
    int fof_rind = rand_int(people[friend_ind].rships.size());
    if (fof_rind == this_rind) return; // Chose yourself, whatever
    int fof_ind = id2ind[people[friend_ind].rships[fof_rind].person_id];
    if (!people[fof_ind].will_branch_out()) return; // Friend is too shy

    // Create relationship with the friend of friend
    // Check if relationship already exists
    for (int i_rship=0;i_rship<rships.size();i_rship++){
        if (rships[i_rship].person_id==people[fof_ind].id) return;
    }
    // Otherwise, create the relationship
    rships.push_back(Relationship(people[fof_ind].id));
    people[fof_ind].rships.push_back(Relationship(id));

    if (watch) printf("\n%s met %s through %s.",names[name].c_str(),names[people[fof_ind].name].c_str(),names[people[friend_ind].name].c_str());
    if (!watch && people[fof_ind].watch) printf("\n%s met %s through %s.",names[people[fof_ind].name].c_str(),names[name].c_str(),names[people[friend_ind].name].c_str());
}

void Person::erode_loyalty(){
    for (int i=0;i<mships.size();i++){
        mships[i].loyalty_to-=LOYALTY_EROSION*(float)(TRAITMAX-conscientiousness)/TRAITMAX; // Reduce loyalty to all groups as time passes
    }
}

void Person::purge_memberships(vector<Group>& groups){
    // Remove membership if no loyalty
    for (int i=(int)(mships.size())-1;i>=0;i--){
        if (mships[i].loyalty_to<=0.0f){
            if (watch) printf("\n%s left %s", names[name].c_str(),gnames[groups[mships[i].id].name].c_str());
            mships.erase(mships.begin() + i); // Remove membership if no loyalty
        }
    }
}

void Person::luxury(){
    // Function to determine how much to spend vs save: save all if neurotic, spend all if not
    float to_enjoy = how_much_will_consume();
    wealth-=to_enjoy;
    contentedness+=to_enjoy*LUX_CNTDNESS;//*((float)(TRAITMAX-neuroticism)/TRAITMAX);
    //if (person[i].watch) printf("\n%s's cness after luxury: %.3f", names[person[i].name].c_str(),person[i].contentedness);
}

void Person::respond_to_wealth_requests(vector<Group>& groups){
    for (int i=0;i<mships.size();i++){
        float req = groups[mships[i].id].wealth_request;

        float transfer_amt = how_much_will_tithe(req, groups[mships[i].id]);

        wealth-=transfer_amt;
        groups[mships[i].id].received+=transfer_amt;
        groups[mships[i].id].npaying+=1;
    }
}

void Person::respond_to_task_requests(vector<Group>& groups,int this_ind){
    if (age<ADULT) return; // Only adults
    for (int i=0;i<mships.size();i++){
        int gid = mships[i].id;
        if (groups[gid].nguards<groups[gid].guard_request){ // need more guards
            // Check if guards needed in your area
            int area_gid=-1;
            for (int j=0;j<groups[gid].tile_inds.size();j++){
                if (groups[gid].tile_inds[j]==home)
                    {area_gid=j; break;}
            }
            if (area_gid==-1) continue;

            // No guards needed on this tile
            if ((groups[gid].nguards_desired[area_gid]<=0) && (groups[gid].nsoldiers_desired[area_gid]<=0)) continue;

            if(will_accept_task()){
                worktype = GUARD;
                employer = gid;
                groups[gid].nguards++;
                if (groups[gid].nguards_desired[area_gid]>0){ // If defense needed on this tile
                    groups[gid].guards.push_back(Guard(this_ind,ACTIONS_PER_GUARD,DEFEND,home));
                    groups[gid].nguards_desired[area_gid]--;
                } else if (groups[gid].nsoldiers_desired[area_gid]>0){ // If defense no longer needed, move to offense
                    groups[gid].guards.push_back(Guard(this_ind,ACTIONS_PER_GUARD,ATTACK,home));
                    groups[gid].nsoldiers_desired[area_gid]--;
                }

                employee_id = groups[gid].guards.size();
                groups[gid].wealth-=groups[gid].guard_cost;
                wealth+=groups[gid].guard_cost;

                // Locality info
                groups[gid].nguards_desired[area_gid]--;
            }
        }
    }
}

void Person::purge_rships(int max_rships, vector<Person>& people, vector<int>& id2ind){
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

int Person::breed(int next_id, int fertility_age, float fertility_rate, vector<Person>& people, vector<int>& id2ind, Nature& nature) {
    if (female && age>=fertility_age){ // If female and old enough
        if (chance(fertility_rate)){ // If having children
            if (watch) printf("\n%s wants a kid.", names[name].c_str());
            // Find father
            int dad_ind = will_choose_which_father(fertility_age,people,id2ind,nature);

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
                if (will_bump_workrate()) {old_workrate=1.0f; workrate=1.0f;}
                if (people[dad_ind].will_bump_workrate()) {people[dad_ind].old_workrate = 1.0f; people[dad_ind].workrate = 1.0f;}

                if (watch || people[dad_ind].watch){
                    char him_or_her[3]=""; strcat(him_or_her, people[people.size()-1].female ? "her" : "him");
                    if (watch)
                        printf("\n%s had a kid with %s! They named %s %s.", names[name].c_str(), names[people[dad_ind].name].c_str(), him_or_her, names[people[people.size()-1].name].c_str());
                    else
                        printf("\n%s had a kid with %s! They named %s %s.", names[people[dad_ind].name].c_str(), names[name].c_str(), him_or_her, names[people[people.size()-1].name].c_str());
                    char yn[2];
                    printf("\nFollow %s? (y/n)",names[people[people.size()-1].name].c_str());
                    cin >> yn;
                    people[people.size()-1].watch=(strcmp(yn,"y")==0);
                }
                return 1;
            }
        }
    }
    return 0;
}


#endif
