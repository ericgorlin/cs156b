#ifndef LOADDATA_H
#define LOADDATA_H
#include <iostream>
#include <fstream>
#include <ctime>
#define ARMA_64BIT_WORD
#include <armadillo>
#include <unordered_map>
#include <math.h>

using namespace std;

class LoadData
{
    public:
        LoadData();
        virtual ~LoadData();
        static arma::sp_mat start();
        static arma::umat loadRatingsVector();
        static arma::umat probe();
        static arma::sp_mat sparseFromMat(arma::umat y);
        int getUserMean(int userIdx);
        double getUserStddev(int userIdx);
        int getMovieMean(int movieIdx);
        double getMovieStddev(int movieIdx);

    protected:
    private:
        static unordered_map<int, vector<int>> userMap;
        static unordered_map<int, vector<int>> movieMap;
};

#endif // LOADDATA_H
