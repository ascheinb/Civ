#ifndef PERSON_HPP
#define PERSON_HPP
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

#define GROW 0
#define FORAGE 1
#define GUARD 2
#define TRAITMAX 32

// Age of adulthood
#define ADULT 48
// Agreeableness min to reset workrate when having kids
#define KIDCARE 8
// Level of fondness needed to share food (unless your kid)
#define FONDSHARE 5
// Agreeableness below which you are ok with stealing
#define THIEF 9
// Size of friend group required to form an official group
#define SIZEFORMGROUP 15
// Level of fondness required to qualify as friend group
#define FONDFORMGROUP 6 //4
// LOYALTY
#define SOCIAL_LOYALTY 1.0f
#define LOYALTY_EROSION 3.0f
#define LOYALTY_DEFENDED 4.0f
#define INTRO_LOYALTY 1.0f
#define INIT_LOYALTY 24.0f
// CONTENTEDNESS
#define FREE_TIME_CNTDNESS 2.0f
#define LUX_CNTDNESS 1.0f
#define FRIEND_FEED_CNTDNESS 1.0f
#define CHILD_FEED_CNTDNESS 2.0f
//  Buggy, must be 1 for now:
#define ACTIONS_PER_GUARD 1
// Food to survive
#define FOOD_TO_SURVIVE 1.0f
// Minimum kept for oneself rather than giving away food
#define STD_LUX 0.5f
// Max luxury
#define MAX_LUX 10.0f

using std::vector;
using std::max;
using std::min;

class Population;

enum PersonalityTrait{
    Extroversion=0,
    Agreeableness,
    Conscientiousness,
    Neuroticism,
    Openness
};

std::vector<std::string> trait_names{ "Extroversion","Agreeableness", "Conscientiousness", "Neuroticism", "Openness"};

class Person{
    public:
    // fixed
    int id;
    int lifespan;
    bool female;
    int name;

    // changing
    int age;
    bool will_starve;

    int home;
    int worktype;
    float old_contentedness;
    float contentedness;
    float old_workrate;
    float workrate;
    float old_luxrate;
    float luxrate;

    int employer;
    int employee_id;

    // Personality
    int extroversion; // Controls branch-out socialization
    int agreeableness; // Controls theft and caretaking
    int conscientiousness; // Controls group loyalty degradation rate
    int neuroticism; // Controls savings vs luxury spending
    int openness; // Controls willingness to move

    // Relationships
    vector<Relationship> rships;
    vector<Membership> mships;

    float wealth; // temporary

    // Simulation
    bool watch;
    bool play;

    // Initial generation
    Person(int id, int age, int lifespan, int home ) : id(id), age(age), lifespan(lifespan), will_starve(false), home(home), wealth(0), watch(false), play(false) {
        female = chance(0.5); // 50% chance of being female
        name = female ? rand_int(NF_NAMES) : NF_NAMES + rand_int(NM_NAMES);
        extroversion = 16 + rand_int(16);
        agreeableness = 16 + rand_int(16);
        conscientiousness = 8 + rand_int(16);
        neuroticism = 8 + rand_int(16);
        openness = 8 + rand_int(16);
        worktype=FORAGE;
        workrate=1.0f;
        luxrate=0.0f;
        old_workrate=1.0f;
        old_luxrate=0.0f;
        old_contentedness=0.0f;
    }

    // Birth
    Person(int id, Person* mom, Person& dad) : id(id), age(0), will_starve(false), wealth(0.0), watch(false), play(false) {
        lifespan = (mom->lifespan+dad.lifespan)/2;
        female = chance(0.5); // 50% chance of being female
        name = female ? rand_int(NF_NAMES) : NF_NAMES + rand_int(NM_NAMES);

        int mutation_rate = 2; // parents avg +/- 0,1,or 2
        extroversion =  (mom->extroversion +dad.extroversion +rand_int(2))/2 + rand_int(1+2*mutation_rate)-mutation_rate;
        int one_parent=rand_int(2);
        agreeableness = (mom->agreeableness*one_parent+dad.agreeableness*(1-one_parent)) + rand_int(1+2*mutation_rate)-mutation_rate;

        conscientiousness =  (mom->conscientiousness+dad.conscientiousness+rand_int(2))/2 + rand_int(1+2*mutation_rate)-mutation_rate;
        neuroticism =  (mom->neuroticism+dad.neuroticism+rand_int(2))/2 + rand_int(1+2*mutation_rate)-mutation_rate;
        openness =  (mom->openness+dad.openness+rand_int(2))/2 + rand_int(1+2*mutation_rate)-mutation_rate;

        // Stay in range
        extroversion=max(min(extroversion,TRAITMAX),0);
        agreeableness=max(min(agreeableness,TRAITMAX),0);
        conscientiousness=max(min(conscientiousness,TRAITMAX),0);
        neuroticism=max(min(neuroticism,TRAITMAX),0);
        openness=max(min(openness,TRAITMAX),0);

        // Home
        home = dad.home; // Patrilineal home for now

        // Learn work habits from parents
        worktype=GROW;
        workrate=(mom->workrate + dad.workrate)/2.0f;
        luxrate=(mom->luxrate + dad.luxrate)/2.0f;
        old_workrate=workrate;
        old_luxrate=luxrate;
        old_contentedness=0.0f;

        // Create relationships
        rships.push_back(Relationship(mom->id,Parent));
        mom->rships.push_back(Relationship(id,Child));
        rships.push_back(Relationship(dad.id,Parent));
        dad.rships.push_back(Relationship(id,Child));

        // Set group membership: dad's last name, affiliation from both
        for (int i=0;i<dad.mships.size();i++){
            mships.push_back(Membership(dad.mships[i].id,INIT_LOYALTY));
        }
        // Join mom's groups if not already in them
        for (int i=0;i<mom->mships.size();i++){
            if (!is_member(mom->mships[i].id)) mships.push_back(Membership(mom->mships[i].id,INIT_LOYALTY));
        }
    }

    // Useful utilities
    bool is_member(int group_id);

    int mship_index(int group_id);

    bool knows(int person_id);

    int rship_index(int person_id);

    int random_local_friend(vector<Person>& people, vector<int>& id2ind);

    // Ask question and wait for UI thread to get answer
    template<typename T>
    T decision(const char* question);

    template<typename T>
    T decision(const char* question, T tmin, T tmax);

    // Decisions

    int will_choose_which_father(int fertility_age, vector<Person>& people, vector<int>& id2ind,Nature& nature);

    float how_hard_will_work();

    bool will_move();

    int where_will_move(Nature& nature);

    bool will_take(bool ordered);

    int whom_will_take_from(vector<Person>& people, vector<Group>& groups,Nature& nature, int ordered);

    bool will_risk_taking(float amt_to_steal, float success_rate, bool ordered);

    bool will_give_food(int i_rship, Person& p);

    float how_much_food_will_give(Person& p);

    int will_choose_which_friend(vector<Person>& people, vector<int>& id2ind);

    bool will_branch_out();

    float how_much_will_consume();

    float how_much_will_tithe(float req, Group& group);

    bool will_accept_task();

    bool will_bump_workrate();

    // Leadership decisions
    float how_much_will_skim(Group& group);

    int how_many_attackers(Group& group);

    // Actions

    void evaluate_choices();

    void move_tiles(Nature& nature, int new_home);

    void do_long_action(Nature& nature);

    void take_by_force(vector<Person>& people, vector<Group>& groups,Nature& nature);

    void feed_friends(vector<Person>& people, vector<int>& id2ind);

    void socialize(vector<Person>& people, vector<int>& id2ind, vector<Group>& groups);

    void erode_loyalty();

    void purge_memberships(vector<Group>& groups);

    void luxury();

    void respond_to_wealth_requests(vector<Group>& groups);

    void respond_to_task_requests(vector<Group>& groups,int this_ind);

    void purge_rships(int max_rships, vector<Person>& people, vector<int>& id2ind);

    int breed(int next_id, int fertility_age, float fertility_rate, vector<Person>& people, vector<int>& id2ind, Nature& nature);

};

#endif
