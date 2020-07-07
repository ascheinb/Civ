#ifndef MODEL_HPP
#define MODEL_HPP

#include "names.hpp"
#include "random.hpp"
#include "population.hpp"
#include "nature.hpp"
#include "graphics.hpp"
#include "interface.hpp"

struct Model{
    Nature nature;
    Population p;
    SimVar<int> nkids;
    SimVar<int> nstarved;
    SimVar<int> ndied;
    SimVar<int> nppl;
    vector<float> plot_fracs;
    float plot_avg;

    // Group-checking diagnostics
    int ncreated;
    int nextant;
    int nmerged;

    TimerManager timer;

    int n_turns;
    float carrying_capacity;
    bool watch;
    int watch_start_year;

    int i_turn;

    Model(int initial_n_ppl, int n_years, float min_food_gen, float max_food_gen, int climate_type, int mapsize, int mapwidth)
        : nkids(n_years*4),nstarved(n_years*4),ndied(n_years*4),nppl(n_years*4),
          nature(min_food_gen,max_food_gen,climate_type,mapsize,mapwidth),
          p(initial_n_ppl,mapsize),
          plot_fracs(33)
    {
        carrying_capacity = (max_food_gen+min_food_gen)/2/FOOD_TO_SURVIVE; // Assuming avg is avg of min and max
        n_turns = n_years*4; // A turn is one season
        i_turn = 1;
        watch = false;
        watch_start_year=500;

        // Group checking diagnostics
        ncreated=0; nextant=0; nmerged=0;
        p.sum_nage=0; p.sum_dage=0;

        printf("\n ******** SIMULATION BEGINS ******* \n");
    }

    void set_fracs(vector<float>& fracs, float& avg){
        // Extroversion
        for (int i=0;i<fracs.size();i++){
            fracs[i] = p.frac(i,[](int i,Person& h){return h.extroversion==i;});
        }
        avg = p.avg([](Person& h){return h.extroversion;});
    }

    void advance(){
        timer.scope();
        timer.start("watch and gen_food");
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
                first_watch=p.id2ind[p.groups[max_id].leader];
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
    timer.stop(); timer.start("eval_choices");
        p.evaluate_choices();
    timer.stop(); timer.start("new_groups");
        ncreated+=p.new_groups();
    timer.stop(); timer.start("leadership");
        p.leadership();
    timer.stop(); timer.start("wealth requests");
        p.wealth_requests();
    timer.stop(); timer.start("task requests");
        p.task_requests(i_turn);
    timer.stop(); timer.start("long_actions");
        p.do_long_actions(nature);
    timer.stop(); timer.start("update_residents");
        p.update_residents(nature);
    timer.stop(); timer.start("Steal");
        p.take_by_force(i_turn,nature);
    timer.stop(); timer.start("Assess defense");
        p.assess_defence();
    timer.stop(); timer.start("feed");
        p.feed_friends();
    timer.stop(); timer.start("socialize");
        p.socialize();
    timer.stop(); timer.start("erode loyalty");
        p.erode_loyalty();
    timer.stop(); timer.start("purge");
        p.purge_memberships();
    timer.stop(); timer.start("merge_groups");
        int nexb=p.get_nextant();
        p.update_memberlists();
        p.merge_groups();
        nmerged=nexb-p.get_nextant();
    timer.stop(); timer.start("eat/enjoy");
        p.survive();
        p.luxury();
    timer.stop(); timer.start("age");
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
        if (p.person.size()==0) {printf("\nPopulation went extinct! :("); return;}
    timer.stop(); timer.start("clean up");
        // Clean up relationships, memberlists, residence lists
        p.purge_rships();
        p.update_residents(nature);
        p.update_memberlists();
    timer.stop(); timer.start("breed");
        // Breed
        int n_kids = p.breed(nature);
        nkids.add(i_turn,n_kids);
    timer.stop(); timer.start("tile ownership");
        // Determine tile ownership (for map viewing - rate could be less)
        determine_owners(p,nature);
    timer.stop(); timer.start("plotting");
        set_fracs(plot_fracs, plot_avg);
    timer.stop();
        int n_ppl = p.person.size();
        nppl.add(i_turn,n_ppl);

        //*** SIMULATION ***//
        // Report
        if (i_turn%480==1 || i_turn==n_turns){
            int extant_groups = p.get_nextant();
            printf("\nAverage workrate: %.3f", p.avg([](Person& h){return h.workrate;}));
            printf("\nAverage luxrate: %.3f", p.avg([](Person& h){return h.luxrate;}));
            printf("\nPercent thieves: %.1f%%", p.frac([](Person& h){return h.agreeableness<=9;})*100);
            printf("\nAverage #mships: %.1f", p.avg([](Person& h){return h.mships.size();}));
            map_by_geogroup(p,nature);
            histograms(p);
        }

    }

    void conclusions(){
        //ctrl.active=false; // Exit UI

        timer.print();
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
};

#endif
