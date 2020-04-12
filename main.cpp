#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include "random.hpp"
#include "person.hpp"
#include "population.hpp"
#include "interface.hpp"

int main(){
    /* initialize random seed: */
    srand (time(NULL));

    int initial_n_ppl = 20;
    int n_turns = 100000;
    SimVar<int> nkids(n_turns);

    Population p(initial_n_ppl);

    printf("\n ******** SIMULATION BEGINS ******* \n");
    for (int i_turn = 1; i_turn <= n_turns; i_turn++){
        //*** NATURE ***//
        int food_available = 1000;

        //*** LONG INDIVIDUAL ACTIONS ***//
        p.do_long_actions(food_available);

        // Check starvation status
        p.check_starvation();

        //*** SHORT GROUP ACTIONS ***//

        //*** SHORT INDIVIDUAL ACTIONS ***//

        // Eat
        p.eat();

        // Age
        p.age();

        // Deaths
        p.die();

        // Breed
        int n_kids = p.breed();
        nkids.add(i_turn,n_kids);

        //*** SIMULATION ***//
        // Report
        if (i_turn%240==1 || i_turn==n_turns){
            p.report(i_turn);
        }

        // Pause
        if (i_turn%960==1) std::cin.get();
    }
    nkids.write("nkids.txt");
}

