#ifndef SGD_H
#define SGD_H
#include <armadillo>
class LoadData2;

class SGD2
{
    public:
        SGD2(int lf, double lambda_val, double lr);
        virtual ~SGD2();
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
        double **y;
        double **probe;
        double *user_avg;
        double *movie_avg;

        void create_file();
};

#endif // SGD_H

