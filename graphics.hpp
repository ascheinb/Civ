#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP
#include <vector>
#include <string.h>
#include "random.hpp"
#include "nature.hpp"
#include "population.hpp"

const char* diag_border(int ind, Nature &n){
    int nw_ind = n.neighbor(ind,1);
    int ne_ind = n.neighbor(ind,2);
    int owner = n.map[ind].owner;
    int nw_owner = (nw_ind==-1 ? -1 : n.map[nw_ind].owner);
    int ne_owner = (ne_ind==-1 ? -1 : n.map[ne_ind].owner);
    int w_ind = n.neighbor(ind,0);
    int e_ind = n.neighbor(ind,3);
    int w_owner = (w_ind==-1 ? -1 : n.map[w_ind].owner);
    int e_owner = (e_ind==-1 ? -1 : n.map[e_ind].owner);
    if (owner==nw_owner && owner==ne_owner) return "    ";
    if (owner==nw_owner) return "   \\";
    if (owner==ne_owner) return " /  ";
    if (ne_owner==nw_owner && nw_owner==w_owner && ne_owner==e_owner) return " / \\";
    if (ne_owner==nw_owner && nw_owner==w_owner) return " / -";
    if (ne_owner==nw_owner && ne_owner==e_owner) return " - \\";
    if (ne_owner==nw_owner) return " ---";
    if (owner==w_owner && owner==e_owner) return " ---";
    if (owner==w_owner ) return " - \\";
    if (owner==e_owner ) return " / -";
    return " / \\";
}

const char* diag_border2(int ind, Nature &n){
    int sw_ind = n.neighbor(ind,5);
    int se_ind = n.neighbor(ind,4);
    int owner = n.map[ind].owner;
    int sw_owner = (sw_ind==-1 ? -1 : n.map[sw_ind].owner);
    int se_owner = (se_ind==-1 ? -1 : n.map[se_ind].owner);
    int w_ind = n.neighbor(ind,0);
    int e_ind = n.neighbor(ind,3);
    int w_owner = (w_ind==-1 ? -1 : n.map[w_ind].owner);
    int e_owner = (e_ind==-1 ? -1 : n.map[e_ind].owner);
    if (owner==sw_owner && owner==se_owner) return "    ";
    if (owner==se_owner) return " \\  ";
    if (owner==sw_owner) return "   /";
    if (se_owner==sw_owner && sw_owner==w_owner && se_owner==e_owner) return " \\ /";
    if (se_owner==sw_owner && sw_owner==w_owner) return " \\ -";
    if (se_owner==sw_owner && se_owner==e_owner) return " - /";
    if (se_owner==sw_owner) return " ---";
    if (owner==w_owner && owner==e_owner) return " ---";
    if (owner==w_owner ) return " - /";
    if (owner==e_owner ) return " \\ -";
    return " \\ /";
}

const char* vert_border(int ind, Nature &n){
    int w_ind = n.neighbor(ind,0);
    int owner = n.map[ind].owner;
    int w_owner = (w_ind==-1 ? -1 : n.map[w_ind].owner);
    if (owner==w_owner) return " ";
    return "|";
}

void print_map(Nature &n){
    for (int i=0;i<n.nrow;i++){
        printf("\n");
        if (i%2==0){
            for (int j=0;j<n.ncol;j++) printf("%s",diag_border(i*n.ncol+j,n));
            if (i>0) printf(" /");
            printf("\n");
            for (int j=0;j<n.ncol;j++) printf("%s%s",vert_border(i*n.ncol+j,n),n.map[i*n.ncol+j].letter);
            printf("|\n");
            for (int j=0;j<n.ncol;j++) printf("%s",diag_border2(i*n.ncol+j,n));
            if (n.nrow%2==0 || i<n.nrow-1) printf(" \\");
        } else {
            printf("  ");
            for (int j=0;j<n.ncol;j++) printf("%s%s",vert_border(i*n.ncol+j,n),n.map[i*n.ncol+j].letter);
            printf("|");
        }
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
    int ngroups=10;
    std::vector<float> frac(ngroups);
    std::vector<int> intfrac(ngroups);
    int filled_frac = 0;
    for (int i=0;i<ngroups;i++){
        frac[i]=p.frac(i,[](int i, Person& h){return h.mships[0].id==i;})*n.map.size();
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

    // Fill map
    int half_ncol = n.ncol/2;
    char letts[4] = " A ";
    int maplr[2];
    maplr[0]=0;maplr[1]=n.ncol-1;
    bool is_full[2];
    is_full[0]=false; is_full[1]=false;
    for (int i=0;i<ngroups;i++){
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
        letts[1]++;
    }

    // Print map
    print_map(n);
}

void map_by_population(Population &p, Nature &n){
    // Determine # ppl/tile
    std::vector<int> tilepop(n.map.size(),0);
    for (int i=0;i<p.person.size();i++){
        tilepop[p.person[i].home]+=1;
    }
    std::string letts;
    // Now fill map
    for (int i=0;i<n.map.size();i++){
        int lpop=tilepop[i];
        if (lpop>999) lpop=999; // Cap at 999
        letts=" ";
        if (lpop>99) letts="";
        letts+=std::to_string(lpop);
        if (lpop<=9) letts+=" ";
        if (lpop==0) letts="   ";
        strcpy(n.map[i].letter,letts.c_str());
        n.map[i].owner=i;
    }

    // Print map
    print_map(n);
}

void map_resources(Nature &n){
    std::string letts;
    // Now fill map
    for (int i=0;i<n.map.size();i++){
        int lpop=n.map[i].food_min;
        if (lpop>999) lpop=999; // Cap at 999
        letts=" ";
        if (lpop>99) letts="";
        letts+=std::to_string(lpop);
        if (lpop<=9) letts+=" ";
        if (lpop==0) letts="   ";
        strcpy(n.map[i].letter,letts.c_str());
        n.map[i].owner=i;
    }

    // Print map
    print_map(n);
}

#endif
