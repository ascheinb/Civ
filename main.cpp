#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>

#include "names.hpp"
#include "random.hpp"

struct Control{
    bool active=true;
    bool needs_float=false;
    float input_float=0.0f;
    float floatmin=0.0f;
    float floatmax=0.0f;
    bool needs_int=false;
    int input_int=0;
    int intmin=0;
    int intmax=0;
    bool range=false;
    bool needs_bool=false;
    bool input_bool=false;
    int info_id=0; // Which person we're focused on
};

Control ctrl;

#include "population.hpp"
#include "nature.hpp"
#include "graphics.hpp"
#include "interface.hpp"

using std::vector;
using std::string;
using std::tuple;
using std::make_tuple;
using std::tie;
using std::strcpy;
using std::cin;
using std::is_same;
using std::stoi;
using std::stof;

void run_simulation(Nature& nature, Population& p, SimVar<int>& nkids, SimVar<int>& nstarved, SimVar<int>& ndied, SimVar<int>& nppl, int n_turns, float carrying_capacity, bool watch, int watch_start_year){
    int ncreated=0; int nextant=0; int nmerged=0; p.sum_nage=0; p.sum_dage=0;
    printf("\n ******** SIMULATION BEGINS ******* \n");
    for (int i_turn = 1; i_turn <= n_turns; i_turn++){
        // Check watch
        if (watch && i_turn==(watch_start_year*4-3)){
            int first_watch;
            if (false){
                // Option 1:
                // Choose someone random of median age
                first_watch=p.person.size()/2;
            } else {
                // Option 2:
                // Find biggest group
                int max_size=0;
                int max_id=0;
                for (int i=0; i<p.groups.size();i++){
                    if(p.groups[i].memberlist.size()>max_size){
                        max_id=i;
                        max_size=p.groups[i].memberlist.size();
                    }
                }
                first_watch=p.groups[max_id].memberlist[p.groups[max_id].leader];
            }
            p.person[first_watch].watch=true;
            p.person[first_watch].play=true;
        }

        //*** NATURE ***//
        nature.generate_food();

        if (watch && i_turn>=(watch_start_year*4-3)){
            if(i_turn%4==1){
                printf("\nYear %d begins",1+(i_turn-1)/4);
                printf("\nFollowing:");
                for (int i=0;i<p.person.size();i++)
                    if (p.person[i].watch) printf(" -%s %s (%d)", names[p.person[i].name].c_str(), gnames[p.groups[p.person[i].mships[0].id].name].c_str(), p.person[i].age/4);
            }
            printf("\n      (%.0f food this season)",nature.food_available);
        }

        p.evaluate_choices();

        p.leadership();
        p.task_requests(i_turn);

        p.do_long_actions(nature);
        p.update_residents(nature);


        if (i_turn>240){
            p.take_by_force(i_turn,nature);
        }

        p.assess_defence();

        p.feed_friends();

        p.socialize();
        p.purge_memberships();

        int nexb=p.get_nextant();
        p.update_memberlists();
        p.merge_groups();
        nmerged=nexb-p.get_nextant();

        p.survive();
        p.luxury();

        p.wealth_requests();

        ncreated+=p.new_groups(); // Move before wealth request

        p.age();
        if (i_turn%480==1 || i_turn==n_turns){
            printf("\n\nYear: %d, Population: %lu",i_turn/4,p.person.size());
            int nowextant = p.get_nextant();
            int ndestroyed = ncreated-(nowextant-nextant)-nmerged;
            if (i_turn>1)    printf("\nGroups info: nextant: %d, created: %d, destroyed: %d, merged %d, avg duration: %.1f years",nowextant, ncreated,ndestroyed, nmerged, (0.25f*p.sum_dage)/p.sum_nage);
            ncreated=0;nmerged=0; nextant=nowextant; p.sum_nage=0; p.sum_dage=0;
            printf("\nPercent growing: %.1f%%", p.frac([](Person& h){return h.worktype==GROW;})*100);
            printf("\nPercent foraging: %.1f%%", p.frac([](Person& h){return h.worktype==FORAGE;})*100);
            printf("\nPercent guarding: %.1f%%", p.frac([](Person& h){return h.worktype==GUARD;})*100);
        }
        // Deaths
        int n_died;
        int n_starved;
        tie(n_died,n_starved) = p.die();
        nstarved.add(i_turn,n_starved);
        ndied.add(i_turn,n_died);

        // Exit simulation if no people
        if (p.person.size()==0) {printf("\nPopulation went extinct! :("); break;}

        // Clean up relationships, memberlists, residence lists
        p.purge_rships();
        p.update_residents(nature);
        p.update_memberlists();

        // Breed
        int n_kids = p.breed(nature);
        nkids.add(i_turn,n_kids);

        int n_ppl = p.person.size();
        nppl.add(i_turn,n_ppl);

        //*** SIMULATION ***//
        // Report
        if (i_turn%480==1 || i_turn==n_turns){
            int extant_groups = p.get_nextant();
            printf("\nAverage workrate: %.3f", p.avg([](Person& h){return h.workrate;}));
            printf("\nPercent thieves: %.1f%%", p.frac([](Person& h){return h.agreeableness<=9;})*100);
            printf("\nAverage #mships: %.1f", p.avg([](Person& h){return h.mships.size();}));
            map_by_geogroup(p,nature);
            histograms(p);
        }
    }
    ctrl.active=false; // Exit UI

    if (p.person.size()==0) return; // If no population, no final stats
    map_by_geogroup(p,nature);
    map_by_groups(p,nature);
    map_by_population(p,nature);
    printf("\nAverage age at final step: %.1f", p.avg([](Person& h){return h.age;})/4);
    printf("\nGender ratio at final step: %.1f%% women", p.frac([](Person& h){return h.female;})*100);
    float avg_ex=p.avg([](Person& h){return h.extroversion;});
    printf("\nPercent intraverts: %.1f%%", p.frac(avg_ex-2,[](int x,Person& h){return h.extroversion<x;})*100);
    printf("\nPercent extroverts: %.1f%%", p.frac(avg_ex+2,[](int x,Person& h){return h.extroversion>x;})*100);
    printf("\nAverage #rships for intraverts: %.1f", p.avg_in(avg_ex-2,[](int x,Person& h){return make_tuple(h.rships.size(),h.extroversion<x);}));
    printf("\nAverage fondness for intraverts: %.1f", p.avg_in(avg_ex-2,[](int x,Person& h){float tfond=0.0; for (int i=0;i<h.rships.size();i++){tfond+=h.rships[i].fondness_to;} return make_tuple(tfond/h.rships.size(),h.extroversion<x);}));
    printf("\nAverage #rships for extroverts: %.1f", p.avg_in(avg_ex+2,[](int x,Person& h){return make_tuple(h.rships.size(),h.extroversion>x);}));
    printf("\nAverage fondness for extroverts: %.1f", p.avg_in(avg_ex+2,[](int x,Person& h){float tfond=0.0; for (int i=0;i<h.rships.size();i++){tfond+=h.rships[i].fondness_to;} return make_tuple(tfond/h.rships.size(),h.extroversion>x);}));
    printf("\nAverage population: %.0f (%.1f%% of carrying capacity)",nppl.eq_avg(),100*nppl.eq_avg()/carrying_capacity);
    printf("\nAverage deaths/turn: %.3f, starvation: %.0f%%\n",ndied.eq_avg(), 100*nstarved.eq_avg()/ndied.eq_avg());
    //nkids.write("nkids.txt");
}

struct Model{
    Nature nature;
    Population p;
    SimVar<int> nkids;
    SimVar<int> nstarved;
    SimVar<int> ndied;
    SimVar<int> nppl;
    int n_turns;
    float carrying_capacity;
    bool watch;
    int watch_start_year;

    Model(int initial_n_ppl, int n_years, float min_food_gen, float max_food_gen, int climate_type, int mapsize, int mapwidth)
        : nkids(n_years*4),nstarved(n_years*4),ndied(n_years*4),nppl(n_years*4),
          nature(min_food_gen,max_food_gen,climate_type,mapsize,mapwidth),
          p(initial_n_ppl,mapsize)
    {
        carrying_capacity = (max_food_gen+min_food_gen)/2/FOOD_TO_SURVIVE; // Assuming avg is avg of min and max
        n_turns = n_years*4; // A turn is one season
        watch = false;
        watch_start_year=500;
    }
};


bool more_info(string& input, Model& model, int& focus_id){
    Person* p=&model.p.person[model.p.id2ind[focus_id]];
    if(input.compare("status")==0){
        printf("\nName: %s %s", names[p->name].c_str(), p->mships.size()>0 ? gnames[model.p.groups[p->mships[0].id].name].c_str() : "" );
        printf("\nAge: %d",p->age/4);
        printf("\nWealth: %.2f",p->wealth);
        printf("\n# of relationships: %lu",p->rships.size());
        printf("\n# of memberships: %lu",p->mships.size());
        printf("\n");
    }else if(input.compare("friends")==0){
        printf("\n#\tName\t\tFondness\tAge\tWealth");
        printf("\n______________________________________________________");
        for (int i=0;i<p->rships.size();i++){
            int ind = model.p.id2ind[p->rships[i].person_id];
            char atab[2];
            strcpy(atab,names[model.p.person[ind].name].length()<8 ? "\t" : "");
            printf("\n%d.\t%s%s\t%d\t\t%d\t%.2f",i,names[model.p.person[ind].name].c_str(),atab,p->rships[i].fondness_to,model.p.person[ind].age/4,model.p.person[ind].wealth);
        }
        printf("\n");
    }else if(input.compare("groups")==0){
        printf("\n#\tName\t\tLoyalty to\tSize\tWealth\tLeader");
        printf("\n______________________________________________________________");
        for (int i=0;i<p->mships.size();i++){
            int ind = p->mships[i].id;
            char atab[2];
            strcpy(atab,gnames[model.p.groups[ind].name].length()<8 ? "\t" : "");
            printf("\n%d.\t%s%s\t%.2f\t\t%lu\t%.2f\t%s",i,gnames[model.p.groups[ind].name].c_str(),atab,p->mships[i].loyalty_to,model.p.groups[ind].memberlist.size(),model.p.groups[ind].wealth,names[model.p.person[model.p.groups[ind].memberlist[model.p.groups[ind].leader]].name].c_str());
        }
        printf("\n");
    }else if(input.compare("tile")==0){
        int itile = p->home;
        string terrain("Grass");
        if (model.nature.map[itile].terrain==WATER) terrain="Water";
        if (model.nature.map[itile].terrain==MOUNTAIN) terrain="Mountain";
        printf("\nTerrain: %s", terrain.c_str());
        printf("\nPopulation: %lu", model.nature.map[itile].residents.size());
        printf("\nGroups\t\tLocal members");
        printf("\n_____________________________");
        vector<int> groups;
        vector<int> group_pop;
        tie(groups,group_pop)=get_geogroup(model.p,model.nature,itile);
        for (int i=0;i<groups.size();i++){
            int ind = groups[i];
            char atab[2];
            strcpy(atab,gnames[model.p.groups[ind].name].length()<8 ? "\t" : "");
            printf("\n%s%s\t%d",gnames[model.p.groups[ind].name].c_str(),atab,group_pop[i]);
        }
        printf("\n");
    }else if(input.compare("map")==0){
        map_by_geogroup(model.p,model.nature);
    }else if(input.compare("mapme")==0){
        map_by_geogroup(model.p,model.nature,p->home);
    }else if(input.compare("popmap")==0){
        map_by_population(model.p,model.nature);
    }else if(input.compare("mapgroup")==0){
        string input_sw;
        int switch_num;
        printf("\nChoose mship: ");
        getline(cin, input_sw);
        try {
            switch_num = stoi(input_sw);
            if (switch_num<0 || switch_num>=p->mships.size()) throw -1;
            int mapgroup_focus = p->mships[switch_num].id;
            printf("\nPopulation distribution of %s:",gnames[model.p.groups[mapgroup_focus].name].c_str());
            map_by_population(model.p,model.nature, mapgroup_focus);
        } catch (...) {
            printf("Not a valid input. ");
        }
    }else if(input.compare("switch")==0){
        string input_sw;
        int switch_num;
        printf("\nChoose rship: ");
        getline(cin, input_sw);
        try {
            switch_num = stoi(input_sw);
            if (switch_num<0 || switch_num>=p->rships.size()) throw -1;
            focus_id = p->rships[switch_num].person_id;
        } catch (...) {
            printf("Not a valid input. ");
        }
    }else if(input.compare("reset")==0){
        focus_id = ctrl.info_id;
    }else{
        return false;
    }
    return true;
}


bool yes_or_no(string& input){
    if ((input.compare("y")==0) || (input.compare("n")==0)){
        return (input.compare("y")==0);
    }
    throw -1;
}

template<typename T>
T get_answer(Model& model){
        T num;
        string input;
        int focus_id = ctrl.info_id;
        bool no_answer=true;
        while (no_answer){
            getline(cin, input);
            if(more_info(input,model,focus_id)) continue;
            try {
                if (is_same<T, float>::value) num = stof(input);
                if (is_same<T, int>::value)   num = stoi(input);
                if (is_same<T, bool>::value)  num = yes_or_no(input);
                no_answer=false;
            } catch (...) {
                printf("Not a valid input. ");
            }

            if (no_answer==false && ctrl.range){
                try {
                    if (is_same<T, float>::value) if (num<ctrl.floatmin || num>ctrl.floatmax) throw -1;
                    if (is_same<T, int>::value)   if (num<ctrl.intmin || num>ctrl.intmax) throw -1;
                } catch (...) {
                    printf("Out of range. ");
                    no_answer=true;
                }
            }
        }
        return num;
}

void *interface_with_user(void *arguments){
    Model *model = ((Model *)arguments);

    while (ctrl.active){
        usleep(50); // Check for updates every 50 ms
        if (ctrl.needs_float){
            ctrl.input_float = get_answer<float>(*model);
            ctrl.needs_float=false;
        }
        if (ctrl.needs_int){
            ctrl.input_int = get_answer<int>(*model);
            ctrl.needs_int=false;
        }
        if (ctrl.needs_bool){
            ctrl.input_bool = get_answer<bool>(*model);
            ctrl.needs_bool=false;
        }
    }

    return NULL;
}

void *launch_simulation(void *arguments){
    Model *model = ((Model *)arguments);

    run_simulation(model->nature, model->p, model->nkids, model->nstarved, model->ndied, model->nppl, model->n_turns, model->carrying_capacity, model->watch, model->watch_start_year);

    return NULL;
}

int main(){
    pthread_t threads[2];
    int result_code;
    const int UIth=0;
    const int SIMth=1;

    /* initialize random seed: */
    srand (time(NULL));
//srand(5);
    // Read in names
    fill_names();

    int initial_n_ppl = 2000;
    int n_years = 2000;
    float min_food_gen=5000;
    float max_food_gen=5000;
    int climate_type = 1; // 0 is uniform; 1 has cold poles
    int mapsize=110; // Must be divisible by mapwidth
    int mapwidth=10; // Keep even for map_by_groups to work

    // Initialize model
    Model model(initial_n_ppl, n_years, min_food_gen, max_food_gen, climate_type, mapsize, mapwidth);

    // Create the threads and launch UI and simulation
    result_code = pthread_create(&threads[UIth], NULL, interface_with_user, &model);
    assert(!result_code);
    result_code = pthread_create(&threads[SIMth], NULL, launch_simulation, &model);
    assert(!result_code);

    //wait for each thread to complete
    result_code = pthread_join(threads[UIth], NULL);
    assert(!result_code);
    result_code = pthread_join(threads[SIMth], NULL);
    assert(!result_code);

    return 0;
}
