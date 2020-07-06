#ifndef NATURE_HPP
#define NATURE_HPP
#include <vector>
#include <string.h>
#include "random.hpp"

using std::vector;

#define GRASS 0
#define WATER 1
#define MOUNTAIN 2

enum Direction { W, NW, NE, E, SE, SW };

Direction random_dir(){
    int intdir = rand_int(6);
    if(intdir==0) return W;
    if(intdir==1) return NW;
    if(intdir==2) return NE;
    if(intdir==3) return E;
    if(intdir==4) return SE;
    if(intdir==5) return SW;
    return W;
}

struct HexTile{
    int id;
    int terrain;
    float food_min;
    float food_max;
    float food_available;
    float defense;
    int owner;
    char letter[4]="";

    vector<int> residents;

    HexTile() : residents(0,0) {}

    int neighbor (int ncol, int nrow, Direction w){
        int row = id/ncol;
        int col = id-row*ncol;
        bool l = (row%2==0);
        int shift = (row+1)%2;
        if (w==W ) return (col==0)                             ? -1 : id-1; // West
        if (w==NW) return (row==0 || (l && col==0))            ? -1 : id-ncol-shift; // Northwest
        if (w==NE) return (row==0 || (!l && col==ncol-1))      ? -1 : id-ncol+1-shift; // Northeast
        if (w==E ) return (col==ncol-1)                        ? -1 : id+1; // East
        if (w==SE) return (row==nrow-1 || (!l && col==ncol-1)) ? -1 : id+ncol+1-shift; // Southeast
        if (w==SW) return (row==nrow-1 || (l && col==0))       ? -1 : id+ncol-shift; // Southwest
        return -1;
    }
};

class Nature{
    float min_food_gen;
    float max_food_gen;

    public:
    float food_available;

    vector<HexTile> map;
    int ncol;
    int nrow;

    Nature(float min_food_gen, float max_food_gen, int climate_type, int mapsize, int mapwidth) : max_food_gen(max_food_gen), min_food_gen(min_food_gen), map(mapsize) {
        for (int i=0;i<map.size();i++){
            strcpy(map[i].letter, "   ");
            map[i].id=i;
            map[i].terrain=GRASS;
            map[i].owner=-1;
        }
        ncol=mapwidth;
        nrow=mapsize/mapwidth;
        if(nrow*ncol!=mapsize) printf("ERROR: mapsize must be divisible by mapwidth");

        // Add water
        int nwater=map.size()/5;
        int iwater=0;
        int wtile=map.size()-1;
        while (iwater<nwater){
            if (map[wtile].terrain!=WATER) iwater++;
            map[wtile].terrain=WATER;
            int cand=-1;
            while (cand==-1)
                cand = map[wtile].neighbor(ncol,nrow,random_dir());
            wtile = cand;
        }

        // Add mountains
        int nmountains=map.size()/8;
        int imountains=0;
        int mtile=rand_int(map.size());
        while (imountains<nmountains){
            if (map[mtile].terrain==GRASS){
                imountains++;
                map[mtile].terrain=MOUNTAIN;
            }
            int cand=-1;
            while (cand==-1)
                cand = map[mtile].neighbor(ncol,nrow,random_dir());
            mtile = cand;
        }


        // "CLIMATE": Available food distribution
        // OPTION 1: Evenly distribute food among tiles
        if (climate_type==0){
            float tile_food_min = min_food_gen/map.size();
            float tile_food_max = max_food_gen/map.size();
            for (int i=0;i<map.size();i++){
                map[i].food_min=tile_food_min;
                map[i].food_max=tile_food_max;
            }
        }else if (climate_type==1){
            // OPTION 2: Icy poles have less food
            for (int i=0;i<map.size();i++){
                if (map[i].terrain==WATER){ // water has no food
                    map[i].food_min=0;
                    map[i].food_max=0;
                }else if (map[i].terrain==MOUNTAIN){ // mountain has no food
                    map[i].food_min=0;
                    map[i].food_max=0;
                }else{
                    float sunniness;
                    int row = i/ncol;
                    bool evenshift = (nrow%2==0) ? 1 : 0;
                    int latitude = row-nrow/2;
                    if (latitude<0) latitude=-latitude-evenshift;
                    sunniness = nrow/2-latitude-evenshift;
                    map[i].food_min=sunniness;
                    map[i].food_max=sunniness;
                }
            }
            // Normalize:
            float tfoodcount = 0.0f;
            for (int i=0;i<map.size();i++){
                tfoodcount+=map[i].food_min;
            }
            for (int i=0;i<map.size();i++){
                map[i].food_min *= min_food_gen/tfoodcount;
                map[i].food_max *= max_food_gen/tfoodcount;
            }
        }
    }

    void generate_food(){
        food_available=0.0f;
        for (int i=0;i<map.size();i++){
            float tile_min = map[i].food_min;
            float tile_max = map[i].food_max;
            map[i].food_available = tile_min + rand_f1()*(tile_max-tile_min);
            food_available+=map[i].food_available; // Track global available food
        }
    }

    int neighbor (int ind, Direction w){
        return map[ind].neighbor(ncol,nrow,w);
    }
};
#endif
