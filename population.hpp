#ifndef POPULATION_HPP
#define POPULATION_HPP
#include <vector>
#include <string.h>
#include "random.hpp"
#include "group.hpp"
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
        for(int i = 0; i<initial_n_ppl;i++)
            person[i].mships.push_back(Membership(rand_f1()*groups.size()));
    }

    void task_requests() {
        for(int i = 0; i<groups.size();i++)
            groups[i].set_task_request();
        for(int i = 0; i<person.size();i++)
            person[i].respond_to_task_requests(groups);
        for(int i = 0; i<groups.size();i++)
            groups[i].set_tasks();
    }

    void evaluate_choices() {
        for(int i = 0; i<person.size();i++)
            person[i].evaluate_choices();
    }

    void do_long_actions(Nature &nature) {
        RandPerm rp(person.size());
        for(int i = 0; i<person.size();i++){
            int ri = rp.x[i];
            person[ri].do_long_action(nature);
        }
    }

    void take_by_force(int i_turn) {
        RandPerm rp(person.size());
        int ramp = person.size()*((i_turn-240.0f)/4000.0f);
        ramp = std::min((int)(person.size()),std::max(0,ramp));
        for(int i = 0; i<ramp;i++){
            int ri = rp.x[i];
            person[ri].take_by_force(person, groups);
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

    void survive() {
        for(int i = 0; i<person.size();i++){
            if (person[i].wealth<1.0f) person[i].will_starve=true;
            float to_eat = std::min(person[i].wealth,1.0f);
            person[i].wealth-=to_eat;
            person[i].contentedness+=to_eat;
            if (person[i].watch) printf("\n%s's cness after surviving: %.3f", names[person[i].name].c_str(),person[i].contentedness);
        }
    }

    void luxury() {
        for(int i = 0; i<person.size();i++){
            float to_enjoy = std::min(person[i].wealth,0.5f);
            person[i].wealth-=to_enjoy;
            person[i].contentedness+=to_enjoy;
            if (person[i].watch) printf("\n%s's cness after luxury: %.3f", names[person[i].name].c_str(),person[i].contentedness);
        }
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
            n_kids += person[i].breed(n_ids+n_kids,fertility_age, fertility_rate, person,id2ind);
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

    bool group_already_exists(std::vector<int>& new_group){
        int groupsize = new_group.size();
        for (int g=0;g<groupsize;g++){ // Loop over new group members
            int ind = new_group[g];
            for (int k=0;k<person[ind].mships.size();k++){ // Loop over their group affiliations
                int xgroupsize = groups[person[ind].mships[k].id].npaying; // Size of preexisting group
                int xgroupid = person[ind].mships[k].id;
                if (groupsize>2*xgroupsize || xgroupsize>2*groupsize) // If one is more than double the other's size
                    continue; // Clearly not the same group
                // But if they're in the same size range, examine further
                int n_common_members=0;
                for (int gc=0;gc<groupsize;gc++){
                    int indc = new_group[gc];
                    for (int kc=0;kc<person[indc].mships.size();kc++){
                        if (xgroupid == person[indc].mships[kc].id)
                            {n_common_members++; break;}
                    }
                }
                if (n_common_members*2>groupsize || n_common_members*2>xgroupsize){
                    // More than half the members in common
                    // Is the same group
                    return true;
                }
            }
        }
        return false;
    }

    void new_groups(){ // O(N*R^3 + N*R*G^2*M^2)
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
                        bool found_similar = group_already_exists(new_group);
                        if (!found_similar){
                            int newgroup_id=groups.size();
                            groups.push_back(Group(newgroup_id,0,groupsize));
                            for (int g=0;g<groupsize;g++){ // Loop over new group members
                                person[new_group[g]].mships.push_back(Membership(newgroup_id));
                            }
                        }
                    }
                }
            }
        }
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
