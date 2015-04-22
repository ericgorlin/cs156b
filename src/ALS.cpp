#include "ALS.h"
#include "LoadData.h"
#include <ctime>

ALS::ALS(int lf, double lambda_val)
{
    // Set the number of latent factors, users, and movies
    latent_factors = lf;
    n_users = 458293;
    n_movies = 17770;
    //n_users = 5;
    //n_movies = 17754;
    lambda = lambda_val;

    // Create the U and V matrices based on these parameters
    // Randomly initialize all values
    u = arma::mat(latent_factors, n_users).randn();
    v = arma::mat(latent_factors, n_movies).randn();

    y = LoadData::start();
    probe = LoadData::probe();
}

ALS::~ALS()
{
    //dtor
}

// Runs alternating least squares to create the U and V matrices
// wij = 1 if user i rated movie j and 0 otherwise
void ALS::run_als()
{
    clock_t begin = 0;
    std::cout << "running als" << std::endl;
    double w = 0;
    double new_error = 0;
    double old_error = 1000000;
    arma::mat new_u(latent_factors, n_users);
    arma::mat new_v(latent_factors, n_movies);

    // Number of iterations
    for (unsigned int k = 0; k < 20; ++k) {
        // Go through all the rows of the matrix
        for (unsigned int i = 0; i < n_users; ++i) {
            if (i % 10000 == 0) {
                if (i != 0) {
                    clock_t end = clock();
                    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
                    cout << elapsed_secs << endl;
                }

                std::cout << i << std::endl;
                begin = clock();

            }
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

            for (unsigned int j = 0; j < n_movies; ++j) {
                // Find the value of w--1 if the the user rated
                // the movie and 0 if he/she didn't
                if (y.at(i,j) == 0)
                    w = 0;
                else {
                    w = 1;
                }


                sum1 += w * v.col(j) * trans(v.col(j));
                sum2 += w * y.at(i,j) * v.col(j);
            }


            //new_u.col(i) = (iden + sum1).i() * sum2;
            new_u.col(i) = inv(sum1.i()) * sum2;

        }
        std::cout << "u matrix done" << std::endl;

        // Go through all the columns of the matrix
        for (unsigned int j = 0; j < n_movies; ++j) {
            // First part of sum
            //arma::mat iden(latent_factors, latent_factors);
            //iden.eye();
            //iden = lambda * iden;

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
                sum1 += w * u.col(i) * trans(u.col(i));

                sum2 += w * y.at(i,j) * u.col(i);
            }

            //new_v.col(j) = (iden + sum1).i() * sum2;
            new_v.col(j) = (sum1.i()) * sum2;
        }
        std::cout << "v matrix done" << std::endl;

        // Find the error for the new values
        std::cout << "finding errors" << std::endl;
        new_error = find_error(new_u, new_v);
        std::cout << new_error << std::endl;

        // If there's no decrease in error, stop.
        if (new_error >= old_error)
            break;

        // Switch u, new_u, v, and new_v
        u = new_u;
        v = new_v;
    }
}

// Find the test error on the probe data set.
double ALS::find_error(arma::mat &u, arma::mat &v) {
    double error = 0;
    // Go through all the columns of probe
    // for (unsigned int i = 0; i < 16; ++i)
    for (unsigned int i = 0; i < 1374739; ++i)
    {
        int user = probe(0, i) - 1;
        int movie = probe(1, i) - 1;
        double actual = probe(2, i);

        arma::mat pred_matrix = trans(u.col(user)) * v.col(movie);
        std::cout << u.col(user) << std::endl;
        std::cout << v.col(movie) << std::endl;
        //std::cout << pred_matrix << std::endl;
        double predicted = pred_matrix(0,0);

        error += (actual - predicted) * (actual - predicted);
        //std::cout << actual << " " << predicted << std::endl;
    }

    // Scale by the number of reviews
    return error / 1374739;
    //return error / 16;

}
int main() {
    ALS als(5, .1);
    als.run_als();
}


