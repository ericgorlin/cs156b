#ifndef SGD_H
#define SGD_H
#include <armadillo>


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

        double **u;
        double **v;
        double **y;
        double **probe;

        void create_file();
};

#endif // SGD_H

