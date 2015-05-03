#ifndef SGD_H
#define SGD_H
#include <set>
#include <vector>
//#include <armadillo>

class LoadData2;

class SGDpp
{
    public:
        SGDpp(int lf, double lambda_val, double lr);
        virtual ~SGDpp();
        void run_sgd();
        double find_error(int epoch);
    private:
        int latent_factors;
        int n_users;
        int n_movies;
        double lambda;
        double learn_rate;
        double global_mean;
        std::vector<double> user_vec;
        std::vector<double> movie_vec;

        LoadData2 *l;
        double **u;
        double **v;
        double **data;
        double **probe;
        double *user_avg;
        double *movie_avg;
        double **y;
        double **sumY;
        std::set<int> *movies_per_user;
        std::vector<double> norms;
        double estimateRating(int user, int movie);


        void create_file();
};

#endif // SGD_H

