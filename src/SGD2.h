#ifndef SGD_H
#define SGD_H
#include <set>
#include <vector>

class LoadData2;

class SGD2
{
    public:
        SGD2(int lf, double lambda_val, double lr, double lambda2, double lr2);
        virtual ~SGD2();
        double run_sgd();
        double find_error(int epoch);
    private:
        int latent_factors;
        int n_users;
        int n_movies;
        double lambda;
        double learn_rate;
        double global_mean;
        double lambda_avg;
        double learn_rate_avg;
        std::vector<double> user_vec;
        std::vector<double> movie_vec;

        LoadData2 *l;
        double **u;
        double **v;
        double **y;
        double **probe;
        double *user_avg;
        double *movie_avg;
        double *implicit;
        double *implicit_sum;
        std::set<int> *movies_per_user;
        std::vector<double> norms;
        double estimateRating(int user, int movie);


        void create_file();
};

#endif // SGD_H

