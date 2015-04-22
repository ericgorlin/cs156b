#ifndef ALS_H
#define ALS_H
#include <armadillo>


class ALS
{
    public:
        ALS(int lf, double lambda_val);
        virtual ~ALS();
        void run_als();
        double find_error(arma::mat &u, arma::mat &v);
    private:
        int latent_factors;
        int n_users;
        int n_movies;
        double lambda;
        arma::mat u;
        arma::mat v;
        arma::sp_mat y;
        arma::mat probe;
};

#endif // ALS_H
