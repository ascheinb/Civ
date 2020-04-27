#ifndef POPULATION_HPP
#define POPULATION_HPP
#include <vector>
#include <string.h>
#include "random.hpp"
#include "myutils.hpp"
#include "fwd_class_defs.hpp"
#include "group.hpp"
#include "person.hpp"
#include "group.impl.cpp"
#include "person.impl.cpp"

class Population{
    int n_ids;
    int lifespan;
    int fertility_age;
    float fertility_rate; // births/turn
    int max_rships;

    public:

    // Diag
    int sum_dage;
    int sum_nage;

    std::vector<Person> person; // Public so people can see each other (use friend instead?)
    std::vector<int> id2ind; // map from id to index

    std::vector<Group> groups;

    Population(int initial_n_ppl, int map_size) : n_ids(initial_n_ppl) {
        // Population-wide traits
        lifespan = 240;
        fertility_age = 64;
        fertility_rate = 4.2/(lifespan-fertility_age);
        max_rships=50;
        int initial_n_groups=10;

        // Initialize individuals
        for(int i = 0; i<initial_n_ppl;i++)
            person.push_back(Person(i,(i*lifespan)/initial_n_ppl,lifespan,i%map_size));

        // Initialize id to index mapping
        for(int i = 0; i<initial_n_ppl;i++)
            id2ind.push_back(i);

        // Initialize groups
        for(int i = 0; i<initial_n_groups;i++)
            groups.push_back(Group(i,0,initial_n_ppl/10));

        // Assign people randomly to groups
//        for(int i = 0; i<initial_n_ppl;i++)
//            person[i].mships.push_back(Membership(rand_int(groups.size()),INITLOYALTY));
    }

    void task_requests(int i_turn) {
        for(int i = 0; i<groups.size();i++)
            if(groups[i].memberlist.size()>0) groups[i].set_task_request(person[groups[i].memberlist[groups[i].leader]]);

        // Reply from members: randomized
        RandPerm rp(person.size());
        for(int i = 0; i<person.size();i++){
            int ri = rp.x[i];
            person[ri].respond_to_task_requests(groups,ri);
        }

        for(int i = 0; i<groups.size();i++)
            if(groups[i].memberlist.size()>0) groups[i].set_tasks(person[groups[i].memberlist[groups[i].leader]]);
    }

    void assess_defence() {
        for(int i = 0; i<groups.size();i++){
            // Convert from people to tiles
            std::vector<int> victim_homes;
            std::vector<int> lused;
            std::vector<int> lundefended;
            for (int j=0;j<groups[i].used.size();j++){
                int victim_home=person[groups[i].used[j]].home;
                int vhome_ind = get_index(victim_homes,victim_home);
                if (vhome_ind==-1){ // Add to list of tiles
                    vhome_ind=victim_homes.size();
                    victim_homes.push_back(victim_home);
                    lused.push_back(0);
                    lundefended.push_back(0);
                }
                lused[vhome_ind]++;
            }
            for (int j=0;j<groups[i].undefended.size();j++){
                int victim_home=person[groups[i].undefended[j]].home;
                int vhome_ind = get_index(victim_homes,victim_home);
                if (vhome_ind==-1){ // Add to list of tiles
                    vhome_ind=victim_homes.size();
                    victim_homes.push_back(victim_home);
                    lused.push_back(0);
                    lundefended.push_back(0);
                }
                lundefended[vhome_ind]++;
            }
            std::vector<int> guards_left(victim_homes.size(),0);
            for (int j=0;j<groups[i].guards.size();j++){
                if (groups[i].guards[j].nactions==0 && groups[i].guards[j].task==DEFEND) continue; // Guard defended 
                int guard_station=groups[i].guards[j].station;
                int ghome_ind = get_index(victim_homes,guard_station);
                if (ghome_ind==-1){ // Add to list of tiles
                    ghome_ind=victim_homes.size();
                    victim_homes.push_back(guard_station);
                    lused.push_back(0);
                    lundefended.push_back(0);
                    guards_left.push_back(0);
                }
                guards_left[ghome_ind]++;
            }

            groups[i].assess_defence(victim_homes,lused,lundefended,guards_left);
        }
    }

    void evaluate_choices() {
        for(int i = 0; i<person.size();i++)
            person[i].evaluate_choices();
    }

    void leadership() {
        // Leadership model: popularity contest
        for (int i=0;i<groups.size();i++)
            groups[i].choose_leadership(person);
    }

    void do_long_actions(Nature &nature) {
        RandPerm rp(person.size());
        for(int i = 0; i<person.size();i++){
            int ri = rp.x[i];
            person[ri].do_long_action(nature);
        }
    }

    void take_by_force(int i_turn,Nature& nature) {
        RandPerm rp(person.size());
        int ramp = person.size()*((i_turn-240.0f)/4000.0f);
        ramp = std::min((int)(person.size()),std::max(0,ramp));
        for(int i = 0; i<ramp;i++){
            int ri = rp.x[i];
            person[ri].take_by_force(person, groups, nature);
        }
    }

    void feed_friends() {
        for(int i = 0; i<person.size();i++)
            person[i].feed_friends(person,id2ind);
    }

    void socialize() {
        for(int i = 0; i<person.size();i++){
            person[i].socialize(person,id2ind,groups);
            person[i].erode_loyalty();
        }
    }

    void purge_memberships() {
        for(int i = 0; i<person.size();i++){
            person[i].purge_memberships(groups);
        }
    }

    void update_memberlists(){
        std::vector<int> old_nmembers(groups.size());
        std::vector<int> nmembers(groups.size(),0);
        // Get old group size
        for(int i = 0; i<groups.size();i++){
            old_nmembers[i]=groups[i].memberlist.size();
        }
        // Get new group list
        for (int i=0;i<person.size();i++){
            for (int j=0;j<person[i].mships.size();j++){
                int g = person[i].mships[j].id;
                if (nmembers[g]<old_nmembers[g]){ // If not larger than before, overwrite
                    groups[g].memberlist[nmembers[g]]=i;
                }else{ // If larger, allocate more
                    groups[g].memberlist.push_back(i);
                }
                nmembers[g]++;
            }
        }
        // If smaller, free up remaining memory
        for(int i = 0; i<groups.size();i++){
            groups[i].memberlist.resize(nmembers[i]);
        }
    }

    void update_residents(Nature& nature){
        std::vector<int> old_nresidents(nature.map.size());
        std::vector<int> nresidents(nature.map.size(),0);
        // Get old #residents
        for(int i = 0; i<nature.map.size();i++){
            old_nresidents[i]=nature.map[i].residents.size();
        }
        // Get new resident list
        for (int i=0;i<person.size();i++){
            int h = person[i].home;
            if (nresidents[h]<old_nresidents[h]){ // If not larger than before, overwrite
                nature.map[h].residents[nresidents[h]]=i;
            }else{ // If larger, allocate more
                nature.map[h].residents.push_back(i);
            }
            nresidents[h]++;
        }
        // If smaller, free up remaining memory
        for(int i = 0; i<nature.map.size();i++){
            nature.map[i].residents.resize(nresidents[i]);
        }
    }

    int get_nextant(){
        int extant_groups=0;
        for (int i=0;i<groups.size();i++)
            if (groups[i].memberlist.size()>0) extant_groups++;
        return extant_groups;
    }

    void merge_groups(){
        for(int i = 0; i<groups.size();i++){
            if (groups[i].memberlist.size()==0) continue; // Don't check empty groups
            int similar_group = find_similar_group(groups[i].memberlist,groups[i].id);
            if (similar_group>=0){
                bool firstwatch=true; // Only announce once (for when watch is on)
                // merge smaller group into bigger one
                int id_bigger = (groups[i].npaying > groups[similar_group].npaying ? i : similar_group);
                int id_smaller = (groups[i].npaying <= groups[similar_group].npaying ? i : similar_group);
                for(int j = 0; j<groups[id_smaller].memberlist.size();j++){ // Loop over members of smaller group
                    int pind = groups[id_smaller].memberlist[j];
                    int k = person[pind].mship_index(id_smaller);
                    // If person is in big group too, delete the small group
                    // (LOSS OF INFO (e.g. loyalty) - could MERGE instead)
                    bool in_both=person[pind].is_member(id_bigger);
                    if (in_both){
                        person[pind].mships.erase(person[pind].mships.begin() + k);
                    } else { // Join bigger group
                        person[pind].mships[k].id=id_bigger; // Just change the ID
                        groups[id_bigger].memberlist.push_back(pind);
                    }
                    if (person[pind].watch && firstwatch){
                        printf("\nGroup %s merged into %s.",gnames[groups[id_smaller].name].c_str(),gnames[groups[id_bigger].name].c_str());
                        firstwatch=false;
                    }
                }
                // Resources/obligations from smaller group go to larger group
                groups[id_bigger].wealth += groups[id_smaller].wealth;
                groups[id_bigger].nguards+= groups[id_smaller].nguards;
                groups[id_bigger].nundefended+= groups[id_smaller].nundefended;
                groups[id_bigger].nused+= groups[id_smaller].nused;
                groups[id_smaller].memberlist.resize(0);
            }
        }
    }

    void survive() {
        for(int i = 0; i<person.size();i++){
            if (person[i].wealth<FOOD_TO_SURVIVE) person[i].will_starve=true;
            float to_eat = std::min(person[i].wealth,FOOD_TO_SURVIVE);
            person[i].wealth-=to_eat;
            person[i].contentedness+=to_eat;
            //if (person[i].watch) printf("\n%s's cness after surviving: %.3f", names[person[i].name].c_str(),person[i].contentedness);
        }
    }

    void luxury() {
        for(int i = 0; i<person.size();i++)
            person[i].luxury();
    }

    void wealth_requests(){
        for(int i = 0; i<groups.size();i++)
            groups[i].set_wealth_request();

        for(int i = 0; i<person.size();i++)
            person[i].respond_to_wealth_requests(groups);

        for(int i = 0; i<groups.size();i++)
            groups[i].assess_wealth_request();
    }

    void age() {
        for(int i = 0; i<person.size();i++){
            person[i].age++;
        }
        for(int i = 0; i<groups.size();i++){
            groups[i].age++;
            if (groups[i].memberlist.size()==0 && groups[i].prevsize !=0){
                sum_dage += groups[i].age;
                sum_nage +=1;
            }
            groups[i].prevsize = groups[i].memberlist.size();
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
        for(int i = (int)(person.size())-1; i>=0;i--){
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

    int breed(Nature& nature) {
        int n_kids = 0;
        for(int i = 0; i<person.size();i++){
            n_kids += person[i].breed(n_ids+n_kids,fertility_age, fertility_rate, person,id2ind, nature);
        }
        // Updated id to index mapping
        for(int i = person.size()-n_kids; i<person.size();i++)
            id2ind.push_back(i);
        n_ids+=n_kids;
        return n_kids;
    }

    void find_potential_group(std::vector<int>& new_group, Person& p1, int ip1, int j){
        int ip2=id2ind[p1.rships[j].person_id]; // ind of P2
        new_group.push_back(ip1);
        new_group.push_back(ip2);
        // If this rship is strong, run through P1's friend list for mutual friends
        for(int k = 0; k<p1.rships.size();k++){
            if (k!=j && p1.rships[k].fondness_to>3){ // Another close friend of P1, P3
                int friend2_id = p1.rships[k].person_id;
                for(int l = 0; l<person[ip2].rships.size();l++){ // Look for P3 in P2's list
                    if (person[ip2].rships[l].person_id==friend2_id && // P3 is friends with P2
                        person[ip2].rships[l].fondness_to>3){ // Is also a close friend!
                        new_group.push_back(id2ind[friend2_id]);
                        break;
                    }
                }
            }
        }
    }

    int find_similar_group(std::vector<int>& new_group, int thisgroupid){
        int groupsize = new_group.size();
        for (int g=0;g<groupsize;g++){ // Loop over new group members
            int ind = new_group[g];
            for (int k=0;k<person[ind].mships.size();k++){ // Loop over their group affiliations
                int xgroupid = person[ind].mships[k].id;
                if (xgroupid == thisgroupid) continue; // If checking for uniqueness (for merge_groups), don't examine own group
                int xgroupsize = groups[person[ind].mships[k].id].memberlist.size(); // Size of preexisting group
                if (groupsize>2*xgroupsize || xgroupsize>2*groupsize) // If one is more than double the other's size
                    continue; // Clearly not the same group
                // But if they're in the same size range, examine further
                int n_common_members=0;
                for (int gc=0;gc<groupsize;gc++){
                    int indc = new_group[gc];
                    if (person[indc].is_member(xgroupid))
                        n_common_members++;
                }
                if (n_common_members*2>groupsize || n_common_members*2>xgroupsize){
                    // More than half the members in common
                    // Is the same group
                    return xgroupid;
                }
            }
        }
        return -1;
    }

    int new_groups(){ // O(N*R^3 + N*R*G^2*M^2)
        int nstart = groups.size();
        // Have a random 10% of the population look for groups to cut down on run time
        int n_group_checkers = person.size()/10;
        RandPerm rp(person.size());
        for(int i = 0; i<n_group_checkers;i++){
            int ri = rp.x[i];
            // P1 doesn't have enough friends
            if (person[ri].rships.size()+1<SIZEFORMGROUP) continue;
            for(int j = 0; j<person[ri].rships.size();j++){
                if (person[ri].rships[j].fondness_to>=FONDFORMGROUP){ // P1 has a close friend (P2)
                    // P2 doesn't have enough friends
                    //if (person[id2ind[person[ri].rships[j].person_id]].rships.size()+1<SIZEFORMGROUP) continue;

                    std::vector<int> new_group;
                    find_potential_group(new_group,person[ri],ri,j);
                    int groupsize = new_group.size();
                    if (groupsize>=SIZEFORMGROUP) {
                        // Check if group already exists
                        bool found_similar = (find_similar_group(new_group,-1)>=0);
                        if (!found_similar){
                            int newgroup_id=groups.size();
                            groups.push_back(Group(newgroup_id,0,groupsize));
                            for (int g=0;g<groupsize;g++){ // Loop over new group members
                                person[new_group[g]].mships.push_back(Membership(newgroup_id,INITLOYALTY));
                                groups[newgroup_id].memberlist.push_back(new_group[g]);
                                if (person[new_group[g]].watch) printf("\n%s joined a new group called %s", names[person[new_group[g]].name].c_str(),gnames[groups[newgroup_id].name].c_str());
                            }
                        }
                    }
                }
            }
        }
        return groups.size()-nstart;
    }

    template <typename Proc>
    float avg(Proc p){
        float sum=0.0;
        for(int i = 0; i<person.size();i++){
            sum += p(person[i]);
        }
        return sum/person.size();
    }

    template <typename Proc>
    float frac(Proc p){
        float sum=0.0;
        for(int i = 0; i<person.size();i++){
            sum += p(person[i]) ? 1.0 : 0.0;
        }
        return sum/person.size();
    }

    template <typename Proc>
    float frac(int x, Proc p){
        float sum=0.0;
        for(int i = 0; i<person.size();i++){
            sum += p(x, person[i]) ? 1.0 : 0.0;
        }
        return sum/person.size();
    }

    template <typename Proc>
    float avg_in(Proc p){ 
        float sum=0.0;
        float x;
        bool use;
        int count=0;
        for(int i = 0; i<person.size();i++){
            std::tie(x,use) = p(person[i]);
            if (use){
                sum+=x;
                count+=1;
            }
        }
        return sum/count;
    }

    template <typename Proc>
    float avg_in(int x, Proc p){
        float sum=0.0;
        float tmp;
        bool use;
        int count=0;
        for(int i = 0; i<person.size();i++){
            std::tie(tmp,use) = p(x,person[i]);
            if (use){
                sum+=tmp;
                count+=1;
            }
        }
        return sum/count;
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
