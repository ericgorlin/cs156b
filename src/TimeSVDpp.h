#ifndef SVD_H
#define SVD_H
#include <set>
#include <vector>
#include <string>


class LoadData3;

class TimeSVDpp
{
public:
    TimeSVDpp(int lf, double lruser, double lrmovie, double lralpha, double lruserbias, double lrmoviebias, double lruserbins, double lrmoviebins, double lambdauser, double lambdamovie, double lambdaalpha, double lambday, double lambdauserbias, double lambdamoviebias, double lambdauserbins, double lambdamoviebins);
    virtual ~TimeSVDpp();
    void run_sgd();
    double find_error(int epoch);
    unsigned int n_datapoints;
    std::string outfile;
    std::string outfileProbe;
private:
    int latentFactors;
    int n_users;
    int n_movies;
    double lr;
    double lambdaY;
    double lrUser;
    double lrMovie;
    double lrAlpha;
    double lrUserBias;
    double lrMovieBias;
    double lrUserBins;
    double lrMovieBins;
    double lambdaUser;
    double lambdaMovie;
    double lambdaAlpha;
    double lambdaUserBias;
    double lambdaMovieBias;
    double lambdaUserBins;
    double lambdaMovieBins;
    double global_mean;
    std::vector<double> user_vec;
    std::vector<double> movie_vec;

    LoadData3 *l;
    double **u;
    double **v;
    double **prev_u;
    double **prev_v;
    double **data;
    double **probe;
    double *user_avg;
    double *movie_avg;
    double **y;
    double **sumY;
    double **alphat;
    double *alpha;
    //double *currAlpha;
    int *dateRange;
    std::set<int> *movies_per_user;
    std::set<int> *users_per_movie;
    std::vector<double> norms;
    double *userAvgDate;
    double **movie_bins;
    double **user_bins;
    double ***movie_bias_bins;
    double ***user_bias_bins;
    double estimateRating(int user, int movie, int date);
    int dateToBin(int date);
    int dateToUserBin(int date);
    int nbins;
    int nuserbins;
    int minDate;
    int maxDate;
    int UserRange(int date);


    void create_file();
    void create_probe_file();
};

#endif // SVD_H

