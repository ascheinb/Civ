#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include "names.hpp"
#include "random.hpp"
#include "person.hpp"
#include "population.hpp"
#include "nature.hpp"
#include "graphics.hpp"
#include "interface.hpp"

int main(){
    /* initialize random seed: */
//    srand (time(NULL));
srand(5);
    // Read in names
    fill_names();

    bool debug=false;
    int initial_n_ppl = 300;
    int n_years = 2000;
    int n_turns = n_years*4; // A turn is one season
    float min_food_gen=600;
    float max_food_gen=600;
    float carrying_capacity = (max_food_gen+min_food_gen)/2/FOOD_TO_SURVIVE; // Assuming avg is avg of min and max
    int climate_type = 1; // 0 is uniform; 1 has cold poles
    int mapsize=64; // Must be divisible by mapwidth
    int mapwidth=8; // Keep even for map_by_groups to work
    bool watch = false;
    int watch_start_year=500;
    SimVar<int> nkids(n_turns);
    SimVar<int> nstarved(n_turns);
    SimVar<int> ndied(n_turns);
    SimVar<int> nppl(n_turns);

    Nature nature(min_food_gen,max_food_gen,climate_type,mapsize,mapwidth);
    Population p(initial_n_ppl,mapsize);
int ncreated=0; int nextant=0; int nmerged=0; p.sum_nage=0; p.sum_dage=0;
    printf("\n ******** SIMULATION BEGINS ******* \n");
    for (int i_turn = 1; i_turn <= n_turns; i_turn++){
        // Check watch
        if (watch && i_turn==(watch_start_year*4-3)){
            // Choose who to watch; start with someone middleaged
            int first_watch=p.person.size()/2;
            p.person[first_watch].watch=true;
        }

        //*** NATURE ***//
        nature.generate_food();
//if (i_turn>0) printf("\nTurn %d",i_turn);
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
//if (i_turn>456){return 0;}
        p.task_requests(i_turn);
//if (i_turn>456){return 0;}
        p.do_long_actions(nature);
        p.update_residents(nature);
//if (i_turn>456){return 0;}
//printf("i_turn: %d", i_turn);
        if (i_turn>240){
            p.take_by_force(i_turn,nature);
        }
//if (i_turn>456){return 0;}
        p.assess_defence();
//if (i_turn>456){return 0;}
        p.feed_friends();

        p.socialize();
int nexb=p.get_nextant();
        p.update_memberlists();
        p.merge_groups();
nmerged=nexb-p.get_nextant();
        p.survive();
        p.luxury();

        p.wealth_requests();
//if (i_turn>456){return 0;}
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
        std::tie(n_died,n_starved) = p.die();
        nstarved.add(i_turn,n_starved);
        ndied.add(i_turn,n_died);

        // Exit simulation if no people
        if (p.person.size()==0) {printf("\nPopulation went extinct! :("); break;}

        // Clean up relationships, memberlists, residence lists
        p.purge_rships();
        p.update_residents(nature);
        p.update_memberlists();

        // Breed
        int n_kids = p.breed();
        nkids.add(i_turn,n_kids);

        int n_ppl = p.person.size();
        nppl.add(i_turn,n_ppl);

        //*** SIMULATION ***//
        // Report
        if (i_turn%480==1 || i_turn==n_turns){
            int extant_groups = p.get_nextant();
printf("\nAverage workrate: %.3f", p.avg([](Person& h){return h.workrate;}));
printf("\nAverage agreeableness: %.1f", p.avg([](Person& h){return h.agreeableness;}));
printf("\nAverage conscientiousness: %.1f", p.avg([](Person& h){return h.conscientiousness;}));
printf("\nPercent thieves: %.1f%%", p.frac([](Person& h){return h.agreeableness<=9;})*100);
printf("\nAverage #mships: %.1f", p.avg([](Person& h){return h.mships.size();}));
map_by_geogroup(p,nature);

//map_by_groups(p,nature);
//map_by_population(p,nature);
histograms(p);

        }
//if (i_turn>456){return 0;}
        // Pause
        if (watch && i_turn>=(watch_start_year*4-3) && i_turn%4==0){
            printf("\n");
            std::cin.get();
        }
    }

//histograms(p);
    map_by_geogroup(p,nature);
    map_by_groups(p,nature);
    map_by_population(p,nature);
    printf("\nAverage age at final step: %.1f", p.avg([](Person& h){return h.age;})/4);
    printf("\nGender ratio at final step: %.1f%% women", p.frac([](Person& h){return h.female;})*100);
    float avg_ex=p.avg([](Person& h){return h.extroversion;});
    printf("\nAverage extroversion at final step: %.1f", avg_ex);
    printf("\nPercent intraverts: %.1f%%", p.frac(avg_ex-2,[](int x,Person& h){return h.extroversion<x;})*100);
    printf("\nPercent extroverts: %.1f%%", p.frac(avg_ex+2,[](int x,Person& h){return h.extroversion>x;})*100);
    printf("\nAverage #rships for intraverts: %.1f", p.avg_in(avg_ex-2,[](int x,Person& h){return std::make_tuple(h.rships.size(),h.extroversion<x);}));
    printf("\nAverage fondness for intraverts: %.1f", p.avg_in(avg_ex-2,[](int x,Person& h){float tfond=0.0; for (int i=0;i<h.rships.size();i++){tfond+=h.rships[i].fondness_to;} return std::make_tuple(tfond/h.rships.size(),h.extroversion<x);}));
    printf("\nAverage #rships for extroverts: %.1f", p.avg_in(avg_ex+2,[](int x,Person& h){return std::make_tuple(h.rships.size(),h.extroversion>x);}));
    printf("\nAverage fondness for extroverts: %.1f", p.avg_in(avg_ex+2,[](int x,Person& h){float tfond=0.0; for (int i=0;i<h.rships.size();i++){tfond+=h.rships[i].fondness_to;} return std::make_tuple(tfond/h.rships.size(),h.extroversion>x);}));
    printf("\nAverage population: %.0f (%.1f%% of carrying capacity)",nppl.eq_avg(),100*nppl.eq_avg()/carrying_capacity);
    printf("\nAverage deaths/turn: %.3f, starvation: %.0f%%\n",ndied.eq_avg(), 100*nstarved.eq_avg()/ndied.eq_avg());
    //nkids.write("nkids.txt");
}

