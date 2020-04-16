#ifndef NATURE_HPP
#define NATURE_HPP
#include <vector>
#include <string.h>
#include "random.hpp"

struct HexTile{
    int id;
    float food_min;
    float food_max;
    float food_available;
    float defense;
    int owner;
    char letter[4]="";

    std::vector<int> residents;

    int neighbor (int ncol, int nrow, int w){
        int row = id/ncol;
        int col = id-row*ncol;
        bool l = (row%2==0);
        int shift = (row+1)%2;
        if (w==0) return (col==0)                             ? -1 : id-1; // West
        if (w==1) return (row==0 || (l && col==0))            ? -1 : id-ncol-shift; // Northwest
        if (w==2) return (row==0 || (!l && col==ncol-1))      ? -1 : id-ncol+1-shift; // Northeast
        if (w==3) return (col==ncol-1)                        ? -1 : id+1; // East
        if (w==4) return (row==nrow-1 || (!l && col==ncol-1)) ? -1 : id+ncol+1-shift; // Southeast
        if (w==5) return (row==nrow-1 || (l && col==0))       ? -1 : id+ncol-shift; // Southwest
        return -1;
    }
};

class Nature{
    float min_food_gen;
    float max_food_gen;

    public:
    float food_available;

    std::vector<HexTile> map;
    int ncol;
    int nrow;

    Nature(float min_food_gen, float max_food_gen, int climate_type, int mapsize, int mapwidth) : max_food_gen(max_food_gen), min_food_gen(min_food_gen), map(mapsize) {
        for (int i=0;i<map.size();i++){
            strcpy(map[i].letter, "   ");
            map[i].id=i;
        }
        ncol=mapwidth;
        nrow=mapsize/mapwidth;
        if(nrow*ncol!=mapsize) printf("ERROR: mapsize must be divisible by mapwidth");

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
                float sunniness;
                int row = i/ncol;
                bool evenshift = (nrow%2==0) ? 1 : 0;
                int latitude = row-nrow/2;
                if (latitude<0) latitude=-latitude-evenshift;
                sunniness = nrow/2-latitude-evenshift;
                map[i].food_min=sunniness;
                map[i].food_max=sunniness;
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

    int neighbor (int ind, int w){
        return map[ind].neighbor(ncol,nrow,w);
    }
};
#endif
