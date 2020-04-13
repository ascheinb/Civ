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

    // Read in names
    fill_names();

    bool debug=false;
    int initial_n_ppl = 200;
    int n_years = 4000;
    int n_turns = n_years*4; // A turn is one season
    int min_food_gen=200;
    int max_food_gen=400;
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
        p.person[first_watch].watch=true;
    }

    printf("\n ******** SIMULATION BEGINS ******* \n");
    for (int i_turn = 1; i_turn <= n_turns; i_turn++){
        //*** NATURE ***//
        nature.generate_food();
        if (watch) printf("\nNature provided %d food this season.",nature.food_available);

        //*** LONG INDIVIDUAL ACTIONS ***//
        p.do_long_actions(nature.food_available);

        // Check starvation status
        p.check_starvation();

        //*** SHORT GROUP ACTIONS ***//

        //*** SHORT INDIVIDUAL ACTIONS ***//
        p.feed_friends();

        p.socialize();

        //*** INVOLUNTARY INDIVIDUAL ACTIONS ***//
        // Eat
        p.eat();

        // Age
        p.age();

        // Deaths
        int n_died;
        int n_starved;
        std::tie(n_died,n_starved) = p.die();
        nstarved.add(i_turn,n_starved);
        ndied.add(i_turn,n_died);

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
//            p.report(i_turn);
//            printf("\nBorn: %d, Died (starved): %d (%d)",n_kids,n_died,n_starved);
printf("\nAverage extroversion: %.1f", p.avg([](Person& h){return h.extroversion;}));
        }

        // Pause
        if (watch && i_turn%4==0){
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
    printf("\nPercent extroverts: %.1f%%", p.frac([](Person& h){return h.extroversion>=16;})*100);
    printf("\nAverage #rships for intraverts at final step: %.1f", p.avg_in([](Person& h){return std::make_tuple(h.rships.size(),h.extroversion<16);}));
    printf("\nAverage #rships for extroverts at final step: %.1f", p.avg_in([](Person& h){return std::make_tuple(h.rships.size(),h.extroversion>=16);}));
    printf("\nAverage population: %.0f",nppl.eq_avg());
    printf("\nAverage deaths/turn: %.3f, starvation: %.0f%%\n",ndied.eq_avg(), 100*nstarved.eq_avg()/ndied.eq_avg());
    //nkids.write("nkids.txt");
}

