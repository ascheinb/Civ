#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <gtkmm/application.h>

//#include "names.hpp"
//#include "random.hpp"

//#include "control.hpp"

//#include "population.hpp"
//#include "nature.hpp"
//#include "graphics.hpp"
//#include "interface.hpp"
//#include "model.hpp"
#include "civwindow.hpp"

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
#ifdef DUMB

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
            printf("\n%d.\t%s%s\t%.2f\t\t%lu\t%.2f\t%s",i,gnames[model.p.groups[ind].name].c_str(),atab,p->mships[i].loyalty_to,model.p.groups[ind].memberlist.size(),model.p.groups[ind].wealth,names[model.p.person[model.p.id2ind[model.p.groups[ind].leader]].name].c_str());
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

#endif
int main(){
    /* initialize random seed: */
    srand (time(NULL));
//srand(5);
    // Read in names
    fill_names();

    int initial_n_ppl = 2000;
    int n_years = 2000;
    float min_food_gen=10000;
    float max_food_gen=10000;
    int climate_type = 1; // 0 is uniform; 1 has cold poles
    int mapsize=216; // Must be divisible by mapwidth
    int mapwidth=18; // Keep even for map_by_groups to work

    auto app = Gtk::Application::create();
    CivWindow window(initial_n_ppl, n_years, min_food_gen, max_food_gen, climate_type, mapsize, mapwidth);

    return app->run(window);
}
