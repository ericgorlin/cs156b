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

    probe = LoadData::probe();
    LoadData l = LoadData();
    y = l.loadRatingsVector();
    clock_t end = clock();
    double elapsed_min = double(end - begin) / CLOCKS_PER_SEC / 60;
    cout << elapsed_min << " minutes to get ratings" << endl;

    // Baseline averages
    userAvg = arma::vec(n_users).randn();
    movieAvg = arma::vec(n_movies).randn();
    //for (int i = 0; i < n_users; ++i)
    //    userAvg(i) = l.getUserMean(i);
    //for (int i = 0; i < n_movies; ++i)
    //    movieAvg(i) = l.getMovieMean(i);

    global_average = mean(y.row(2));
    cout << "mean: " << global_average << endl;


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
    arma::mat estimate_mat;
    arma::vec uUpdate;
    arma::vec vUpdate;

    for (unsigned int epoch = 1; epoch < 100; epoch++) {

        std::cout << "New epoch " << epoch << std::endl;

        //learn rate is original divided by epoch
        //lr = learn_rate / epoch;
        // Learning rate from funny paper
        lr = learn_rate;
        std::cout << lr << std::endl;
        shuffle(y);

        // Iterate through data points
        for (unsigned int i = 0; i < y.n_cols; i++) { //< y.n_cols
            rating = y.at(2, i);
            user = y.at(0, i) - 1;
            movie = y.at(1, i) - 1;
            estimate_mat = trans(u.col(user)) * v.col(movie);
            estimate = estimate_mat.at(0,0) + global_average + userAvg(user) + movieAvg(movie);


            error = (rating - estimate);
            arma::vec user_col = u.col(user);
            arma::vec movie_col = v.col(movie);
            uUpdate = lr * (error * movie_col - lambda * user_col);
            vUpdate = lr * (error * user_col - lambda * movie_col);

            u.col(user) += uUpdate;
            v.col(movie) += vUpdate;

            userAvg += lr * (error - lambda * userAvg(user));
            movieAvg += lr * (error - lambda * movieAvg(movie));


        }

        // Find the error for the new values
        new_error = find_error(u, v, epoch);

        // If there's no decrease in error, stop.
        std::cout << "Error: " << new_error << std::endl;
        std::cout << "Old error: " << old_error << std::endl;
        if (new_error >= old_error && epoch > 5) {
            u = prev_u;
            v = prev_v;
            break;
        }
        old_error = new_error;
    }

    create_file(u, v);
}

// Find the test error on the probe data set.
double SGD::find_error(arma::mat &u, arma::mat &v, int epoch) {
    double error = 0;
    // Go through all the columns of probe
    // for (unsigned int i = 0; i < 16; ++i)
    for (unsigned int i = 0; i < 1374739; ++i)
    {
        int user = probe.at(0, i) - 1;
        int movie = probe.at(1, i) - 1;
        double rating = probe.at(2, i);

        arma::mat pred_matrix = trans(u.col(user)) * v.col(movie);

        double predicted = pred_matrix.at(0,0) + global_average + userAvg(user) + movieAvg(movie);

        cout << predicted << " " << rating << endl;
        // Truncate the estimate to 1 and 5
        if (predicted < 1)
            predicted = 1;
        else if (predicted > 5)
            predicted = 5;

        error += (rating - predicted) * (rating - predicted);
    }

    // Scale by the number of reviews
    return error / 1374739;
    //return error / 16;

}

// Create an output file using the u and v matrices we found and the qual
// data
void SGD::create_file(arma::mat u, arma::mat v)
{
    ofstream myfile1;
    std::string s = "new_sgd_results.txt";
    myfile1.open(s);
    arma::mat qual = LoadData::qual();

    for (unsigned int i = 0; i < 2749898; ++i)
    {
        int user = qual.at(0, i) - 1;
        int movie = qual.at(1, i) - 1;

        arma::mat pred_matrix = trans(u.col(user)) * v.col(movie);
        double predicted = pred_matrix.at(0,0);


        // Truncate predictions
        if (predicted < 1)
            predicted = 1;
        else if (predicted > 5)
            predicted = 5;
        myfile1 << predicted << "\n";

    }
    myfile1.close();
}
/*
int main() {
    SGD sgd(30, 0.02, 0.001); // remember to have learning rate divided by number of epochs
    std::cout << "done loading\n";
    sgd.run_sgd();


}


*/
