#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP
#include <vector>
#include <string.h>
#include "random.hpp"
#include "nature.hpp"
#include "population.hpp"

using std::vector;
using std::string;
using std::to_string;
using std::strcat;
using std::make_tuple;
using std::tuple;
using std::tie;

string diag_border(int ind, Nature &n){
    string diags="";
    int nw_ind = n.neighbor(ind,NW);
    int ne_ind = n.neighbor(ind,NE);
    int owner = n.map[ind].owner;
    int nw_owner = (nw_ind==-1 ? -1 : n.map[nw_ind].owner);
    int ne_owner = (ne_ind==-1 ? -1 : n.map[ne_ind].owner);
    int w_ind = n.neighbor(ind,W);
    int e_ind = n.neighbor(ind,E);
    int w_owner = (w_ind==-1 ? -1 : n.map[w_ind].owner);
    int e_owner = (e_ind==-1 ? -1 : n.map[e_ind].owner);
    bool mtn=(n.map[ind].terrain==MOUNTAIN);
    bool nw_mtn=(n.map[nw_ind].terrain==MOUNTAIN);
    bool ne_mtn=(n.map[ne_ind].terrain==MOUNTAIN);
    bool wtr=(n.map[ind].terrain==WATER);
    bool nw_wtr=(n.map[nw_ind].terrain==WATER || nw_ind==-1);
    bool ne_wtr=(n.map[ne_ind].terrain==WATER || ne_ind==-1);

    diags+=" ";
    if ((mtn && !nw_wtr) || (nw_mtn && !wtr) ){
        diags+="^";
    }else if (owner==nw_owner){
        diags+=" ";
    }else if (ne_owner==nw_owner && nw_owner!=w_owner){
        diags+="-";
    }else{
        diags+="/";
    }
    if ((mtn && !nw_wtr && !ne_wtr) || (nw_mtn && ne_mtn && !wtr)){
        diags+="^";
    }else if (owner!=nw_owner && nw_owner!=w_owner & owner!=ne_owner && ne_owner==nw_owner && ne_owner!=e_owner){
        diags+="-";
    }else{
        diags+=" ";
    }
    if ((mtn && !ne_wtr) || (ne_mtn && !wtr)){
        diags+="^";
    }else if (owner==ne_owner){
        diags+=" ";
    }else if (ne_owner==nw_owner && ne_owner!=e_owner){
        diags+="-";
    }else{
        diags+="\\";
    }
    return diags;
}

string diag_border2(int ind, Nature &n){
    string diags="";
    int sw_ind = n.neighbor(ind,SW);
    int se_ind = n.neighbor(ind,SE);
    int owner = n.map[ind].owner;
    int sw_owner = (sw_ind==-1 ? -1 : n.map[sw_ind].owner);
    int se_owner = (se_ind==-1 ? -1 : n.map[se_ind].owner);
    int w_ind = n.neighbor(ind,W);
    int e_ind = n.neighbor(ind,E);
    int w_owner = (w_ind==-1 ? -1 : n.map[w_ind].owner);
    int e_owner = (e_ind==-1 ? -1 : n.map[e_ind].owner);
    bool mtn=(n.map[ind].terrain==MOUNTAIN);
    bool sw_mtn=(n.map[sw_ind].terrain==MOUNTAIN);
    bool se_mtn=(n.map[se_ind].terrain==MOUNTAIN);
    bool wtr=(n.map[ind].terrain==WATER);
    bool sw_wtr=(n.map[sw_ind].terrain==WATER || sw_ind==-1);
    bool se_wtr=(n.map[se_ind].terrain==WATER || se_ind==-1);

    diags+=" ";
    if ((mtn && !sw_wtr) || (sw_mtn && !wtr) ){
        diags+="^";
    }else if (owner==sw_owner){
        diags+=" ";
    }else if (se_owner==sw_owner && sw_owner!=w_owner){
        diags+="-";
    }else{
        diags+="\\";
    }
    if ((mtn && !sw_wtr && !se_wtr) || (sw_mtn && se_mtn && !wtr)){
        diags+="^";
    }else if (owner!=sw_owner && sw_owner!=w_owner & owner!=se_owner && se_owner==sw_owner && se_owner!=e_owner){
        diags+="-";
    }else{
        diags+=" ";
    }
    if ((mtn && !se_wtr) || (se_mtn && !wtr)){
        diags+="^";
    }else if (owner==se_owner){
        diags+=" ";
    }else if (se_owner==sw_owner && se_owner!=e_owner){
        diags+="-";
    }else{
        diags+="/";
    }
    return diags;
}

string vert_border(int ind, Nature &n){
    string verts="";
    int w_ind = n.neighbor(ind,W);
    int owner = n.map[ind].owner;
    int w_owner = (w_ind==-1 ? -1 : n.map[w_ind].owner);
    bool mtn=(n.map[ind].terrain==MOUNTAIN);
    bool w_mtn=(n.map[w_ind].terrain==MOUNTAIN);
    bool wtr=(n.map[ind].terrain==WATER);
    bool w_wtr=(n.map[w_ind].terrain==WATER || w_ind==-1);

    if ((mtn && !w_wtr) || (w_mtn && !wtr)){
        verts+="^";
    }else if (owner==w_owner){
        verts+=" ";
    }else{
        verts="|";
    }
    return verts;
}

void print_map(Nature &n){
    for (int i=0;i<n.nrow;i++){
        printf("\n");
        if (i%2==0){
            string line("");
            for (int j=0;j<n.ncol;j++) line+=diag_border(i*n.ncol+j,n);
            printf("%s",line.c_str());
            if (i>0) printf(" /");
            printf("\n");
        } else {
            string line("");
            for (int j=0;j<n.ncol;j++) line+=diag_border2((i-1)*n.ncol+j,n);
            printf("%s",line.c_str());
            if (n.nrow%2==0 || i<n.nrow) printf(" \\");
            printf("\n  ");
        }
        string line("");
        for (int j=0;j<n.ncol;j++) line+=vert_border(i*n.ncol+j,n)+n.map[i*n.ncol+j].letter;
        printf("%s",line.c_str());
        printf("|");
    }
    if (n.nrow%2==0){
        printf("\n   \\");
        for (int j=0;j<n.ncol-1;j++) printf("--- ");
        printf("-/");
    }
}

// Assumes even number of columns
void map_by_groups(Population &p, Nature &n){
    // Determine # tiles per group
    vector<int> extant_groups;
    printf("\nGroups: ");
    int nameless=0;
    for (int i=0;i<p.person.size();i++){
        if (p.person[i].mships.size()==0)
            {nameless++;continue;}

        int lastname = p.person[i].mships[0].id;
        bool already_present = false;
        // Check if name already appears
        for (int j=0;j<extant_groups.size();j++){
            if (extant_groups[j]==lastname)
                {already_present=true; break;}
        }
        if (!already_present){
            if (i>0) printf(", ");
            printf("%s",gnames[p.groups[lastname].name].c_str());
            extant_groups.push_back(lastname);
        }
    }
    if (nameless>0) printf(" (%d people belonged to no group.)",nameless);
    int ngroups=extant_groups.size();
    if (ngroups==0){
        printf("\nNo groups, skipping map");
        return;
    }
    vector<float> frac(ngroups);
    vector<int> intfrac(ngroups);
    int filled_frac = 0;
    for (int i=0;i<ngroups;i++){
        int gi = extant_groups[i];
        // Frac by first name
        // frac[i]=p.frac(gi,[](int gi, Person& h){return h.mships.size()>0 ? h.mships[0].id==gi : false;})*n.map.size();
        // Frac by loyalty
        frac[i]=p.frac(gi,[](int gi, Person& h){
                float max_loyalty=0.0;
                int ml_id=-1;
                for (int i=0;i<h.mships.size();i++)
                    if (h.mships[i].loyalty_to>max_loyalty)
                        {ml_id=h.mships[i].id; max_loyalty=h.mships[i].loyalty_to;}
                return (ml_id==gi);})*n.map.size();
        intfrac[i]=(int)(frac[i]);
        filled_frac+=intfrac[i];
    }
    int filled=n.map.size();
    for (int i=0;i<ngroups;i++)
        if (filled_frac<filled)
            if(frac[i]-intfrac[i]>0.5) {intfrac[i]++; filled_frac++;}
    for (int i=0;i<ngroups;i++)
        if (filled_frac<filled)
            {intfrac[i]++; filled_frac++;}
    // Comment if group too small for representation
    for (int i=0;i<ngroups;i++)
        if(intfrac[i]==0){
            printf("\nToo small to appear: %s",gnames[p.groups[extant_groups[i]].name].c_str());
        }

    // Fill rest (for those with no loyalty)
    bool nl_group=false;
    if (filled_frac<filled){
        nl_group=true;
        ngroups+=1;
        intfrac.push_back(0);
        for (int i=filled_frac;i<filled;i++){
            intfrac[ngroups-1]++;
        }
    }

    string abbrev;
    // Fill map
    int half_ncol = n.ncol/2;
    char letts[4] = " A ";
    int maplr[2];
    maplr[0]=0;maplr[1]=n.ncol-1;
    bool is_full[2];
    is_full[0]=false; is_full[1]=false;
    for (int i=0;i<ngroups;i++){
        if (i==ngroups-1 && nl_group){
            abbrev="NLo";
        }else{
            abbrev=gnames[p.groups[extant_groups[i]].name].substr(0,3);
        }
        strcpy(letts,abbrev.c_str());
        int l_or_r=(i%2==0 ? 0 : 1);
        if (l_or_r==0 && is_full[0]) l_or_r=1;
        if (l_or_r==1 && is_full[1]) l_or_r=0;
        bool reverse=false; int r_ind;
        for (int j=0;j<intfrac[i];j++){
            if (!reverse){
                if (j==intfrac[i]/2) strcpy(n.map[maplr[l_or_r]].letter,letts);
                else strcpy(n.map[maplr[l_or_r]].letter,"   ");
                n.map[maplr[l_or_r]].owner=i;
                if(l_or_r==0){
                    maplr[0]++;
                    if (maplr[0]%half_ncol==0) maplr[0]+=half_ncol;
                    if (maplr[0]>n.map.size()-1) {is_full[0]=true; l_or_r=1; reverse=true; r_ind=n.map.size()-half_ncol;}
                } else {
                    maplr[1]--;
                    if (maplr[1]%half_ncol==half_ncol-1) maplr[1]+=half_ncol*3;
                    if (maplr[1]>n.map.size()-1) {is_full[1]=true; l_or_r=0; reverse=true; r_ind=n.map.size()-half_ncol-1;}
                }
            } else { // Handle case where column is filled and rest spills over
                if (j==intfrac[i]/2) strcpy(n.map[r_ind].letter,letts);
                else strcpy(n.map[r_ind].letter,"   ");
                n.map[r_ind].owner=i;
                if(l_or_r==0){
                    r_ind--;
                    if (r_ind%half_ncol==half_ncol-1) r_ind-=half_ncol;
                } else {
                    r_ind++;
                    if (r_ind%half_ncol==0) r_ind-=half_ncol*3;
                }
            }
        }
    }

    // Print map
    print_map(n);
}

void map_by_population(Population &p, Nature &n, int group_focus=-1){
    // Determine # ppl/tile
    vector<int> tilepop(n.map.size(),0);
    for (int i=0;i<p.person.size();i++){
        if (group_focus==-1){
            tilepop[p.person[i].home]+=1;
        } else { // Count only members of group_focus
            if (p.person[i].is_member(group_focus)) tilepop[p.person[i].home]+=1;
        }
    }
    // Get max population
    int maxpop=0;
    for (int i=0;i<tilepop.size();i++){
        if (tilepop[i]>maxpop) maxpop=tilepop[i];
    }
    string letts;
    // Now fill map
    for (int i=0;i<n.map.size();i++){
        int lpop=tilepop[i];
        if (lpop>999) lpop=999; // Cap at 999
        letts=" ";
        if (lpop>99) letts="";
        letts+=to_string(lpop);
        if (lpop<=9) letts+=" ";
        int gid=(4*lpop)/maxpop; // Creates four density contours
        if (group_focus==-1){ // Keep details if doing group_focus
            if (gid==0) letts="   ";
            if (gid==1) letts=" + ";
            if (gid==2) letts="+ +";
            if (gid>=3) letts="+++";
        }

        if (lpop==0){
            if (n.map[i].terrain==GRASS){
                letts="  '";
                gid=-2;
            }else if (n.map[i].terrain==WATER){ // water
                letts=" ~~";
                gid=-3;
            }else{ // mountain
                letts=" ^^";
                gid=-4;
            }
        }
        strcpy(n.map[i].letter,letts.c_str());
        n.map[i].owner=gid;
    }

    // Print map
    print_map(n);
}



tuple<vector<int>,vector<int>> get_geogroup(Population &p, Nature &n, int itile){
        // Determine dominant group in each tile 
        vector<int> groups;
        vector<int> group_pop;
        for(int ires = 0; ires<n.map[itile].residents.size();ires++){
            // Loop over residents in tile, determine their top loyalty
            int res_id=n.map[itile].residents[ires];
            float max_loyalty=0.0;
            int ml_id=-1;
            for (int i=0;i<p.person[res_id].mships.size();i++){
                if (p.person[res_id].mships[i].loyalty_to>max_loyalty) {
                    ml_id=p.person[res_id].mships[i].id;
                    max_loyalty=p.person[res_id].mships[i].loyalty_to;
                }
            }
            int this_id;
            if (ml_id>=0){
                this_id=ml_id;
            } else {
                this_id=-1;
            }

            // Check to see if group already listed
            int which_group=-1;
            for (int i=0;i<groups.size();i++)
                if (groups[i]==this_id)
                    {which_group=i;break;}
            if (which_group==-1){
                which_group=groups.size();
                groups.push_back(this_id);
                group_pop.push_back(0);
            }

            // Count resident towards this group
            group_pop[which_group]+=1;
        }
        return make_tuple(groups,group_pop);
    }

void map_by_geogroup(Population &p, Nature &n, int mark_spot=-1){
    for (int itile=0;itile<n.map.size();itile++){
        // Determine dominant group in each tile 
        vector<int> groups;
        vector<int> group_pop;
        tie(groups,group_pop)=get_geogroup(p,n,itile);
        // Finally, determine which group is dominant in this tile
        int max_nres=0;
        int gid=-2;
        for (int i = 0; i<groups.size();i++){
            if (group_pop[i]>max_nres) {
                max_nres=group_pop[i];
                gid=groups[i];
            }
        }

        string abbrev;
        if (gid==-1){ // If most have no loyalty
            abbrev = "NLo";
            gid=-3; // since -1 is used for the map edge
        } else if (gid==-2){ // If no people
            if (n.map[itile].terrain==GRASS){
                abbrev="  '";
            }else if (n.map[itile].terrain==WATER){ // water
                abbrev=" ~~";
                gid=-4;
            }else{ // mountain
                abbrev=" ^^";
            }
        } else if (gid>=0) { // If group gid has most loyalty
            abbrev=gnames[p.groups[gid].name].substr(0,3);
        }
        if (gid>=0 || gid==-3){
            // Make map cleaner: don't print name if adjacent tile is same group
            for (int i_neigh=0;i_neigh<3;i_neigh++){
                Direction direction;
                if (i_neigh==0) direction=W;
                if (i_neigh==1) direction=NW;
                if (i_neigh==2) direction=NE;
                int neighbor_tile = n.neighbor(itile,direction);
                if (neighbor_tile>=0 && n.map[neighbor_tile].owner==gid) abbrev="   "; // Make a little cleaner
            }
        }
        
        strcpy(n.map[itile].letter,abbrev.c_str());
        n.map[itile].owner=gid;
    }

    // Add * to mark a location if sent
    if (mark_spot>=0){
        strcpy(n.map[mark_spot].letter," * ");
    }

    // Print map
    print_map(n);
}

void map_resources(Nature &n){
    string letts;
    // Now fill map
    for (int i=0;i<n.map.size();i++){
        int lpop=n.map[i].food_min;
        if (lpop>999) lpop=999; // Cap at 999
        letts=" ";
        if (lpop>99) letts="";
        letts+=to_string(lpop);
        if (lpop<=9) letts+=" ";
        if (lpop==0) letts="   ";
        strcpy(n.map[i].letter,letts.c_str());
        n.map[i].owner=i;
    }

    // Print map
    print_map(n);
}

void print_histogram(vector<float> fracs){
    int hist_height=10;
    vector<int> heights(fracs.size());
    float maxheight=0.0f;
    for (int i=0;i<fracs.size();i++){
        if (fracs[i]>maxheight) maxheight=fracs[i];
    }
    for (int i=0;i<fracs.size();i++){
        heights[i]=(fracs[i]/maxheight) * hist_height;
    }
    for (int i=hist_height;i>0;i--){
        printf("\n ");
        for (int j=0;j<fracs.size();j++){
            if (heights[j]>i || (j>0 && heights[j-1]>i)){
                printf("|");
            } else if (i==1){
                printf("_");
            } else {
                printf(" ");
            }
            if (heights[j]==i || i==1){
                printf("_");
            } else {
                printf(" ");
            }
        }
        if (heights[fracs.size()-1]>i){
            printf("|");
        }
    }
}

void histograms(Population& p){
    // Extroversion
    vector<float> fracs(33);
    for (int i=0;i<fracs.size();i++){
        fracs[i] = p.frac(i,[](int i,Person& h){return h.extroversion==i;});
    }
    printf("\nExtroversion: (average: %.1f)", p.avg([](Person& h){return h.extroversion;}));
    print_histogram(fracs);

    // Agreeableness
    for (int i=0;i<fracs.size();i++){
        fracs[i] = p.frac(i,[](int i,Person& h){return h.agreeableness==i;});
    }
    printf("\nAgreeableness: (average: %.1f)", p.avg([](Person& h){return h.agreeableness;}));
    print_histogram(fracs);

    // Conscientiousness
    for (int i=0;i<fracs.size();i++){
        fracs[i] = p.frac(i,[](int i,Person& h){return h.conscientiousness==i;});
    }
    printf("\nConscientiousness (average: %.1f)", p.avg([](Person& h){return h.conscientiousness;}));
    print_histogram(fracs);

    // Neuroticism
    for (int i=0;i<fracs.size();i++){
        fracs[i] = p.frac(i,[](int i,Person& h){return h.neuroticism==i;});
    }
    printf("\nNeuroticism: (average: %.1f)", p.avg([](Person& h){return h.neuroticism;}));
    print_histogram(fracs);

    // Openness
    for (int i=0;i<fracs.size();i++){
        fracs[i] = p.frac(i,[](int i,Person& h){return h.openness==i;});
    }
    printf("\nOpenness: (average: %.1f)", p.avg([](Person& h){return h.openness;}));
    print_histogram(fracs);
    fflush(stdout);
}

#endif
