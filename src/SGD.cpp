#include "SGD.h"
#include "LoadData.h"
#include "Average.h"
#include <ctime>
#include <fstream>
#include <iostream>

SGD::SGD(int lf, double lambda_val, double lr)
{
    // Set the number of latent factors, users, and movies
    latent_factors = lf;
    learn_rate = lr;
    n_users = 458293;
    n_movies = 17770;
    //n_users = 5;
    //n_movies = 17754;
    lambda = lambda_val;

    // Create the U and V matrices based on these parameters
    // Randomly initialize all values
    u = arma::mat(latent_factors, n_users).randn();
    v = arma::mat(latent_factors, n_movies).randn();

    clock_t begin = 0;
    //y = LoadData::loadRatingsVector();

    probe = LoadData::probe();
    //y = probe;
    LoadData l = LoadData();
    y = l.loadRatingsVector();
    clock_t end = clock();
    double elapsed_min = double(end - begin) / CLOCKS_PER_SEC / 60;
    cout << elapsed_min << " minutes to get ratings" << endl;

    //LoadData::sparseFromMat(y);

    //userAvg = Average::getUserAverages();
    //movieAvg = Average::getMovieAverages();
}

SGD::~SGD()
{
    //dtor
}

// Runs alternating least squares to create the U and V matrices
// wij = 1 if user i rated movie j and 0 otherwise
void SGD::run_sgd()
{
    clock_t begin = 0;
    std::cout << "running SGD" << std::endl;

    double new_error = 0;
    double old_error = 1000000;
    arma::mat prev_u = u;
    arma::mat prev_v = v;

    double lr; // learn rate
    int user;
    int movie;
    int rating;
    double estimate;
    double error;
    arma::vec uUpdate;
    arma::vec vUpdate;

    for (unsigned int epoch = 1; epoch < 20; epoch++) {

        std::cout << "New epoch" << std::endl;

        //learn rate is original divided by epoch
        lr = learn_rate / epoch;
        std::cout << lr << std::endl;
        shuffle(y);

        // Iterate through data points
        for (unsigned int i = 0; i < y.n_cols; i++) { //< y.n_cols
//std::cout << "_____" << std::endl;
            rating = y(2, i);
            user = y(0, i) - 1;
            movie = y(1, i) - 1;
            //std::cout << user << " " << movie << std::endl;
//for (int z = 0; z < 5; z++) {
         //   estimate = p(userInd, :) * q(movieInd, :)' + a(userInd, :) + b(movieInd, :);
            estimate = arma::dot(u.col(user), v.col(movie));
         //   std::cout << estimate << std::endl;


            error = rating - estimate;
         //   std::cout << error << std::endl << std::endl;
//        error = rating - globalAvg - estimate;


            uUpdate = lr * (error * v.col(movie) - lambda * u.col(user));
            vUpdate = lr * (error * u.col(user) - lambda * v.col(movie));
//        aUpdate = learnRate * (error - lambda * a(userInd));
//        bUpdate = learnRate * (error - lambda * b(movieInd));
            u.col(user) += uUpdate;
            v.col(movie) += vUpdate;
//}
//        q(movieInd, :) = q(movieInd, :) + qUpdate;
//        a(userInd) = a(userInd) + aUpdate;
//        b(movieInd) = b(movieInd) + bUpdate;

//            if (i % 10000 == 0) {
//                if (i != 0) {
//                    clock_t end = clock();
//                    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
//                    cout << elapsed_secs << endl;
//                }
//
//                std::cout << i << std::endl;
//                begin = clock();

//            }

        }

        // Find the error for the new values
   //     std::cout << "Epoch complete, calculating error" << std::endl;
        new_error = find_error(u, v, epoch);
   //     std::cout << new_error << std::endl;

        // If there's no decrease in error, stop.
        std::cout << "Error: " << new_error << std::endl;
        std::cout << "Old error: " << old_error << std::endl;
        /*
        if (new_error >= old_error) {
            u = prev_u;
            v = prev_v;
            break;
        }
        */
    }
}

// Find the test error on the probe data set.
double SGD::find_error(arma::mat &u, arma::mat &v, int epoch) {
    ofstream myfile1;
    std::string a = std::to_string(epoch);
    std::string s = "sgd_results" + a + ".txt";
    std::cout << s << std::endl;
    myfile1.open(s);

    double error = 0;
    // Go through all the columns of probe
    // for (unsigned int i = 0; i < 16; ++i)
    for (unsigned int i = 0; i < 1374739; ++i)
    {
        int user = probe(0, i) - 1;
        int movie = probe(1, i) - 1;
        double rating = probe(2, i);

        arma::mat pred_matrix = trans(u.col(user)) * v.col(movie);
        std::cout << u.col(user) << std::endl;
        std::cout << v.col(movie) << std::endl;
 //       std::cout << u.col(user) << std::endl;
 //       std::cout << v.col(movie) << std::endl;
        //std::cout << pred_matrix << std::endl;
        double predicted = pred_matrix(0,0);

        if (predicted < 0)
            predicted = 0;
        else if (predicted > 5)
            predicted = 5;
        myfile1 << predicted << "\n";
        //std::cout << rating << " " << predicted << std::endl;

        error += (rating - predicted) * (rating - predicted);
        //std::cout << rating << " " << predicted << std::endl;
    }
    myfile1.close();

    // Scale by the number of reviews
    return error / 1374739;
    //return error / 16;

}
int main() {
    SGD sgd(30, 0.1, 0.1); // remember to have learning rate divided by number of epochs
    std::cout << "done loading\n";
    sgd.run_sgd();


}


