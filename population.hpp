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

    Population(int initial_n_ppl) : n_ids(initial_n_ppl) {
        // Population-wide traits
        lifespan = 240;
        fertility_age = 64;
        fertility_rate = 4.2/(lifespan-fertility_age);
        max_rships=50;
        int initial_n_groups=10;

        // Initialize individuals
        for(int i = 0; i<initial_n_ppl;i++)
            person.push_back(Person(i,(i*lifespan)/initial_n_ppl,lifespan,0));

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
    }

    void evaluate_choices() {
        for(int i = 0; i<person.size();i++)
            person[i].evaluate_choices();
    }

    void do_long_actions(float& food_available) {
        RandPerm rp(person.size());
        for(int i = 0; i<person.size();i++){
            int ri = rp.x[i];
            person[ri].do_long_action(food_available);
        }
    }

    void theft() {
        for(int i = 0; i<person.size();i++)
            person[i].steal(person, groups);
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
            float to_eat = std::min(person[i].wealth,1.5f);
            person[i].wealth-=to_eat;
            person[i].contentedness+=(to_eat);
            if (person[i].watch) printf("\n%s's cness after eating: %.3f", names[person[i].name].c_str(),person[i].contentedness);
            if (to_eat<1.0f) person[i].will_starve=true;
        }
    }

    void wealth_requests(){
        for(int i = 0; i<groups.size();i++)
            groups[i].set_wealth_request();

        for(int i = 0; i<person.size();i++)
            person[i].respond_to_wealth_requests(groups);
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
            n_kids += person[i].breed(n_ids+n_kids,fertility_age, fertility_rate, person);
        }
        // Updated id to index mapping
        for(int i = person.size()-n_kids; i<person.size();i++)
            id2ind.push_back(i);
        n_ids+=n_kids;
        return n_kids;
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
