#include "SGD2.h"
#include "LoadData2.h"
#include <ctime>
#include <fstream>
#include <iostream>
#include <cmath>
#include <cstdlib>

SGD2::SGD2(int lf, double lambda_val, double lr, double lambda2, double lr2)
{
    // Set the number of latent factors, users, and movies
    latent_factors = lf;
    learn_rate = lr;
    n_users = 458293;
    n_movies = 17770;
    //n_users = 5;
    //n_movies = 17754;
    lambda = lambda_val;
    lambda_avg = lambda2;
    learn_rate_avg = lr2;
    outfile = "sgd_results18probeinc2.txt";
    outprobe = "sgd_probe18probeinc2.txt";

    // Create a normal distribution to sample random numbers from
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> d(0,.01);
    srand (static_cast <unsigned> (time(0)));

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

    l = new LoadData2();
    y = 0;
    y = new double*[3];
    //y[0] = new double[98291669];
    //y[1] = new double[98291669];
    //y[2] = new double[98291669];
    y[0] = new double[99666408];
    y[1] = new double[99666408];
    y[2] = new double[99666408];
    y = l->loadRatingsVector();
    std::cout << "checkpoint 1" << std::endl;

    user_avg = 0;
    user_avg = new double[n_users];
    for (unsigned int i = 0; i < n_users; ++i)
        //user_avg[i] = d(gen);
    movie_avg = 0;
    movie_avg = new double[n_movies];
    for (unsigned int i = 0; i < n_movies; ++i)
        movie_avg[i] = d(gen);

    global_mean = l->getGlobalMean();

    //user_vec = l->getBetterUserMean();
    //movie_vec = l->getBetterMovieMean();
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
double SGD2::run_sgd()
{
    cout << y[0][0] << endl;
    cout << y[1][0] << endl;
    clock_t begin = 0;
    std::cout << "running SGD" << std::endl;

    double new_error = 0;
    double old_error = 1000000;
    /*
    double **prev_u = 0;
    double **prev_v = 0;
    prev_u = 0;
    prev_u = new double*[n_users];
    prev_v = 0;
    prev_v = new double*[n_movies];
    for (unsigned int i = 0; i < n_users; ++i)
        prev_u[i] = new double[latent_factors];
    for (unsigned int i = 0; i < n_movies; ++i)
        prev_v[i] = new double[latent_factors];
    */

    double lr; // learn rate
    int user;
    int movie;
    int rating;
    double estimate;
    double error;
    double *oldu = 0;
    double uval;
    double vval;
    oldu = new double[n_users];

    for (unsigned int epoch = 1; epoch < 18; epoch++) {
        clock_t begin = 0;

        std::cout << "New epoch " << epoch << std::endl;

        //learn rate is original divided by epoch
        //lr = learn_rate / epoch;
        // Learning rate from funny paper
        lr = learn_rate;
        std::cout << lr << std::endl;
        //shuffle(y);

        // Iterate through data points
        //#pragma omp parallel for
        //for (unsigned int i = 0; i < 98291669; i++) { //< y.n_cols
        for (unsigned int i = 0; i < 99666408; ++i) {
            rating = y[2][i];
            user = y[0][i] - 1;
            movie = y[1][i] - 1;
            //std::cout << "checkpoint 1" << endl;
            //std::cout << user << " " << movie << " " << rating << endl;
            estimate = user_avg[user] + global_mean + movie_avg[movie];
            //estimate = 0
            //#pragma omp parallel for
            for (unsigned int j = 0; j < latent_factors; ++j) {
                estimate += u[user][j] * v[movie][j];

                /*
                if (estimate > 5)
                    estimate = 5;
                else if (estimate < 1)
                    estimate = 1;
                */
            }

            if (estimate > 5)
                estimate = 5;
            else if (estimate < 1)
                estimate = 1;
            //std::cout << "checkpoint 2" << endl;
            error = (rating - estimate);

            oldu = u[user];
            //std::cout << "checkpoint 3" << endl;
            //#pragma omp parallel for
            for (unsigned int k = 0; k < latent_factors; ++k)
                u[user][k] += lr * (error * v[movie][k] - lambda * oldu[k]);
            //#pragma omp parallel for
            for (unsigned int k = 0; k < latent_factors; ++k)
                v[movie][k] += lr * (error * oldu[k] - lambda * v[movie][k]);
            //std::cout << "checkpoint 4" << endl;

            // Update user avg and movie avg
            user_avg[user] += learn_rate_avg * (error - lambda_avg * user_avg[user]);
            movie_avg[movie] += learn_rate_avg * (error - lambda_avg * movie_avg[movie]);

        }
        cout << "for loop done" << endl;

        // Find the error for the new values
        new_error = find_error(epoch);

        // If there's no decrease in error, stop.
        std::cout << "Error: " << new_error << std::endl;
        std::cout << "Old error: " << old_error << std::endl;
        if (new_error + .00001 >= old_error && epoch > 5) {
            //u = prev_u;
            //v = prev_v;
            break;
        }
        old_error = new_error;

        clock_t end = clock();
        double elapsed_min = double(end - begin) / CLOCKS_PER_SEC / 60;
        cout << elapsed_min << " minutes to get for this epoch" << endl;
    }

    create_file();
    create_probe_file();
    delete[] oldu;
    return new_error;
    //delete[] prev_u;
    //delete[] prev_v;
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
    double uval = 0;
    double vval = 0;
    // Go through all the columns of probe
    // for (unsigned int i = 0; i < 16; ++i)
    for (unsigned int i = 0; i < 1374739; ++i)
    {
       // std::cout << probe[0][i] << " probe" << std::endl;
        int user = probe[0][i] - 1;
        int movie = probe[1][i] - 1;
        double rating = probe[2][i];

        //double predicted = user_vec[user] - global_mean + movie_vec[movie];
        //double predicted = 0;
        double predicted = global_mean + user_avg[user] + movie_avg[movie];
        //std::cout << u[user][1] << " u" << endl;
        for (unsigned int j = 0; j < latent_factors; ++j) {
            predicted += u[user][j] * v[movie][j];
            /*
            if (predicted > 5)
                predicted = 5;
            else if (predicted < 1)
                predicted = 1;
            */
        }
        //predicted += user_vec[user] - global_mean + movie_vec[movie];

        //cout << predicted << " " << rating << endl;
        // Truncate the estimate to 1 and 5

        if (predicted < 1)
            predicted = 1;
        else if (predicted > 5)
            predicted = 5;


        error += (rating - predicted) * (rating - predicted);
    }

    // Scale by the number of reviews
    return sqrt(error / 1374739);
    //return error / 16;

}

// Create an output file using the u and v matrices we found and the qual
// data
void SGD2::create_file()
{
    ofstream myfile1;
    myfile1.open(outfile);
    double **qual = 0;
    qual = new double *[2];
    qual[0] = new double[2749898];
    qual[1] = new double[2749898];
    qual = LoadData2::qual();
    cout << "checkpoint 100" << endl;
    int c = 0;

    for (unsigned int i = 0; i < 2749898; ++i)
    {
        int user = qual[0][i] - 1;
        int movie = qual[1][i] - 1;
        //double predicted = user_vec[user] - global_mean + movie_vec[movie];
        //double predicted = 0;
        double predicted = user_avg[user] + global_mean + movie_avg[movie];
        for (unsigned int j = 0; j < latent_factors; ++j) {
            predicted += u[user][j] * v[movie][j];
            /*
            if (predicted > 5)
                predicted = 5;
            else if (predicted < 1)
                predicted = 1;
            */

            //cout << u[user][j] << " " << v[movie][j] << endl;
        }
        c += 1;

        if (predicted > 5)
            predicted = 5;
        else if (predicted < 1)
            predicted = 1;

        myfile1 << predicted << "\n";

    }
    //myfile1.close();
}

// Create an output file using the u and v matrices we found and the qual
// data
void SGD2::create_probe_file()
{
    ofstream myfile1;
    myfile1.open(outprobe);
    cout << "Creating output file" << endl;
    int c = 0;

    for (unsigned int i = 0; i < 1374739; ++i)
    {
        int user = probe[0][i] - 1;
        int movie = probe[1][i] - 1;

        //double predicted = user_vec[user] - global_mean + movie_vec[movie];
        //double predicted = 0;
        double predicted = user_avg[user] + global_mean + movie_avg[movie];
        for (unsigned int j = 0; j < latent_factors; ++j) {
            predicted += u[user][j] * v[movie][j];
            /*
            if (predicted > 5)
                predicted = 5;
            else if (predicted < 1)
                predicted = 1;
            */

            //cout << u[user][j] << " " << v[movie][j] << endl;
        }

        c += 1;

        if (predicted > 5)
            predicted = 5;
        else if (predicted < 1)
            predicted = 1;

        myfile1 << predicted << "\n";
    }
}

int main() {
    double error;
    //SGD2 sgd(120, .012, .005, .00001, .005); // remember to have learning rate divided by number of epochs
    ofstream myfile1;
    std::string s = "all_results5.txt";
    std::cout << "done loading\n";
    myfile1.open(s);
    /*
    error = sgd.run_sgd();
    myfile1 << "120 .012 .005 .00001 .005 " << error << endl;
    */
    SGD2 sgd2(120, .012, .005, .001, .001); //.911506    1    .904275 18          .90051 probe18   .89927 probe18 2
    //SGD2 sgd2(120, .015, .007, .001, .001); // .914145    2    .906024 19
    //SGD2 sgd2(120, .012, .006, .001, .001); // .912915    3
    //SGD2 sgd2(120, .012, .005, .0009, .0009); // .911835   4
    //SGD2 sgd2(50, .01, .005, .001, .001); // .911275    5          .907234 20
    //SGD2 sgd2(100, .01, .005, .001, .001); // .912609    6
   // SGD2 sgd2(30, .008, .005, .001, .001); // .912609    7
    //SGD2 sgd2(120, .011, .004, .001, .001); // .911411    8
    //SGD2 sgd2(300, .015, .007, .001, .001);  // .914979    9
    //SGD2 sgd2(300, .012, .005, .001, .001);  // .913362    10   /902195 24
    //SGD2 sgd2(400, .012, .005, .001, .001);  // .90Ŕ653    26
    //SGD2 sgd2(300, .01, .004, .001, .001); // .914953 11
    //SGD2 sgd2(200, .012, .005, .001, .001); // .912804     12      .902853 21    .89924    probe21
    //SGD2 sgd2(150, .012, .005, .001, .001); // .911957    13       .903853 22
    //SGD2 sgd2(80, .013, .0045, .001, .001); // .909817    14    // .904565
    //SGD2 sgd2(113, .013, .00465, .001, .001);
    //SGD2 sgd2(10, .01, .005, .001, .001); // .919 16       // .9   18578 24
    //SGD2 sgd2(400, .01, .005, .001, .001); // .916 17
    error = sgd2.run_sgd();
    //myfile1 << "100 .01 .005 .001 .001 " << error << endl;
    /*
    SGD2 sgd3(100, .012, .005, .001, .0001);
    error = sgd3.run_sgd();
    myfile1 << "100 .012 .005 .001 .0001 " << error << endl;
    SGD2 sgd4(100, .012, .005, .001, .00001);
    error = sgd4.run_sgd();
    myfile1 << "100 .012 .005 .001 .00001 " << error << endl;
    SGD2 sgd5(100, .012, .005, .001, .01);
    error = sgd5.run_sgd();
    myfile1 << "100 .012 .005 .001 .01 " << error << endl;
    SGD2 sgd6(100, .012, .005, .001, .1);
    error = sgd6.run_sgd();
    myfile1 << "100 .012 .005 .001 .1 " << error << endl;
    SGD2 sgd7(100, .012, .005, .001, 1);
    error = sgd7.run_sgd();
    myfile1 << "100 .012 .005 .001 1 " << error << endl;
    SGD2 sgd8(100, .012, .005, .001, 10);
    error = sgd8.run_sgd();
    myfile1 << "100 .012 .005 .001 10 " << error << endl;
    */

    /*
    SGD2 sgd9(100, .012, .005, .01, .001);
    error = sgd9.run_sgd();
    myfile1 << "100 .012 .005 .001 .0001 " << error << endl;
    SGD2 sgd10(100, .012, .005, .1, .001);
    error = sgd10.run_sgd();
    myfile1 << "100 .012 .005 .001 .00001 " << error << endl;
    SGD2 sgd11(100, .012, .005, .0001, .001);
    error = sgd11.run_sgd();
    myfile1 << "100 .012 .005 .001 .01 " << error << endl;
    SGD2 sgd12(100, .012, .005, .00001, .001);
    error = sgd12.run_sgd();
    myfile1 << "100 .012 .005 .001 .1 " << error << endl;
    SGD2 sgd13(100, .012, .005, .00001, .001);
    error = sgd13.run_sgd();
    myfile1 << "100 .012 .005 .001 1 " << error << endl;
    SGD2 sgd14(100, .012, .005, 1, 1);
    error = sgd14.run_sgd();
    myfile1 << "100 .012 .005 .001 10 " << error << endl;
    */
}
