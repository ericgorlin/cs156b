#ifndef LOADDATA_H
#define LOADDATA_H
#include <iostream>
#include <fstream>
#include <ctime>
#include <unordered_map>
#include <math.h>
#include <set>
#include <vector>

using namespace std;

class LoadData2
{
    public:
        LoadData2();
        virtual ~LoadData2();
        double **loadRatingsVector();
        static double **probe();
        static double **qual();
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
        set<int> *getMoviesPerUser();
        std::vector<double> getNorms();


        bool testingOnProbe;
        unsigned int n_users;
        unsigned int n_movies;
        unsigned int n_datapoints;

    protected:
    private:
        unordered_map<int, std::vector<double> > userMap;
        unordered_map<int, std::vector<double> > movieMap;
        double **data;
        set<int> *movies_per_user;
        int totalMovies;
        double sumRatings;
};

#endif // LOADDATA_H

