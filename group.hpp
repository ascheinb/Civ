#ifndef GROUP_HPP
#define GROUP_HPP
#include <vector>
#include <string.h>
#include "random.hpp"
#include "person.hpp"
#include "names.hpp"

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
    std::vector<int> guards;
    std::vector<int> guard_actions;

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

    void set_wealth_request(){
        int nguards_desired_total=0;
        for (int i=0;i<nguards_desired.size();i++){
            nguards_desired_total+=nguards_desired[i];
        }
        float desired_expenditure=guard_cost*nguards_desired_total;
        float amt_to_raise = std::max(desired_expenditure-wealth,0.0f);
        wealth_request=req_to_rec*amt_to_raise/npaying; // Requests going out to each member
//if (id==0) printf("\n%s had %d used and %d undef, requests %.3f to %d members",gnames[name].c_str(),nused,nundefended,wealth_request,npaying);
        npaying=0; // Set this to zero to get an accurate count next time
//        wealth_request=0.0f;
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
//if (id==0) printf("\n%s raised %.3f ",gnames[name].c_str(),received);

        guard_request = wealth/guard_cost; // Find this many people to hire
//if (guard_request!=0) printf("\nGuard request of %d: %d",id,guard_request);
        nguards=0; // Assume have to rehire all guards each turn
        guards.resize(0);
        guard_actions.resize(0);
//if (id==0) printf("and bought %d guards",nguards);
    }

    void set_tasks(){
        nused=0;
        nundefended=0;
        used.resize(0);
        undefended.resize(0);
//if (id==0) printf("\n%s hired %d guards, wanted %d",gnames[name].c_str(),nguards,guard_request);
    }
};

#endif
