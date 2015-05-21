
#include "SGDpp.h"
#include "LoadData2.h"
#include <ctime>
#include <fstream>
#include <iostream>
#include <random>

int main() {
    //SGDpp sgd(100, 0.005, 0.007, 0.015); 4.4%
    //SGDpp sgd(100, 0.008, 0.007, 0.02);  //4.65% .905 probe
    //SGDpp sgd(100, 0.02, 0.007, 0.02); worse
    //SGDpp sgd(75, 0.008, 0.007, 0.02); 4.58%
    //SGDpp sgd(150, 0.008, 0.007, 0.02); 4.77
    //SGDpp sgd(100, 0.01, 0.007, 0.025); 4.77 (file 10)
    //SGDpp sgd(150, 0.008, 0.007, 0.02); 4.78 (file 11)
    //SGDpp sgd(100, 0.01, 0.01, 0.02); 4.82 (file 12)
    //SGDpp sgd(150, 0.012, 0.01, 0.02); 5.01 (file 13)
    //SGDpp sgd(150, 0.016, 0.01, 0.02); 4.95 (file 14)
    //SGDpp sgd(200, 0.012, 0.01, 0.02); 5.06 (file 15)
    //SGDpp sgd(200, 0.012, 0.01, 0.024); 5.07 (file 16)
    //SGDpp sgd(200, 0.012, 0.01, 0.016); 5.07 (file 17)
    //SGDpp sgd(150, 0.06, 0.01, 0.1); 2.41 (file 18)
    //SGDpp sgd(200, 0.012, 0.015, 0.02); 4.76 (file 19)
    //SGDpp sgd(250, 0.012, 0.01, 0.02); 5.12 (file 20)
    SGDpp sgd(400, 0.012, 0.01, 0.02);
    
    std::cout << "Done loading\n";
    sgd.run_sgd();
    
}

SGDpp::SGDpp(int lf, double lambda_val, double lr, double lambda_y)
{
    bool testingOnProbe = false; // change this in LoadData2.cpp as well
    outfile = "SGDpp_results21.txt";
    outfileProbe = "SGDpp_probe21.txt";


    // Set the number of latent factors, users, and movies
    latent_factors = lf;
    learn_rate = lr;
    n_users = 458293;
    n_movies = 17770;
    if (testingOnProbe)
        n_datapoints = 1374739;
    else
        n_datapoints = 98291669;
    lambda = lambda_val;
    lambdaY = lambda_y;

    // Create a normal distribution to sample random numbers from
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> d(0, 0.01);

    // Create the U and V matrices based on these parameters
    // Randomly initialize all values
    u = 0;
    u = new double*[n_users];
    prev_u = 0;
    prev_u = new double*[n_users];
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
    
    
    prev_u = 0;
    prev_u = new double*[n_users];
    for (unsigned int i = 0; i < n_users; ++i)
    {
        u[i] = new double[latent_factors];
    }
    
    prev_v = 0;
    prev_v = new double*[n_movies];
    for (unsigned int i = 0; i < n_movies; ++i)
    {
        prev_v[i] = new double[latent_factors];
    }

    l = new LoadData2();

    global_mean = l->getGlobalMean();
    cout << "Global Mean = " << global_mean << endl;
    movies_per_user = l->getMoviesPerUser();
    norms = l->getNorms();

    cout << "Initiating SVD++ implicit arrays" << endl;
    y = new double*[n_movies];

    for (unsigned int i = 0; i < n_movies; ++i)
    {
        y[i] = new double[lf];

        for (unsigned int j = 0; j < lf; ++j) {
            //y[i][j] = 0.1;
            y[i][j] = d(gen);
        }
    }

    sumY = new double*[n_users];

    for (unsigned int i = 0; i < n_users; ++i)
    {
        sumY[i] = new double[lf];
        
        for (unsigned int j = 0; j < lf; ++j) {
            sumY[i][j] = 0.0;
        }

        set<int>::iterator it;
        for (it = movies_per_user[i].begin(); it != movies_per_user[i].end(); ++it) {
            int thisMovie = *it - 1;
            
            for (unsigned int j = 0; j < lf; ++j) {
                sumY[i][j] += y[thisMovie][j];
            }
        }
    }
    cout << "Done initializing SVD++ implicit arrays" << endl;

    clock_t begin = 0;

    data = 0;
    data = new double*[3];
    data[0] = new double[n_datapoints];
    data[1] = new double[n_datapoints];
    data[2] = new double[n_datapoints];
    data = l->loadRatingsVector();

    user_avg = 0;
    user_avg = new double[n_users];
    for (unsigned int i = 0; i < n_users; ++i)
        user_avg[i] = 0;
    movie_avg = 0;
    movie_avg = new double[n_movies];
    for (unsigned int i = 0; i < n_movies; ++i)
        movie_avg[i] = 0;



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

SGDpp::~SGDpp()
{
    delete[] u;
    delete[] v;
    delete[] data;
    delete[] probe;
    delete[] y;
    delete[] sumY;
    delete[] movies_per_user;
}

// Runs alternating least squares to create the U and V matrices
// wij = 1 if user i rated movie j and 0 otherwise
void SGDpp::run_sgd()
{
    clock_t begin = 0;
    std::cout << "running SGD++" << std::endl;

    double new_error = 0;
    double old_error = 1000000;
    
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
    

    double lr = learn_rate;
    int user;
    int movie;
    int rating;
    double error;

    double *tempSumY = new double[latent_factors];

    for (int i = 0; i < latent_factors; i++)
            tempSumY[i] = 0.0;

    for (unsigned int epoch = 1; epoch < 41; epoch++) {

        std::cout << "New epoch " << epoch << std::endl;

        // Should shuffle? Can't blindly shuffle with this algorithm
        // Relies on user ratings being in order
        //shuffle(y);

        int prev_user = -1;
        int perUser = 0;

        
        
        clock_t begin2 = clock();
        double countTo10 = 0;
        // Iterate through data points, one user at a time
        for (unsigned int i = 0; i < n_datapoints; ++i) { //< data.n_cols
//        for (unsigned int i = 0; i <  1374739; i++) { //< data.n_cols


            user = data[0][i] - 1;
            movie = data[1][i] - 1;
            rating = data[2][i];

            double *currentPointSumY = sumY[user];

            int userNumMovies = movies_per_user[user].size();
            //std::cout << user << " " << movie << " " << rating << endl;

            //std::cout << "checkpoint 2" << endl;
            error = rating - SGDpp::estimateRating(user, movie);

            // Update user avg and movie avg
            user_avg[user] += lr * (error - lambda * user_avg[user]);
            movie_avg[movie] += lr * (error - lambda * movie_avg[movie]);

            if (user != prev_user) {
                perUser = 1;
            } else {
                perUser++;
            }


            for (unsigned int k = 0; k < latent_factors; ++k) {
                double oldUserVal = u[user][k];
                double oldMovieVal = v[movie][k];

                u[user][k] += lr * (error * oldMovieVal - lambda * oldUserVal);
                v[movie][k] += lr * (error * (oldUserVal + norms[user] * currentPointSumY[k]) - lambda * oldMovieVal);
                tempSumY[k] += (error * norms[user] * oldMovieVal);

                if (perUser == userNumMovies) {
                    double totalUpdate = 0;

                    set<int>::iterator it;
                    for (it = movies_per_user[user].begin(); it != movies_per_user[user].end(); ++it) {
                        int thisMovie = *it - 1;
                        double *currY = y[thisMovie];
                        double update = lr * (tempSumY[k] - lambdaY * currY[k]);
                        currY[k] += update;
                        totalUpdate += update;
                    }


                    sumY[user][k] += totalUpdate;
                }

            //std::cout << "checkpoint 4" << endl;

            }

            if (perUser == userNumMovies) {
                for (int k = 0; k < latent_factors; k++) {
                    tempSumY[k] = 0.0;
                }
            }
            if(i % (n_datapoints / 10) == 1) {
                if (countTo10 != 0 && countTo10 != 10) {
                    cout << ((double)clock() - begin2) / CLOCKS_PER_SEC / 60. << " minute chunk ";
                    cout << countTo10 << "/10 done, ETA = " <<
                    ((double)clock() - begin2) / CLOCKS_PER_SEC / 60. * (n_datapoints - i) / (n_datapoints / 10 + 1) << " minutes" << endl;
                }
                begin2 = clock();
                
                countTo10++;
            }

            prev_user = user;
        }
        cout << "Epoch completed." << endl;

        // Find the error for the new values
        new_error = find_error(epoch);

        // If there's no decrease in error, stop.
        std::cout << "RMSE: " << new_error << std::endl;
        std::cout << "Old error: " << old_error << std::endl;
        if (new_error + .0001 >= old_error && epoch > 5) {
            if (new_error > old_error) {
                // update prev_u and prev_v
                for (unsigned int i = 0; i < n_users; ++i)
                {
                    for (unsigned int j = 0; j < latent_factors; ++j)
                        u[i][j] = prev_u[i][j];
                }
                for (unsigned int i = 0; i < n_movies; ++i)
                {
                    for (unsigned int j = 0; j < latent_factors; ++j)
                        v[i][j] = prev_v[i][j];
                }
            }
            break;
        }
        old_error = new_error;
        
        // update prev_u and prev_v
        for (unsigned int i = 0; i < n_users; ++i)
        {
            u[i] = new double[latent_factors];
            
            for (unsigned int j = 0; j < latent_factors; ++j)
                prev_u[i][j] = u[i][j];
        }
        for (unsigned int i = 0; i < n_movies; ++i)
        {
            u[i] = new double[latent_factors];
            
            for (unsigned int j = 0; j < latent_factors; ++j)
                prev_v[i][j] = v[i][j];
        }


        lr *= 0.9; // make this a variable

    }

    delete[] tempSumY;
    create_file();
    create_probe_file();
    
    cout << ((double)clock() - begin) / CLOCKS_PER_SEC / 60. << " minutes to learn" << endl;

    delete[] prev_u;
    delete[] prev_v;
}

// Find the test error on the probe data set.
double SGDpp::find_error(int epoch) {
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

        double predicted = SGDpp::estimateRating(user, movie);


        error += (rating - predicted) * (rating - predicted);
    }

    // Scale by the number of reviews
    return pow(error / 1374739, 0.5);
    //return error / 16;

}

// Create an output file using the u and v matrices we found and the qual
// data
void SGDpp::create_file()
{
    ofstream myfile1;
    myfile1.open(outfile);
    double **qual = 0;
    qual = new double *[2];
    qual[0] = new double[2749898];
    qual[1] = new double[2749898];
    qual = LoadData2::qual();
    cout << "Creating output file" << endl;
    int c = 0;

    for (unsigned int i = 0; i < 2749898; ++i)
    {
        int user = qual[0][i] - 1;
        int movie = qual[1][i] - 1;
//        if (c < 7) {
//        }

        //double predicted = user_vec[user] - global_mean + movie_vec[movie];
        //double predicted = 0;
        double predicted = SGDpp::estimateRating(user, movie);

        c += 1;

        myfile1 << predicted << "\n";

    }
    //myfile1.close();
}

// Create an output file using the u and v matrices we found and the qual
// data
void SGDpp::create_probe_file()
{
    ofstream myfile1;
    myfile1.open(outfileProbe);
    cout << "Creating output file" << endl;
    int c = 0;
    
    for (unsigned int i = 0; i < 1374739; ++i)
    {
        int user = probe[0][i] - 1;
        int movie = probe[1][i] - 1;

        double predicted = SGDpp::estimateRating(user, movie);
        
        c += 1;
        
        myfile1 << predicted << "\n";
        
    }
}

double SGDpp::estimateRating(int user, int movie) {
    double estimate = user_avg[user] + global_mean + movie_avg[movie];

    for (unsigned int j = 0; j < latent_factors; ++j) {
        estimate += (u[user][j] + sumY[user][j] * norms[user]) * v[movie][j];
    }

    if (estimate > 5)
        estimate = 5;
    else if (estimate < 1)
        estimate = 1;

    return estimate;
}




