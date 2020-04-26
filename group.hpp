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

    // Member list (index in person list)
    std::vector<int> memberlist;

    // Tentative: leader
    int leader; // index in memberlist

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

    void will_desire_how_many_guards(std::vector<int> victim_homes,std::vector<int> lused, std::vector<int> lundefended,std::vector<int> guards_left);

    float will_try_to_raise_how_much();

    int will_request_how_many_guards();

    void set_tasks();

    bool will_try_to_defend();

    // Actions

    void choose_leadership(std::vector<Person>& people);

    void assess_defence(std::vector<int> victim_homes,std::vector<int> lused, std::vector<int> lundefended,std::vector<int> guards_left);

    void set_wealth_request();

    void assess_wealth_request();

    void set_task_request();

    std::tuple<float,int> provide_defense(int victim_ind, int location);
};

#endif
