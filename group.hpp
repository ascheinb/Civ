#ifndef GROUP_HPP
#define GROUP_HPP
#include <vector>
#include <string.h>
#include "random.hpp"
#include "person.hpp"
#include "names.hpp"

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
    int npaying;
    float req_to_rec; // ratio of requested/received

    // Will be a function later
    float wealth_request;
    float received;

    int guard_request;

    Group(int id, int land, int npaying) : id(id), wealth(0), land(land), npaying(npaying),
        guard_strength(20),guard_cost(2),nguards(0),nused(0),nundefended(0),req_to_rec(0.0f),received(0.0f),
        guard_request(0)
    {
        name = id;
        if (id>=NGROUP_NAMES){
            // Better create a new name
            char ucons[21] = "QWRTPSDFGHJKLZXCVBNM";
            char lcons[21] = "qwrtpsdfghjklzxcvbnm";
            char uvows[7] = "EYUIOA";
            char lvows[7] = "eyuioa";
            std::string new_gname;
            int namelength=5+rand_f1()*3;
            bool wasvow=true;
            bool isvow=false;
            for (int i=0;i<namelength;i++){
                if (!wasvow && !isvow) // CC
                    {wasvow=isvow;isvow=true;}
                else if (!wasvow && isvow) // CA
                    {wasvow=isvow;isvow=false;}
                else if (i==namelength-1) // AC (dont end in ACC)
                    {wasvow=isvow;isvow=true;}
                else // AC
                    {wasvow=isvow;isvow=((int)(rand_f1()*6)==0);}

                if (i==0){
                    if (isvow) new_gname += uvows[(int)(rand_f1()*6)];
                    else new_gname += ucons [(int)(rand_f1()*20)];
                } else {
                    if (isvow) new_gname += lvows[(int)(rand_f1()*6)];
                    else new_gname += lcons [(int)(rand_f1()*20)];
                }
            }
            gnames.push_back(new_gname);
        }
    }

    void set_wealth_request(){
        // Adjust request based on what would have been nice last turn
        int adjustment= (nguards>nused) ? -1 : 0;
        adjustment = (nundefended>0) ? 10 : adjustment; // Aggressively pessimistic
        float nneeded = nguards + adjustment;
        wealth_request=req_to_rec*guard_cost*nneeded/npaying; // Requests going out to each member
//if (id==0) printf("\n%s had %d used and %d undef, requests %.3f to %d members",gnames[name].c_str(),nused,nundefended,wealth_request,npaying);
        npaying=0; // Set this to zero to get an accurate count next time
//        wealth_request=0.0f;
    }

    void assess_wealth_request(){
        if (received == 0.0f){
            req_to_rec = 1.0f;
        }else{
            req_to_rec = std::min(wealth_request/received*npaying,10.0f);
        }
        wealth += received;
        received = 0.0f;
    }

    void set_task_request(){
//if (id==0) printf("\n%s raised %.3f ",gnames[name].c_str(),received);
        nguards=0; // Assume have to rehire all guards each turn
        if (false){ // INFINITE MERCENARY LABOR
            // Assume mercenary labor for now: Buy guards
            nguards = wealth/guard_cost; // floor
            wealth -= nguards*guard_cost;
        } else {
            guard_request = wealth/guard_cost; // Find this many people to hire
        }

        nused=0;
        nundefended=0;
//if (id==0) printf("and bought %d guards",nguards);
    }

    void set_tasks(){
//if (id==0) printf("\n%s hired %d guards, wanted %d",gnames[name].c_str(),nguards,guard_request);
    }
};

#endif
