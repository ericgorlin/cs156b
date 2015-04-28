
#include "SGD2.h"
#include "LoadData2.h"
#include <ctime>
#include <fstream>
#include <iostream>

SGD2::SGD2(int lf, double lambda_val, double lr)
{
    // Set the number of latent factors, users, and movies
    latent_factors = lf;
    learn_rate = lr;
    n_users = 458293;
    n_movies = 17770;
    //n_users = 5;
    //n_movies = 17754;
    lambda = lambda_val;

    // Create a normal distribution to sample random numbers from
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> d(0,1);

    // Create the U and V matrices based on these parameters
    // Randomly initialize all values
    u = 0;
    u = new double*[n_users];
    for (unsigned int i = 0; i < n_users; ++i)
    {
        u[i] = new double[latent_factors];

        for (unsigned int j = 0; j < latent_factors; ++j)
            u[i][j] = d(gen);
    }

    v = 0;
    v = new double*[n_movies];
    for (unsigned int i = 0; i < n_movies; ++i)
    {
        v[i] = new double[latent_factors];

        for (unsigned int j = 0; j < latent_factors; ++j)
            v[i][j] = d(gen);
    }

    clock_t begin = 0;


    clock_t end = clock();
    double elapsed_min = double(end - begin) / CLOCKS_PER_SEC / 60;
    cout << elapsed_min << " minutes to get ratings" << endl;

    // Baseline averages
    //userAvg = arma::vec(n_users).randn();
    //movieAvg = arma::vec(n_movies).randn();
    //for (int i = 0; i < n_users; ++i)
    //    userAvg(i) = l.getUserMean(i);
    //for (int i = 0; i < n_movies; ++i)
    //    movieAvg(i) = l.getMovieMean(i);

    //global_average = mean(y.row(2));
    //cout << "mean: " << global_average << endl;


}

SGD2::~SGD2()
{
    delete[] u;
    delete[] v;
    delete[] y;
    delete[] probe;
}

// Runs alternating least squares to create the U and V matrices
// wij = 1 if user i rated movie j and 0 otherwise
void SGD2::run_sgd()
{
    LoadData2 *l = new LoadData2();
    y = 0;
    y = new double*[3];
    y[0] = new double[98291669];
    y[1] = new double[98291669];
    y[2] = new double[98291669];
    y = l->loadRatingsVector();
    cout << y[0][0] << endl;
    cout << y[1][0] << endl;
    clock_t begin = 0;
    std::cout << "running SGD" << std::endl;

    double new_error = 0;
    double old_error = 1000000;
    double **prev_u = u;
    double **prev_v = v;
    prev_u = 0;
    prev_u = new double*[n_users];
    prev_v = 0;
    prev_v = new double*[n_movies];
    for (unsigned int i = 0; i < n_users; ++i)
        prev_u[i] = new double[latent_factors];
    for (unsigned int i = 0; i < n_movies; ++i)
        prev_v[i] = new double[latent_factors];

    double lr; // learn rate
    int user;
    int movie;
    int rating;
    double estimate;
    double error;
    double *oldu = 0;
    oldu = new double[n_users];

    for (unsigned int epoch = 1; epoch < 100; epoch++) {

        std::cout << "New epoch " << epoch << std::endl;

        //learn rate is original divided by epoch
        //lr = learn_rate / epoch;
        // Learning rate from funny paper
        lr = learn_rate;
        std::cout << lr << std::endl;
        //shuffle(y);

        // Iterate through data points
        for (unsigned int i = 0; i < 98291669; i++) { //< y.n_cols
            rating = y[2][i];
            user = y[0][i] - 1;
            movie = y[1][i] - 1;
            //std::cout << "checkpoint 1" << endl;
            //std::cout << user << " " << movie << " " << rating << endl;
            estimate = 0;
            for (unsigned int j = 0; j < latent_factors; ++j) {
                //std::cout << j << endl;
                //std::cout << user << std::endl;
                estimate += u[user][j] * v[movie][j];
            }
            //std::cout << "checkpoint 2" << endl;
            error = (rating - estimate);

            oldu = u[user];
            //std::cout << "checkpoint 3" << endl;
            for (unsigned int k = 0; k < latent_factors; ++k)
                u[user][k] += lr * (error * v[movie][k] - lambda * oldu[k]);
            for (unsigned int k = 0; k < latent_factors; ++k)
                v[movie][k] += lr * (error * oldu[k] - lambda * v[movie][k]);
            //std::cout << "checkpoint 4" << endl;

        }
        cout << "for loop done" << endl;

        // Find the error for the new values
        new_error = find_error(epoch);

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

    create_file();
    delete[] oldu;
    delete[] prev_u;
    delete[] prev_v;
}

// Find the test error on the probe data set.
double SGD2::find_error(int epoch) {
    probe = 0;
    probe = new double*[3];
    probe[0] = new double[1374739];
    probe[1] = new double[1374739];
    probe[2] = new double[1374739];
    probe = LoadData2::probe();
    double error = 0;
    // Go through all the columns of probe
    // for (unsigned int i = 0; i < 16; ++i)
    for (unsigned int i = 0; i < 1374739; ++i)
    {
       // std::cout << probe[0][i] << " probe" << std::endl;
        int user = probe[0][i] - 1;
        int movie = probe[1][i] - 1;
        double rating = probe[2][i];

        double predicted = 0;
        //std::cout << u[user][1] << " u" << endl;
        for (unsigned int j = 0; j < latent_factors; ++j)
            predicted += u[user][j] * v[movie][j];

        //cout << predicted << " " << rating << endl;
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
void SGD2::create_file()
{
    ofstream myfile1;
    std::string s = "new_sgd_results.txt";
    myfile1.open(s);
    double **qual = 0;
    qual = new double *[2];
    qual[0] = new double[2749898];
    qual[1] = new double[2749898];
    qual = LoadData2::qual();

    for (unsigned int i = 0; i < 2749898; ++i)
    {
        int user = qual[0][i] - 1;
        int movie = qual[1][i] - 1;

        double predicted = 0;
        for (unsigned int j = 0; j < latent_factors; ++j)
            predicted += u[user][j] * v[movie][j];


        // Truncate predictions
        if (predicted < 1)
            predicted = 1;
        else if (predicted > 5)
            predicted = 5;
        myfile1 << predicted << "\n";

    }
    myfile1.close();
}
int main() {
    SGD2 sgd(30, 0.02, 0.001); // remember to have learning rate divided by number of epochs
    std::cout << "done loading\n";
    sgd.run_sgd();


}



