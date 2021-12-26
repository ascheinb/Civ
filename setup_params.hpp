#ifndef SETUP_PARAMS_HPP
#define SETUP_PARAMS_HPP

struct SetupParameters{
    int initial_n_ppl = 12000;
    int n_years = 2000;
    float min_food_gen = 4000;
    float max_food_gen = 4000;
    int climate_type = 1;
    int mapsize = 1*3;//15*18*9;//15*18;
    int mapwidth = 1;//18*3;//18;
};

#endif
