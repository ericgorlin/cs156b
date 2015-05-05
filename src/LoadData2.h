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

class LoadData2
{
    public:
        LoadData2();
        virtual ~LoadData2();
        double **loadRatingsVector();
        static double **probe();
        static double **qual();
        arma::sp_mat loadSparseData();
        double getUserMean(int userIdx);
        //KNN();
        //KNN(arma::umat m);
        //arma::vec normalize(unsigned int user);
        //virtual ~KNN();
        //arma::umat getKNN();
        double getUserStddev(int userIdx);
        double getMovieMean(int movieIdx);
        double getMovieStddev(int movieIdx);
        double getGlobalMean();
        std::vector<double> getBetterUserMean();
        std::vector<double> getBetterMovieMean();
        std::vector<unsigned int> getUserRatings(unsigned int user);
        std::vector<unsigned int> getMovieRatings(unsigned int movie);

    protected:
    private:
        unordered_map<int, vector<double>> userMap;
        unordered_map<int, vector<double>> movieMap;
        unordered_map<int, vector<unsigned int>> userRatings;
        unordered_map<int, vector<unsigned int>> movieRatings;
        double **data;
        arma::sp_mat sp_data;
        int totalMovies;
        int sumRatings;
};

#endif // LOADDATA_H

