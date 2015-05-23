#ifndef SVD_H
#define SVD_H
#include <set>
#include <vector>
#include <string>

class LoadData3;

class TimeSVDpp
{
public:
    TimeSVDpp(int lf, double lruser, double lrmovie, double lralpha, double lruserbias, double lrmoviebias, double lambdauser, double lambdamovie, double lambdaalpha, double lambday, double lambdauserbias, double lambdamoviebias);
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
    double lambdaUser;
    double lambdaMovie;
    double lambdaAlpha;
    double lambdaUserBias;
    double lambdaMovieBias;
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
    double **alpha;
    double *currAlpha;
    std::set<int> *movies_per_user;
    std::vector<double> norms;
    double *userAvgDate;
    
    double estimateRating(int user, int movie, int date);
    
    
    void create_file();
    void create_probe_file();
};

#endif // SVD_H

