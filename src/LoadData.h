#ifndef LOADDATA_H
#define LOADDATA_H
#include <iostream>
#include <fstream>
#include <ctime>
#define ARMA_64BIT_WORD
#include <armadillo>

using namespace std;

class LoadData
{
    public:
        LoadData();
        virtual ~LoadData();
        static arma::sp_mat start();

    protected:
    private:
};

#endif // LOADDATA_H
