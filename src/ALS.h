#ifndef ALS_H
#define ALS_H
#include <armadillo>


class ALS
{
    public:
        ALS(int lf, double lambda_val);
        virtual ~ALS();
        void run_als();
    protected:
    private:
        int latent_factors;
        int n_users;
        int n_movies;
        double lambda;
        arma::mat u;
        arma::mat v;
        arma::sp_mat y;
};

#endif // ALS_H
