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
    float deployment_strength;
    float deployment_cost;
    int ndeployments;
    int nused;
    int nundefended;
    int npaying;
    float req_to_rec; // ratio of requested/received

    // Will be a function later
    float wealth_request;
    float received;

    Group(int id, int land, int npaying) : id(id), wealth(0), land(land), npaying(npaying),
        deployment_strength(20),deployment_cost(2),ndeployments(0),nused(0),nundefended(0),req_to_rec(0.0f),received(0.0f)
    {
        name = (rand_f1()*NGROUP_NAMES);
    }

    void set_wealth_request(){
        // Adjust request based on what would have been nice last turn
        int adjustment= (ndeployments>nused) ? -1 : 0;
        adjustment = (nundefended>0) ? 10 : adjustment;
        float nneeded = ndeployments + adjustment;
        wealth_request=req_to_rec*deployment_cost*nneeded/npaying; // Requests going out to each member
//if (id==0) printf("\n%s had %d used and %d undef, requests %.3f to %d members",gnames[name].c_str(),nused,nundefended,wealth_request,npaying);
        npaying=0; // Set this to zero to get an accurate count next time
//        wealth_request=0.0f;
    }

    void set_task_request(){
//if (id==0) printf("\n%s raised %.3f ",gnames[name].c_str(),received);
        // Process income
        if (received == 0.0f){
            req_to_rec = 1.0f;
        }else{
            req_to_rec = std::min(wealth_request/received*npaying,10.0f);
        }
        wealth += received;
        received = 0.0f;

        // Assume mercenary labor for now: Buy deployments
        ndeployments = wealth/deployment_cost; // floor
        wealth -= ndeployments*deployment_cost;
//        ndeployments = 1000;//nused + nundefended;
        nused=0;
        nundefended=0;
//if (id==0) printf("and bought %d deployments",ndeployments);
    }
};

#endif
