#ifndef GROUP_HPP
#define GROUP_HPP
#include <vector>
#include <string.h>
#include "random.hpp"
#include "person.hpp"
#include "names.hpp"
#include "guard.hpp"

using std::vector;
using std::string;
using std::tuple;

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
    vector<int> used;
    vector<int> undefended;
    vector<int> nguards_desired;
    vector<int> used_soldier;
    vector<int> unused_soldier;
    vector<int> nsoldiers_desired;
    vector<int> tile_inds;
    int npaying;
    float req_to_rec; // ratio of requested/received

    int age;
    int prevsize;

    // Member list (index in person list)
    vector<int> memberlist;

    // Tentative: leader
    int leader = 0; // index in memberlist

    // Guard list
    vector<Guard> guards;

    // Will be a function later
    float wealth_request;
    float received;

    int guard_request;

    Group(int id, int land, int groupsize) : age(0), id(id), wealth(0), land(land), npaying(groupsize),
        guard_strength(20),guard_cost(2),nguards(0),nused(0),nundefended(0),req_to_rec(0.0f),received(0.0f),
        guard_request(0),
        memberlist(groupsize)
    {
        name = id;
        if (id>=NGROUP_NAMES){
            string new_gname = generate_name();
            gnames.push_back(new_gname);
        }
    }

    // Decisions

    void will_desire_how_many_guards(vector<int> victim_homes,vector<int> lused, vector<int> lundefended,vector<int> guards_left,
                                     vector<int> lused_soldier, vector<int> lunused_soldier);

    float will_try_to_raise_how_much();

    int will_request_how_many_guards();

    void set_tasks(Person& pleader);

    bool will_try_to_defend();

    // Actions

    void choose_leadership(vector<Person>& people);

    void assess_defence(vector<int> victim_homes,vector<int> lused, vector<int> lundefended,vector<int> guards_left,
                        vector<int> lused_soldier, vector<int> lunused_soldier);

    void set_wealth_request();

    void assess_wealth_request();

    void set_task_request(Person& pleader);

    tuple<float,int> provide_defense(int victim_ind, int location);
};

#endif
