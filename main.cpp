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
#include "interface.hpp"

int main(){
    /* initialize random seed: */
    srand (time(NULL));
//srand(1);
    // Read in names
    fill_names();

    bool debug=false;
    int initial_n_ppl = 200;
    int n_years = 4000;
    int n_turns = n_years*4; // A turn is one season
    float min_food_gen=600;
    float max_food_gen=600;
    bool watch = false;
    SimVar<int> nkids(n_turns);
    SimVar<int> nstarved(n_turns);
    SimVar<int> ndied(n_turns);
    SimVar<int> nppl(n_turns);

    Population p(initial_n_ppl);
    Nature nature(min_food_gen,max_food_gen);

    if (watch){
        // Choose who to watch; start with someone middleaged
        int first_watch=initial_n_ppl/2;
//        p.person[first_watch].watch=true;
    }

    printf("\n ******** SIMULATION BEGINS ******* \n");
    for (int i_turn = 1; i_turn <= n_turns; i_turn++){
        //*** NATURE ***//
        nature.generate_food();
        if (watch) printf("\nNature provided %.0f food this season.",nature.food_available);

        p.evaluate_choices();

if (i_turn>230)
        p.task_requests();

        p.do_long_actions(nature.food_available);
if (i_turn>240)
        p.take_by_force();

        p.feed_friends();

        p.socialize();

        p.eat();
if (i_turn>230)
        p.wealth_requests();

        p.age();
        if (i_turn%480==1 || i_turn==n_turns){
printf("\nPercent growing: %.1f%%", p.frac([](Person& h){return h.worktype==0;})*100);
printf("\nPercent foraging: %.1f%%", p.frac([](Person& h){return h.worktype==1;})*100);
printf("\nPercent guarding: %.1f%%", p.frac([](Person& h){return h.worktype==2;})*100);
        }
        // Deaths
        int n_died;
        int n_starved;
        std::tie(n_died,n_starved) = p.die();
        nstarved.add(i_turn,n_starved);
        ndied.add(i_turn,n_died);

        // Exit simulation if no people
        if (p.person.size()==0) {printf("\nPopulation went extinct! :("); break;}

        // Clean up relationships
        p.purge_rships();

        // Breed
        int n_kids = p.breed();
        nkids.add(i_turn,n_kids);

        int n_ppl = p.person.size();
        nppl.add(i_turn,n_ppl);

        //*** SIMULATION ***//
        // Report
        if (i_turn%480==1 || i_turn==n_turns){
printf("\nAverage workrate: %.3f", p.avg([](Person& h){return h.workrate;}));
printf("\nAverage agreeableness: %.1f", p.avg([](Person& h){return h.agreeableness;}));
printf("\nPercent thieves: %.1f%%", p.frac([](Person& h){return h.agreeableness<=9;})*100);
//nature.map_by_population(p);
        }

        // Pause
        if (watch && i_turn%4==0 && i_turn>230){
            if (i_turn>0) std::cin.get();
            printf("\nStarting Year %d",1+i_turn/4);
            printf("\nFollowing: ");
            for (int i=0;i<p.person.size();i++)
                if (p.person[i].watch) printf("%s %s (%d) - ", names[p.person[i].name].c_str(), gnames[p.groups[p.person[i].mships[0].id].name].c_str(), p.person[i].age/4);
            
        }
    }
    printf("\nAverage age at final step: %.1f", p.avg([](Person& h){return h.age;})/4);
    printf("\nGender ratio at final step: %.1f%% women", p.frac([](Person& h){return h.female;})*100);
    float avg_extroversion=p.avg([](Person& h){return h.extroversion;});
    printf("\nAverage extroversion at final step: %.1f", avg_extroversion);
    printf("\nPercent intraverts: %.1f%%", p.frac([](Person& h){return h.extroversion<14;})*100);
    printf("\nPercent extroverts: %.1f%%", p.frac([](Person& h){return h.extroversion>18;})*100);
    printf("\nAverage #rships for intraverts: %.1f", p.avg_in([](Person& h){return std::make_tuple(h.rships.size(),h.extroversion<14);}));
    printf("\nAverage fondness for intraverts: %.1f", p.avg_in([](Person& h){float tfond=0.0; for (int i=0;i<h.rships.size();i++){tfond+=h.rships[i].fondness_to;} return std::make_tuple(tfond/h.rships.size(),h.extroversion<14);}));
    printf("\nAverage #rships for extroverts: %.1f", p.avg_in([](Person& h){return std::make_tuple(h.rships.size(),h.extroversion>18);}));
    printf("\nAverage fondness for extroverts: %.1f", p.avg_in([](Person& h){float tfond=0.0; for (int i=0;i<h.rships.size();i++){tfond+=h.rships[i].fondness_to;} return std::make_tuple(tfond/h.rships.size(),h.extroversion>18);}));
    printf("\nAverage population: %.0f",nppl.eq_avg());
    printf("\nAverage deaths/turn: %.3f, starvation: %.0f%%\n",ndied.eq_avg(), 100*nstarved.eq_avg()/ndied.eq_avg());
    //nkids.write("nkids.txt");
}

