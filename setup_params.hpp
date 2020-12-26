#ifndef SETUP_PARAMS_HPP
#define SETUP_PARAMS_HPP

struct SetupParameters{
    int initial_n_ppl = 10000;
    int n_years = 2000;
    float min_food_gen = 10000;
    float max_food_gen = 10000;
    int climate_type = 1;
    int mapsize = 1;//15*18;
    int mapwidth = 1;//18;
};

#endif
