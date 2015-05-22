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

class LoadData3
{
public:
    LoadData3();
    virtual ~LoadData3();
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
    double *getUserAvgDate();
    set<int> *getMoviesPerUser();
    set<int> *getDatesPerUser();
    std::vector<double> getNorms();
    int getMinDate();
    int getMaxDate();


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
    set<int> *dates_per_user;
    double *userAvgDate;
    int totalMovies;
    double sumRatings;
    int minDate;
    int maxDate;
};

#endif // LOADDATA_H

