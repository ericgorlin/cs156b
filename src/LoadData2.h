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
        std::unordered_map<int, vector<double>> getUserMap();
        std::unordered_map<int, vector<double>> getMovieMap();
        //std::vector<unsigned int> getUserRatings(unsigned int user);
        //std::vector<unsigned int> getMovieRatings(unsigned int movie);

    protected:
    private:
        std::unordered_map<int, vector<double>> userMap;
        std::unordered_map<int, vector<double>> movieMap;
        //std::unordered_map<int, std::unordered_map<unsigned int, unsigned int>> usersOfMovies; // {movie1: {user1: rating1, ... }, movie2: {}, ...}
        //std::unordered_map<int, std::unordered_map<unsigned int, unsigned int>> moviesOfUsers; // {user1: {movie1: rating1, ... }, user2: {}, ...}
        std::unordered_map<int, std::vector<int>> usersOfMovies;
        std::unordered_map<int, std::vector<int>> moviesOfUsers;
        //std::vector<std::vector<unsigned int>> usersOfMovies;//(17770, std::vector<unsigned int>);
        //std::vector<std::vector<unsigned int>> moviesOfUsers;//(458293, std::vector<unsigned int>);
        double **data;
        arma::sp_mat sp_data;
        int totalMovies;
        int sumRatings;
};

#endif // LOADDATA_H

