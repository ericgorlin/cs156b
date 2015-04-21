#include "ALS.h"
#include "LoadData.h"

ALS::ALS(int lf, double lambda_val)
{
    // Set the number of latent factors, users, and movies
    latent_factors = lf;
    n_users = 458293;
    n_movies = 17770;
    lambda = lambda_val;

    // Create the U and V matrices based on these parameters
    // Randomly initialize all values
    u = arma::mat(n_users, latent_factors).randn();
    v = arma::mat(n_movies, latent_factors).randn();

    y = LoadData::start();
}

ALS::~ALS()
{
    //dtor
}

// Runs alternating least squares to create the U and V matrices
// wij = 1 if user i rated movie j and 0 otherwise
void ALS::run_als()
{
    int w = 0;
    arma::mat new_u(n_users, latent_factors);
    arma::mat new_v(n_movies, latent_factors);
    // Number of iterations
    for (unsigned int k = 0; k < 20; ++k) {
        // Go through all the rows of the matrix
        for (unsigned int i = 0; i < n_users; ++i) {
            // First part of sum
            arma::mat iden(latent_factors, latent_factors);
            iden.eye();
            iden = lambda * iden;

            // Second part of sum
            arma::mat sum1(latent_factors, latent_factors);
            sum1.zeros();

            // Second term
            arma::mat sum2(1, latent_factors);
            sum2.zeros();

            for (unsigned int j = 0; j < n_movies; ++j) {
                // Find the value of w--1 if the the user rated
                // the movie and 0 if he/she didn't
                if (y(i,j) == 0)
                    w = 0;
                else
                    w = 1;
                sum1 += w * v.col(j) * trans(v.col(j));

                sum2 += w * y(i,j) * v.col(j);
            }

            new_u.row(i) = (iden + sum1).i() * sum2;
        }

        // Go through all the columns of the matrix
        for (unsigned int j = 0; j < n_movies; ++j) {
            // First part of sum
            arma::mat iden(latent_factors, latent_factors);
            iden.eye();
            iden = lambda * iden;

            // Second part of sum
            arma::mat sum1(latent_factors, latent_factors);
            sum1.zeros();

            // Second term
            arma::mat sum2(latent_factors, 1);
            sum2.zeros();

            for (unsigned int i = 0; i < n_users; ++i) {
                // Find the value of w--1 if the the user rated
                // the movie and 0 if he/she didn't
                if (y(i,j) == 0)
                    w = 0;
                else
                    w = 1;
                sum1 += w * trans(u.row(i)) * u.row(i);

                sum2 += w * y(i,j) * u.row(i);
            }

            new_v.col(j) = (iden + sum1).i() * sum2;
        }

        // Switch u, new_u, v, and new_v
        u = new_u;
        v = new_v;

    }
}

int main() {}


