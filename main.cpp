#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include "names.hpp"
#include "random.hpp"
#include "person.hpp"
#include "population.hpp"
#include "interface.hpp"

int main(){
    /* initialize random seed: */
    srand (time(NULL));

    // Read in names
    fill_names();

    int initial_n_ppl = 200;
    int n_turns = 10000;
    bool watch = false;
    SimVar<int> nkids(n_turns);
    SimVar<int> nstarved(n_turns);
    SimVar<int> ndied(n_turns);
    SimVar<int> nppl(n_turns);

    Population p(initial_n_ppl);

    if (watch){
        // Choose who to watch; start with someone middleaged
        int first_watch=initial_n_ppl/2;
        p.person[first_watch].watch=true;
    }

    printf("\n ******** SIMULATION BEGINS ******* \n");
    for (int i_turn = 1; i_turn <= n_turns; i_turn++){
        //*** NATURE ***//
        int food_available = 400;

        //*** LONG INDIVIDUAL ACTIONS ***//
        p.do_long_actions(food_available);

        // Check starvation status
        p.check_starvation();

        //*** SHORT GROUP ACTIONS ***//

        //*** SHORT INDIVIDUAL ACTIONS ***//
        p.feed_friends();

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

        // Breed
        int n_kids = p.breed();
        nkids.add(i_turn,n_kids);

        int n_ppl = p.person.size();
        nppl.add(i_turn,n_ppl);

        //*** SIMULATION ***//
        // Report
        if (i_turn%240==1 || i_turn==n_turns){
//            p.report(i_turn);
//            printf("\nBorn: %d, Died (starved): %d (%d)",n_kids,n_died,n_starved);
        }

        // Pause
        if (watch && i_turn%4==0){
            std::cin.get();
            printf("\nStarting Year %d",1+i_turn/4);
        }
    }
    printf("\nAverage population: %.0f\n",nppl.avg());
    printf("\nAverage deaths/turn: %.3f, starvation: %.0f%%\n",ndied.avg(), 100*nstarved.avg()/ndied.avg());
    nkids.write("nkids.txt");
}

