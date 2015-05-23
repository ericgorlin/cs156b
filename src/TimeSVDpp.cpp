
#include "TimeSVDpp.h"
#include "LoadData3.h"
#include <ctime>
#include <fstream>
#include <iostream>
#include <random>


int main() {
    //TimeSVDpp sgd(10, 0.006, 0.011, 0.0003, 0.03, 0.03, 0.08, 0.006, 40, 0.03, 0.03, 0.001);
    //TimeSVDpp sgd(10, 0.006, 0.011, 0.0003, 0.03, 0.03, 0.08, 0.006, .04, 0.03, 0.03, 0.001);
    
    
    
    //TimeSVDpp sgd(10, .01, .01, .01, .01, .01, .012, .012, .4, .024, .012, .012); f1
    // add bins 30?
    //TimeSVDpp sgd(150, .01, .01, .001, .01, .01, .012, .012, .4, .024, .012, .012); // 5.0% f2
    //TimeSVDpp sgd(150, .01, .01, .01, .01, .01, .012, .012, .4, .024, .012, .012); //  4.96% f3
    
    
    
    std::cout << "Done loading\n";
    sgd.run_sgd();

}


TimeSVDpp::TimeSVDpp(int lf, double lruser, double lrmovie, double lralpha, double lruserbias, double lrmoviebias, double lambdauser, double lambdamovie, double lambdaalpha, double lambday, double lambdauserbias, double lambdamoviebias)
{
    nbins = 30;
    bool testingOnProbe = false; // change this in LoadData3.cpp as well
    outfile = "TimeSVDpp_resultsEG3.txt";
    outfileProbe = "TimeSVDpp_probeEG3.txt";


    // Set the number of latent factors, users, and movies

    n_users = 458293;
    n_movies = 17770;
    if (testingOnProbe)
        n_datapoints = 1374739;
    else
        n_datapoints = 98291669;

    latentFactors = lf;
    lrUser = lruser;
    lrMovie = lrmovie;
    lrAlpha = lralpha;
    lrUserBias = lruserbias;
    lrMovieBias = lrmoviebias;
    lambdaUser = lambdauser;
    lambdaMovie = lambdamovie;
    lambdaAlpha = lambdaalpha;
    lambdaY = lambday;
    lambdaUserBias = lambdauserbias;
    lambdaMovieBias = lambdamoviebias;

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
        u[i] = new double[latentFactors];

        for (unsigned int j = 0; j < latentFactors; ++j)
            u[i][j] = d(gen);
    }

    v = 0;
    v = new double*[n_movies];
    for (unsigned int i = 0; i < n_movies; ++i)
    {
        v[i] = new double[latentFactors];

        for (unsigned int j = 0; j < latentFactors; ++j)
            v[i][j] = d(gen);
    }


    prev_u = 0;
    prev_u = new double*[n_users];
    for (unsigned int i = 0; i < n_users; ++i)
    {
        u[i] = new double[latentFactors];
    }

    prev_v = 0;
    prev_v = new double*[n_movies];
    for (unsigned int i = 0; i < n_movies; ++i)
    {
        prev_v[i] = new double[latentFactors];
    }

    l = new LoadData3();

    global_mean = l->getGlobalMean();
    cout << "Global Mean = " << global_mean << endl;
    movies_per_user = l->getMoviesPerUser();
    norms = l->getNorms();
    userAvgDate = l->getUserAvgDate();

    cout << "Initiating SVD++ implicit arrays" << endl;
    y = new double*[n_movies];

    for (unsigned int i = 0; i < n_movies; ++i)
    {
        y[i] = new double[lf];

        for (unsigned int j = 0; j < lf; ++j) {

            y[i][j] = d(gen);
        }
    }

    alpha = new double[n_users];

    sumY = new double*[n_users];
    alphat = new double*[n_users];
    for (unsigned int i = 0; i < n_users; ++i)
    {
        alpha[i] = d(gen);
        sumY[i] = new double[lf];
        alphat[i] = new double[lf];

        for (unsigned int j = 0; j < lf; ++j) {
            sumY[i][j] = 0;
            alphat[i][j] = 0;
        }        set<int>::iterator it;
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
    data = new double*[4];
    data[0] = new double[n_datapoints];
    data[1] = new double[n_datapoints];
    data[2] = new double[n_datapoints];
    data[3] = new double[n_datapoints];
    data = l->loadRatingsVector();
    minDate = l->getMinDate();
    maxDate = l->getMaxDate();

    user_avg = 0;
    user_avg = new double[n_users];
    for (unsigned int i = 0; i < n_users; ++i)
        user_avg[i] = d(gen);
    movie_avg = 0;
    movie_avg = new double[n_movies];
    for (unsigned int i = 0; i < n_movies; ++i)
        movie_avg[i] = d(gen);


    movie_bins = 0;
    movie_bins = new double*[n_movies];
    for (unsigned int i = 0; i < n_movies; ++i) {
        movie_bins[i] = new double[nbins];
        for (unsigned int j = 0; j < nbins; ++j)
            movie_bins[i][j] = 0;
    }

    dateRange = new int[n_users];
    // Find the date range per user
    std::set<int> *dates_per_user = l->getDatesPerUser();
    set<int>::iterator it;
    for (int i = 0; i < n_users; ++i) {
        int first = 10000000;
        int last = 0;
        int date;
        for (it = dates_per_user[i].begin(); it != dates_per_user[i].end(); ++it) {
            date = *it;
            if (date > last)
                last = date;
            if (date < first)
                first = date;
        }
        if (last != first) {
            dateRange[i] = last - first;
        }
        else
            dateRange[i] = 1;
    }


    clock_t end = clock();
    double elapsed_min = double(end - begin) / CLOCKS_PER_SEC / 60;
    cout << elapsed_min << " minutes to get ratings" << endl;

}

TimeSVDpp::~TimeSVDpp()
{
    delete[] u;
    delete[] v;
    delete[] data;
    delete[] probe;
    delete[] y;
    delete[] sumY;
    delete[] alpha;
    delete[] movies_per_user;
    delete[] dateRange;
}

int TimeSVDpp::dateToBin(int date) {
    int total = maxDate - minDate;
    int curr = minDate;

    for (int i = 1; i <= nbins; ++i)
    {
        if (date >= curr && date < minDate + total / nbins * i) {
            return (i - 1);
        }
    }
}

// Runs alternating least squares to create the U and V matrices
// wij = 1 if user i rated movie j and 0 otherwise
void TimeSVDpp::run_sgd()
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
        prev_u[i] = new double[latentFactors];
    for (unsigned int i = 0; i < n_movies; ++i)
        prev_v[i] = new double[latentFactors];

    int user;
    int movie;
    int rating;
    int date;
    double error;
    double lr = lrUser;
    double lambda = lambdaUser;

    double *tempSumY = new double[latentFactors];

    for (int i = 0; i < latentFactors; i++)
        tempSumY[i] = 0.0;

    for (unsigned int epoch = 1; epoch < 20; epoch++) {

        std::cout << "New epoch " << epoch << std::endl;

        // Should shuffle? Can't blindly shuffle with this algorithm
        // Relies on user ratings being in order
        //shuffle(y);

        int prev_user = -1;
        int perUser = 0;



        clock_t begin2 = clock();
        double countTo10 = 0;
        // Iterate through data points, one user at a time
        for (unsigned int i = 0; i < n_datapoints; ++i) {
            //std::cout << "1" << std::endl;
            user = data[0][i] - 1;
            movie = data[1][i] - 1;
            rating = data[2][i];
            date = data[3][i];

            //std::cout << "2" << std::endl;

            double *currentPointSumY = sumY[user];
            //currAlpha = alpha[user];

            //std::cout << "3" << std::endl;
            int userNumMovies = movies_per_user[user].size();
            //std::cout << user << " " << movie << " " << rating << endl;

            //std::cout << "checkpoint 2" << endl;
            //std::cout << user << std::endl;
            error = rating - TimeSVDpp::estimateRating(user, movie, date);

            //std::cout << "4" << std::endl;

            // Update user avg and movie avg
            user_avg[user] += lrUserBias * (error - lambdaUserBias * user_avg[user]);
            movie_avg[movie] += lrMovieBias * (error - lambdaMovieBias * movie_avg[movie]);
            movie_bins[movie][dateToBin(date)] += lrMovieBias * (error - lambdaMovieBias * movie_bins[movie][dateToBin(date)]);
            //std::cout << dateToBin(date) << std::endl;
            //user_avg[user] += lr * (error - lambda * user_avg[user]);
            //movie_avg[movie] += lr * (error - lambda * movie_avg[movie]);

            if (user != prev_user) {
                perUser = 1;
            } else {
                perUser++;
            }
            double timeVal = pow(abs(date - userAvgDate[user]), 0.4) * (date - userAvgDate[user] > 0 ? 1 : -1);
            timeVal /= dateRange[user];
            alpha[user] += lrAlpha * (error * timeVal - lambdaAlpha * alpha[user]);

            for (unsigned int k = 0; k < latentFactors; ++k) {
                double oldUserVal = u[user][k];
                double oldMovieVal = v[movie][k];

                u[user][k] += lrUser * (error * oldMovieVal - lambdaUser * oldUserVal);
                v[movie][k] += lrMovie * (error * (oldUserVal + norms[user] * currentPointSumY[k]) - lambdaMovie * oldMovieVal);
                //tempSumY[k] += error * norms[user] * oldMovieVal;
                //u[user][k] += lr * (error * oldMovieVal - lambda * oldUserVal);
                //v[movie][k] += lr * (error * (oldUserVal + norms[user] * currentPointSumY[k]) - lambda * oldMovieVal);
                tempSumY[k] += (error * norms[user] * oldMovieVal);

                alphat[user][k] += lrAlpha * (error * timeVal - lambdaAlpha * alphat[user][k]);
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
                for (int k = 0; k < latentFactors; k++) {
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
                    for (unsigned int j = 0; j < latentFactors; ++j)
                        u[i][j] = prev_u[i][j];
                    }
                for (unsigned int i = 0; i < n_movies; ++i)
                {
                    for (unsigned int j = 0; j < latentFactors; ++j)
                        v[i][j] = prev_v[i][j];
                }
            }
            break;
        }
        old_error = new_error;

        // update prev_u and prev_v
        for (unsigned int i = 0; i < n_users; ++i)
        {
            u[i] = new double[latentFactors];

            for (unsigned int j = 0; j < latentFactors; ++j)
                prev_u[i][j] = u[i][j];
        }
        for (unsigned int i = 0; i < n_movies; ++i)
        {
            u[i] = new double[latentFactors];

            for (unsigned int j = 0; j < latentFactors; ++j)
                prev_v[i][j] = v[i][j];
        }


        lr *= 0.9; // make this a variable
        lrUser *= .9;
        lrUserBias *= .9;
        lrMovie *= .9;
        lrMovieBias *= .9;
        lrAlpha *= .9;

    }

    delete[] tempSumY;
    create_file();
    create_probe_file();

    cout << ((double)clock() - begin) / CLOCKS_PER_SEC / 60. << " minutes to learn" << endl;

    delete[] prev_u;
    delete[] prev_v;
}

// Find the test error on the probe data set.
double TimeSVDpp::find_error(int epoch) {
    probe = 0;
    probe = new double*[4];
    probe[0] = new double[1374739];
    probe[1] = new double[1374739];
    probe[2] = new double[1374739];
    probe[3] = new double[1374739];
    probe = LoadData3::probe();
    double error = 0;
    // Go through all the columns of probe
    // for (unsigned int i = 0; i < 16; ++i)
    for (unsigned int i = 0; i < 1374739; ++i)
    {
        // std::cout << probe[0][i] << " probe" << std::endl;
        int user = probe[0][i] - 1;
        int movie = probe[1][i] - 1;
        double rating = probe[2][i];
        int date = probe[3][i];

        double predicted = TimeSVDpp::estimateRating(user, movie, date);


        error += (rating - predicted) * (rating - predicted);
    }

    // Scale by the number of reviews
    return pow(error / 1374739, 0.5);


}

// Create an output file using the u and v matrices we found and the qual
// data
void TimeSVDpp::create_file()
{
    ofstream myfile1;
    myfile1.open(outfile);
    double **qual = 0;
    qual = new double *[3];
    qual[0] = new double[2749898];
    qual[1] = new double[2749898];
    qual[2] = new double[2749898];
    qual = LoadData3::qual();
    cout << "Creating output file" << endl;
    int c = 0;

    for (unsigned int i = 0; i < 2749898; ++i)
    {
        int user = qual[0][i] - 1;
        int movie = qual[1][i] - 1;
        int date = qual[2][i];

        double predicted = TimeSVDpp::estimateRating(user, movie, date);

        c += 1;

        myfile1 << predicted << "\n";

    }
    //myfile1.close();
}

// Create an output file using the u and v matrices we found and the qual
// data
void TimeSVDpp::create_probe_file()
{
    ofstream myfile1;
    myfile1.open(outfileProbe);
    cout << "Creating output file" << endl;
    int c = 0;

for (unsigned int i = 0; i < 1374739; ++i)
{
    int user = probe[0][i] - 1;
    int movie = probe[1][i] - 1;
    int date = probe[2][i];

    double predicted = TimeSVDpp::estimateRating(user, movie, date);

    c += 1;

    myfile1 << predicted << "\n";

    }
}

double TimeSVDpp::estimateRating(int user, int movie, int date) {
    double estimate = user_avg[user] + global_mean + movie_avg[movie] + movie_bins[movie][dateToBin(date)];
    double timeVal = pow(abs(date - userAvgDate[user]), 0.4) * (date - userAvgDate[user] > 0 ? 1 : -1);
    timeVal /= dateRange[user];
    //std::cout <<  << std::endl;
    estimate += alpha[user] * timeVal;
    //std::cout << movie_bins[movie][dateToBin(date)] << std::endl;


    //std::cout << timeVal << std::endl;
    //cout << (date - userAvgDate[user] > 0 ? 1 : -1) << endl;

    for (unsigned int j = 0; j < latentFactors; ++j) {
        estimate += (u[user][j] + alphat[user][j] * timeVal + sumY[user][j] * norms[user]) * v[movie][j];
        //estimate += (u[user][j] + sumY[user][j] * norms[user]) * v[movie][j];
        //estimate += (u[user][j] + sumY[user][j] * norms[user]) * v[movie][j];
    }

    if (estimate > 5)
        estimate = 5;
    else if (estimate < 1)
        estimate = 1;

    return estimate;
}




