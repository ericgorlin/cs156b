#ifndef SGD_H
#define SGD_H
#include <armadillo>


class SGD
{
    public:
        SGD(int lf, double lambda_val, double lr);
        virtual ~SGD();
        void run_sgd();
        double find_error(arma::mat &u, arma::mat &v);
    private:
        int latent_factors;
        int n_users;
        int n_movies;
        double lambda;
        double learn_rate;

        arma::mat u;
        arma::mat v;
        arma::umat y;
        arma::umat probe;
        arma::vec userAvg;
        arma::vec movieAvg;
};

#endif // SGD_H
