#ifndef NATURE_HPP
#define NATURE_HPP
#include <vector>
#include <string.h>
#include "random.hpp"
#include "population.hpp"

struct HexTile{
    int id;
    float food_min;
    float food_max;
    float food_available;
    float defense;
    int owner;
    char letter[2]="";

    int neighbor (int ncol, int nrow, int w){
        int row = id/ncol;
        int col = id-row*ncol;
        bool l = (row%2==0);
        int shift = (row+1)%2;
        if (w==0) return (col==0) ? -1 : id-1; // West
        if (w==1) return (row==0 || (l && col==0)) ? -1 : id-ncol-shift; // Northwest
        if (w==2) return (row==0 || (!l && col==ncol-1)) ? -1 : id-ncol+1-shift; // Northeast
        if (w==3) return (col==ncol-1) ? -1 : id+1; // East
        if (w==4) return (row==nrow-1 || (!l && col==ncol-1)) ? -1 : id+ncol+1-shift; // Southeast
        if (w==5) return (row==nrow-1 || (l && col==0)) ? -1 : id+ncol-shift; // Southwest
        return -1;
    }
};

class Nature{
    float min_food_gen;
    float max_food_gen;

    public:
    float food_available;

    std::vector<HexTile> map;

    Nature(float min_food_gen, float max_food_gen) : max_food_gen(max_food_gen), min_food_gen(min_food_gen), map(100) {
        for (int i=0;i<100;i++){
            strcpy(map[i].letter, " ");
            map[i].id=i;
        }
    }

    void generate_food(){
        food_available = min_food_gen+rand_f1()*(max_food_gen-min_food_gen);
    }

    void print_hexmap(){
        
        for (int i=0;i<10;i++){
            printf("\n");
            if (i%2==0){
                for (int j=0;j<10;j++) printf(" / \\");
                if (i>0) printf(" /");
                printf("\n");
                for (int j=0;j<10;j++) printf("| %s ",map[i*10+j].letter);
                printf("|\n");
                for (int j=0;j<10;j++) printf(" \\ /");
                printf(" \\");
            } else {
                printf("  ");
                for (int j=0;j<10;j++) printf("| %s ",map[i*10+j].letter);
                printf("|");
            }
        }
        printf("\n  ");
        for (int j=0;j<10;j++) printf(" \\ /");
    }

    const char* diag_border(int ind){
        int nw_ind = map[ind].neighbor(10, 10, 1);
        int ne_ind = map[ind].neighbor(10, 10, 2);
        int owner = map[ind].owner;
        int nw_owner = (nw_ind==-1 ? -1 : map[nw_ind].owner);
        int ne_owner = (ne_ind==-1 ? -1 : map[ne_ind].owner);
        int w_ind = map[ind].neighbor(10, 10, 0);
        int e_ind = map[ind].neighbor(10, 10, 3);
        int w_owner = (w_ind==-1 ? -1 : map[w_ind].owner);
        int e_owner = (e_ind==-1 ? -1 : map[e_ind].owner);
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

    const char* diag_border2(int ind){
        int sw_ind = map[ind].neighbor(10, 10, 5);
        int se_ind = map[ind].neighbor(10, 10, 4);
        int owner = map[ind].owner;
        int sw_owner = (sw_ind==-1 ? -1 : map[sw_ind].owner);
        int se_owner = (se_ind==-1 ? -1 : map[se_ind].owner);
        int w_ind = map[ind].neighbor(10, 10, 0);
        int e_ind = map[ind].neighbor(10, 10, 3);
        int w_owner = (w_ind==-1 ? -1 : map[w_ind].owner);
        int e_owner = (e_ind==-1 ? -1 : map[e_ind].owner);
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

    const char* vert_border(int ind){
        int w_ind = map[ind].neighbor(10, 10, 0);
        int owner = map[ind].owner;
        int w_owner = (w_ind==-1 ? -1 : map[w_ind].owner);
        if (owner==w_owner) return "  ";
        return "| ";
    }

    void print_map(){

        for (int i=0;i<10;i++){
            printf("\n");
            if (i%2==0){
                for (int j=0;j<10;j++) printf("%s",diag_border(i*10+j));
                if (i>0) printf(" /");
                printf("\n");
                for (int j=0;j<10;j++) printf("%s%s ",vert_border(i*10+j),map[i*10+j].letter);
                printf("|\n");
                for (int j=0;j<10;j++) printf("%s",diag_border2(i*10+j));
                printf(" \\");
            } else {
                printf("  ");
                for (int j=0;j<10;j++) printf("%s%s ",vert_border(i*10+j),map[i*10+j].letter);
                printf("|");
            }
        }
        printf("\n   \\");
        for (int j=0;j<9;j++) printf("--- ");
        printf("-/");
    }

    void map_by_population(Population &p){
        // Determine # tiles per group
        int ngroups=10;
        std::vector<float> frac(ngroups);
        std::vector<int> intfrac(ngroups);
        int filled_frac = 0;
        for (int i=0;i<ngroups;i++){
            frac[i]=p.frac(i,[](int i, Person& h){return h.mships[0].id==i;})*100;
            intfrac[i]=(int)(frac[i]);
            filled_frac+=intfrac[i];
        }
        int filled=100;
        for (int i=0;i<ngroups;i++)
            if (filled_frac<filled)
                if(frac[i]-intfrac[i]>0.5) {intfrac[i]++; filled_frac++;}
        for (int i=0;i<ngroups;i++)
            if (filled_frac<filled)
                {intfrac[i]++; filled_frac++;}

        // Fill map
        char letts[2] = "A";
        int maplr[2];
        maplr[0]=0;maplr[1]=9;
        bool is_full[2];
        is_full[0]=false; is_full[1]=false;
        for (int i=0;i<10;i++){
            int l_or_r=(i%2==0 ? 0 : 1);
            if (l_or_r==0 && is_full[0]) l_or_r=1;
            if (l_or_r==1 && is_full[1]) l_or_r=0;
            bool reverse=false; int r_ind;
            for (int j=0;j<intfrac[i];j++){
                if (!reverse){
                    if (j==intfrac[i]/2) strcpy(map[maplr[l_or_r]].letter,letts);
                    else strcpy(map[maplr[l_or_r]].letter," ");
                    map[maplr[l_or_r]].owner=i;
                    if(l_or_r==0){
                        maplr[0]++;
                        if (maplr[0]%5==0) maplr[0]+=5;
                        if (maplr[0]>99) {is_full[0]=true; l_or_r=1; reverse=true; r_ind=95;}
                    } else {
                        maplr[1]--;
                        if (maplr[1]%5==4) maplr[1]+=15;
                        if (maplr[1]>99) {is_full[1]=true; l_or_r=0; reverse=true; r_ind=94;}
                    }
                } else { // Handle case where column is filled and rest spills over
                    if (j==intfrac[i]/2) strcpy(map[r_ind].letter,letts);
                    else strcpy(map[r_ind].letter," ");
                    map[r_ind].owner=i;
                    if(l_or_r==0){
                        r_ind--;
                        if (r_ind%5==4) r_ind-=5;
                    } else {
                        r_ind++;
                        if (r_ind%5==0) r_ind-=15;
                    }
                }
            }
            letts[0]++;
        }

        // Print map
        print_map();
    }
};

#endif
