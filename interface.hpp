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
