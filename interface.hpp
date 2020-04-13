#ifndef INTERFACE_HPP
#define INTERFACE_HPP
#include <stdio.h>
#include <vector>

template<typename T>
struct SimVar{
    std::vector<T> var;

    SimVar(int n) : var(n) {}

    void add(int i, T &newvar){
        var[i]=newvar;
    }

    float avg(){
        float sum = 0;
        for (int i=0 ; i<var.size() ; i++) {
            sum += (float)(var[i]);
        }
        return sum/var.size();
    }

    float eq_avg(){
        float sum = 0;
        int i_start = var.size()*.9; // Look only at the last 10% of time
        for (int i=i_start ; i<var.size() ; i++) {
            sum += (float)(var[i]);
        }
        return sum/(var.size()-i_start);
    }

    void write(const char* filename){
        FILE * pFile;
        pFile = fopen (filename,"w");
        for (int i=0 ; i<var.size() ; i++)
        {
            fprintf (pFile, "%d\n",var[i]);
        }
        fclose (pFile);
    }
};


#endif
