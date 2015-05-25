#include "RBM.h"
#include "KNN.h"
#include "LoadData2.h"

RBM::RBM(unsigned int numHiddenFeatures, double eps, double wd, double mom)
{
    knn = new KNN(true); // bool input to add probe or not
    clock_t begin1 = clock();
    nFeatures = numHiddenFeatures;
    epsilon0 = eps;
    weightDecay = wd;
    momentum = mom;
    numEpochs = 0;
    // Instead of a 3d array, create a weight matrix for each rating k=1 to k=5
    weights1 = new double*[nFeatures];
    weights2 = new double*[nFeatures];
    weights3 = new double*[nFeatures];
    weights4 = new double*[nFeatures];
    weights5 = new double*[nFeatures];
    for (unsigned int i = 0; i < nFeatures; ++i)
    {
        weights1[i] = new double[17770];
        weights2[i] = new double[17770];
        weights3[i] = new double[17770];
        weights4[i] = new double[17770];
        weights5[i] = new double[17770];
    }

    // Create a normal distribution of mean 0, stdev 0.01
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> d(0,0.01);
    srand (static_cast <unsigned> (time(0)));
    // Initialize vector of hidden biases
    hiddenBias = new double[nFeatures];
    h0 = new unsigned int[nFeatures];
    h0probs = new double[nFeatures];
    h1 = new double[nFeatures];
    h0_old = new double[nFeatures];
    h1_old = new double[nFeatures];
    // Initialize each entry of the weight matrices from the normal distribution
    for (unsigned int f = 0; f < nFeatures; ++f)
    {
        hiddenBias[f] = 0;
        h0[f] = 0;
        h0probs[f] = 0;
        h1[f] = 0;
        h0_old[f] = 0;
        h1_old[f] = 0;
        for (unsigned int m = 0; m < 17770; ++m)
        {
            weights1[f][m] = d(gen);
            weights2[f][m] = d(gen);
            weights3[f][m] = d(gen);
            weights4[f][m] = d(gen);
            weights5[f][m] = d(gen);
        }
    }
    // Biases of the visible inputs (the movies)
    visibleBias = new double*[5];
    V0 = new unsigned int*[5];
    V1 = new unsigned int*[5];
    V0_old = new unsigned int*[5];
    V1_old = new unsigned int*[5];
    for (unsigned int i = 0; i < 5; ++i)
    {
        visibleBias[i] = new double[17770];
        V0[i] = new unsigned int[17770];
        V1[i] = new unsigned int[17770];
        V0_old[i] = new unsigned int[17770];
        V1_old[i] = new unsigned int[17770];
    }
    // Initialize visibleBias[i][j] = log(# of times movie j rated i / # of times movie j rated total)

    for (unsigned int m = 1; m < 17771; ++m)
    {
        std::vector<unsigned int> ratings = knn->movieRatings[m];
        for (unsigned int k = 0; k < 5; ++k)
        {
            unsigned int total = ratings[0] + ratings[1] + ratings[2] + ratings[3] + ratings[4];
            visibleBias[k][m-1] = log(double((ratings[k] + 1.0) / (total + 5.0))); // add 1/5 ratings just in case the true % is 0 from small sample size
            V0[k][m-1] = 0;
            V1[k][m-1] = 0;
            //V0_old[k][m-1] = 0; //MOMENTUM
            //V1_old[k][m-1] = 0; //MOMENTUM
        }
    }

    //l = new LoadData2();
    clock_t end1 = clock();
    double elapsed2 = double(end1 - begin1) / CLOCKS_PER_SEC;


    std::cout << "time for rbm initialization " << elapsed2 << std::endl;
}

RBM::~RBM()
{
    delete[] weights1;
    delete[] weights2;
    delete[] weights3;
    delete[] weights4;
    delete[] weights5;
    delete[] visibleBias;
    delete[] hiddenBias;
    delete[] V0_old;
    delete[] V1_old;
    delete[] V0;
    delete[] V1;
    delete[] h0;
    delete[] h0probs;
    delete[] h0_old;
    delete[] h1;
    delete[] h1_old;
}

// Accessor for KNN instance
KNN *RBM::getKNN()
{
    return knn;
}

void RBM::updateStep(unsigned int user)
{
    //clock_t time1 = clock();
    KNN *knn = getKNN();
    std::vector<unsigned int> moviesRated = knn->getMoviesOfUser(user);

    /*
    if (user % 10000 == 0)
    {
        std::cout << visibleBias[0][3] << " " << visibleBias[1][3] << " " << visibleBias[2][3] << " " << visibleBias[3][3] << " " << visibleBias[4][3] << " " << std::endl;
        std::cout << weights1[3][3] << " " << weights2[3][3] << weights3[3][3] << " " << weights4[3][3] << " " << weights5[3][3] << std::endl;
        std::cout << hiddenBias[3] << std::endl;
    }
    */

    //clock_t time2 = clock();
    //unsigned int ratings[moviesRated.size()];
    //std::unordered_map<unsigned int, unsigned int> movieRatings = knn->MoviesOfUsers(users);
    //for (auto & m : moviesRated)
    //{
    //    unsigned int rating = movieRatings[m];

    //}
    //unsigned int V0[5][17770];
    // Set all entries of V0 to 0
    for (unsigned int i = 0; i < 5; ++i)
    {
        for (unsigned int j = 0; j < 17770; ++j)
        {
            //V0_old[i][j] = V0[i][j]; //MOMENTUM
            V0[i][j] = 0;
        }
    }
    //clock_t time3 = clock();
    double sum;
    double timeForRating = 0;
    //unsigned int h0[nFeatures]; // hidden units vector for this user
    // Initialization of h0 and also setting appropriate entries of V0 to 1
    for (unsigned int i = 0; i < nFeatures; ++i)
    {
        sum = hiddenBias[i];
        unsigned int rating;
        //h0_old[i] = h0[i]; //MOMENTUM
        for (auto & movie : moviesRated) // movie is 1-indexed
        {
            //clock_t eee = clock();
            rating = knn->getRating(user, movie);
            //clock_t eee2 = clock();

            //timeForRating += double(eee2 - eee) / CLOCKS_PER_SEC;
            V0[rating - 1][movie - 1] = 1;
            if (rating == 3) // the weird order of these loops is to minimize the number of if lookups (3 is most popular rating, 1 least, etc)
              sum += weights3[i][movie - 1];
            else if (rating == 4)
                sum += weights4[i][movie - 1];
            else if (rating == 5)
                sum += weights5[i][movie - 1];
            else if (rating == 2)
                sum += weights2[i][movie - 1];
            else
                sum += weights1[i][movie - 1];
        }
        double prob = sigmoid(sum);
        h0probs[i] = prob;
        double randnum = ((double) rand() / (double)(RAND_MAX));
        //if (user % 10000 == 0)
        //    std::cout << "sum = " << hiddenBias[i] << " " << sum  << std::endl;
        if (prob > randnum)
            h0[i] = 1;
        else
            h0[i] = 0;
    }
    //clock_t time4 = clock();
    // Reconstruction of the visible units
    //unsigned int V1[5][17770];
    double sum1 = 0;
    double sum2 = 0;
    double sum3 = 0;
    double sum4 = 0;
    double sum5 = 0;
    for (unsigned int i = 0; i < 5; ++i)
    {
        for (unsigned int j = 0; j < 17770; ++j)
        {
            //V1_old[i][j] = V1[i][j]; //MOMENTUM
            V1[i][j] = 0;
        }
    }

    for (auto & movie : moviesRated)//(unsigned int j = 0; j < 17770; ++j)
    {
        unsigned int j = movie-1;
        sum1 = visibleBias[0][j];
        sum2 = visibleBias[1][j];
        sum3 = visibleBias[2][j];
        sum4 = visibleBias[3][j];
        sum5 = visibleBias[4][j];
        for (unsigned int f = 0; f < nFeatures; ++f)
        {
            sum1 += h0[f] * weights1[f][j];
            sum2 += h0[f] * weights2[f][j];
            sum3 += h0[f] * weights3[f][j];
            sum4 += h0[f] * weights4[f][j];
            sum5 += h0[f] * weights5[f][j];
        }
        double denom = exp(sum1) + exp(sum2) + exp(sum3) + exp(sum4) + exp(sum5);
        /*
        if (exp(sum1) / denom > ((double) rand() / (double)(RAND_MAX)))
            V1[0][j] = 1;
        else if (exp(sum2) / denom > ((double) rand() / (double)(RAND_MAX)))
            V1[1][j] = 1;
        else if (exp(sum3) / denom > ((double) rand() / (double)(RAND_MAX)))
            V1[2][j] = 1;
        else if (exp(sum4) / denom > ((double) rand() / (double)(RAND_MAX)))
            V1[3][j] = 1;
        else if (exp(sum5) / denom > ((double) rand() / (double)(RAND_MAX)))
            V1[4][j] = 1;
        else
            V1[4][j] = 0;
        */
        // Below code sets one and only one V1[k] on for each j
        double prob1 = exp(sum1) / denom;
        double prob2 = exp(sum2) / denom;
        double prob3 = exp(sum3) / denom;
        double prob4 = exp(sum4) / denom;
        double prob5 = exp(sum5) / denom;
        double tempProb[5] = {prob1, prob2, prob3, prob4, prob5};
        double prob[5] = {prob1, prob2, prob3, prob4, prob5};
        double probSum = 0;
        for (int k = 0; k < 5; k++)
        {
            if (k > 0 && probSum < 1)
                prob[k] = prob[k] * (1.0 / (1.0 - probSum));
            probSum += tempProb[k];
        }

        bool setOn = false;
        for (int k = 0; k < 5; k++)
        {
            if (!setOn && (prob[k] > ((double) rand() / (double)(RAND_MAX)) || k == 4))
            {
                V1[k][j] = 1;
                setOn = true;
            }
        }

    }

    //clock_t time5 = clock();
    //double h1[nFeatures];
    double summ;
    for (unsigned int i = 0; i < nFeatures; ++i) // hidden units based on reconstruction
    {
        summ = hiddenBias[i];// - 1;
        //h1_old[i] = h1[i]; //MOMENTUM
        for (auto & movie : moviesRated)//(unsigned int j = 0; j < 17770; ++j) // movie is 1-indexed
        {
            unsigned int j = movie - 1;
            if (V1[0][j] == 1)
            //sum += weights1[i][j] * V1[0][j] + weights2[i][j] * V1[1][j] + weights3[i][j] * V1[2][j] + weights4[i][j] * V1[3][j] + weights5[i][j] * V1[4][j];
                summ += weights1[i][j];
            else if (V1[1][j] == 1)
                summ += weights2[i][j];
            else if (V1[2][j] == 1)
                summ += weights3[i][j];
            else if (V1[3][j] == 1)
                summ += weights4[i][j];
            else
                summ += weights5[i][j];
        }
        h1[i] = sigmoid(summ);
        //h1[i] = exp(-sum * sum * 0.5) / 2.5066; //sqrt(2*pi) = 2.5066
        //if (user % 10000 == 0)
        //    std::cout << "hh " << hiddenBias[i] << " " << sum << std::endl;
    }
    //clock_t time6 = clock();

    // Update hidden bias and weight vectors
    double epsilon = epsilon0 / (1.0 + (458293 * numEpochs + user) / (3.0 * 458293)); // decay learning rate
    for (unsigned int i = 0; i < nFeatures; ++i)
    {
        //double h0i = h0probs[i];
        unsigned int h0i = h0[i];
        hiddenBias[i] += epsilon * (double(h0i) - h1[i]);// - weightDecay * hiddenBias[i]);// + momentum * (h0_old[i] - h1_old[i]);

        for (auto & movie : moviesRated) //(unsigned int j = 0; j < 17770; ++j)
        {
            unsigned int j = movie - 1;
            // include weight decay and momentum

            if (h0i == 0)
            {
                weights1[i][j] += epsilon * (0 - h1[i] * double(V1[0][j]) - weightDecay * weights1[i][j]); //+ momentum * (h0_old[i] * double(V0_old[0][j]) - h1_old[i] * double(V1_old[0][j]));
                weights2[i][j] += epsilon * (0 - h1[i] * double(V1[1][j]) - weightDecay * weights2[i][j]); //+ momentum * (h0_old[i] * double(V0_old[1][j]) - h1_old[i] * double(V1_old[1][j]));
                weights3[i][j] += epsilon * (0 - h1[i] * double(V1[2][j]) - weightDecay * weights3[i][j]); //+ momentum * (h0_old[i] * double(V0_old[2][j]) - h1_old[i] * double(V1_old[2][j]));
                weights4[i][j] += epsilon * (0 - h1[i] * double(V1[3][j]) - weightDecay * weights4[i][j]); //+ momentum * (h0_old[i] * double(V0_old[3][j]) - h1_old[i] * double(V1_old[3][j]));
                weights5[i][j] += epsilon * (0 - h1[i] * double(V1[4][j]) - weightDecay * weights5[i][j]); //+ momentum * (h0_old[i] * double(V0_old[4][j]) - h1_old[i] * double(V1_old[4][j]));
            }
            else
            {
                weights1[i][j] += epsilon * (double(V0[0][j]) - h1[i] * double(V1[0][j]) - weightDecay * weights1[i][j]); //+ momentum * (h0_old[i] * double(V0_old[0][j]) - h1_old[i] * double(V1_old[0][j]));
                weights2[i][j] += epsilon * (double(V0[1][j]) - h1[i] * double(V1[1][j]) - weightDecay * weights2[i][j]); //+ momentum * (h0_old[i] * double(V0_old[1][j]) - h1_old[i] * double(V1_old[1][j]));
                weights3[i][j] += epsilon * (double(V0[2][j]) - h1[i] * double(V1[2][j]) - weightDecay * weights3[i][j]); //+ momentum * (h0_old[i] * double(V0_old[2][j]) - h1_old[i] * double(V1_old[2][j]));
                weights4[i][j] += epsilon * (double(V0[3][j]) - h1[i] * double(V1[3][j]) - weightDecay * weights4[i][j]); //+ momentum * (h0_old[i] * double(V0_old[3][j]) - h1_old[i] * double(V1_old[3][j]));
                weights5[i][j] += epsilon * (double(V0[4][j]) - h1[i] * double(V1[4][j]) - weightDecay * weights5[i][j]); //+ momentum * (h0_old[i] * double(V0_old[4][j]) - h1_old[i] * double(V1_old[4][j]));
            }

            /*
            weights1[i][j] += epsilon * (h0i * double(V0[0][j]) - h1[i] * double(V1[0][j]) - weightDecay * weights1[i][j]); //+ momentum * (h0_old[i] * double(V0_old[0][j]) - h1_old[i] * double(V1_old[0][j]));
            weights2[i][j] += epsilon * (h0i * double(V0[1][j]) - h1[i] * double(V1[1][j]) - weightDecay * weights2[i][j]); //+ momentum * (h0_old[i] * double(V0_old[1][j]) - h1_old[i] * double(V1_old[1][j]));
            weights3[i][j] += epsilon * (h0i * double(V0[2][j]) - h1[i] * double(V1[2][j]) - weightDecay * weights3[i][j]); //+ momentum * (h0_old[i] * double(V0_old[2][j]) - h1_old[i] * double(V1_old[2][j]));
            weights4[i][j] += epsilon * (h0i * double(V0[3][j]) - h1[i] * double(V1[3][j]) - weightDecay * weights4[i][j]); //+ momentum * (h0_old[i] * double(V0_old[3][j]) - h1_old[i] * double(V1_old[3][j]));
            weights5[i][j] += epsilon * (h0i * double(V0[4][j]) - h1[i] * double(V1[4][j]) - weightDecay * weights5[i][j]); //+ momentum * (h0_old[i] * double(V0_old[4][j]) - h1_old[i] * double(V1_old[4][j]));
            */

        }
    }
    //clock_t time7 = clock();
    // Update visible biases

    for (unsigned int i = 0; i < 5; ++i)
    {
        for (auto & movie : moviesRated) // (unsigned int j = 0; j < 17770; ++j)
        {
            unsigned int j = movie - 1;
            visibleBias[i][j] += epsilon * (double(V0[i][j]) - double(V1[i][j]));// - weightDecay * visibleBias[i][j]);//+ momentum * (double(V0_old[i][j]) - double(V1_old[i][j]));
        }
    }
    /*
    if (user % 10000 == 0)
    {
        for (int i = 0; i < nFeatures; ++i)
            std::cout << double(V0[2][i+1]) << " " << double(V1[2][i+1]) << " " << double(V0[3][i+1]) << " " << double(V1[3][i+1]) << " " << h0probs[i] << " " << h1[i] << " " << hiddenBias[i] << std::endl;

        for (auto & movie : moviesRated) {
            unsigned int rating = knn->getRating(user, movie);
            //std::cout << V0[rating - 1][movie - 1] << " " << V1[rating - 1][movie - 1] << std::endl;
        }*/

    }


    /*
    std::cout << V0[3][3] << " "<< V1[3][3] << std::endl;
    std::cout << epsilon << std::endl;
    std::cout << double(V0[3][3]) - double(V1[3][3]) << std::endl;
    std::cout << epsilon * (V0[3][3] - V1[3][3]) << std::endl;
    std::cout << epsilon * double(V0[3][3] - V1[3][3]) << std::endl; */

    //clock_t time8 = clock();

    /*
    std::cout << "time1 to time2 " << double(time2 - time1) / CLOCKS_PER_SEC << std::endl;
    std::cout << "time2 to time3 " << double(time3 - time2) / CLOCKS_PER_SEC << std::endl;
    std::cout << "time3 to time4 " << double(time4 - time3) / CLOCKS_PER_SEC << std::endl;
    std::cout << "time to get rating " << timeForRating << std::endl;
    std::cout << "time4 to time5 " << double(time5 - time4) / CLOCKS_PER_SEC << std::endl;
    //std::cout << "time44 to time5 " << double(time5 - time44) / CLOCKS_PER_SEC << std::endl;
    std::cout << "time5 to time6 " << double(time6 - time5) / CLOCKS_PER_SEC << std::endl;
    std::cout << "time6 to time7 " << double(time7 - time6) / CLOCKS_PER_SEC << std::endl;
    std::cout << "time7 to time8 " << double(time8 - time7) / CLOCKS_PER_SEC << std::endl;
    std::cout << "time1 to time8 " << double(time8 - time1) / CLOCKS_PER_SEC << std::endl;

    //std::cout << "time4 to time44 " << double(time44 - time4) / CLOCKS_PER_SEC << std::endl;
    //std::cout << "time44 to time5 " << double(time5 - time44) / CLOCKS_PER_SEC << std::endl;
    */

}

double RBM::predict(unsigned int user, unsigned int movie)
{
    KNN *knn2 = getKNN();
    std::vector<unsigned int> moviesRated = knn2->getMoviesOfUser(user);
    //std::vector<double> pVec;
    //pVec.resize(nFeatures);
    double numerator1 = visibleBias[0][movie - 1]; // numerator to predict prob of rating 1;
    double numerator2 = visibleBias[1][movie - 1]; // numerator to predict prob of rating 2;
    double numerator3 = visibleBias[2][movie - 1]; // numerator to predict prob of rating 3;
    double numerator4 = visibleBias[3][movie - 1]; // numerator to predict prob of rating 4;
    double numerator5 = visibleBias[4][movie - 1]; // numerator to predict prob of rating 5;
    //std::cout << numerator1 << " " << " " << numerator2 << " " << numerator3 << " " << numerator4 << " " << numerator5 << " " << std::endl;
    for (unsigned int j = 0; j < nFeatures; ++j)
    {
        double pj = hiddenBias[j];
        for (auto & m : moviesRated)
        {
            unsigned int rating = knn->getRating(user, m);
            if (rating == 1)
                pj += weights1[j][m - 1];
            else if (rating == 2)
                pj += weights2[j][m - 1];
            else if (rating == 3)
                pj += weights3[j][m - 1];
            else if (rating == 4)
                pj += weights4[j][m - 1];
            else
                pj += weights5[j][m - 1];

        }
        pj = sigmoid(pj);
        numerator1 += pj * weights1[j][movie - 1];
        numerator2 += pj * weights2[j][movie - 1];
        numerator3 += pj * weights3[j][movie - 1];
        numerator4 += pj * weights4[j][movie - 1];
        numerator5 += pj * weights5[j][movie - 1];
        //pVec[j] = pj;
        //std::cout << pj << std::endl;
    }
    //std::cout << "pj[3] = " << pVec[3] << std::endl;

    double denominator = 0;
    /*
    double tmp = visibleBias[0][movie - 1];
    for (unsigned int j = 0; j < nFeatures; ++j)
    {
        tmp += pVec[j] * weights1[j][movie - 1];
    }
    denominator += exp(tmp);
    //std::cout << denominator <<std::endl;
    tmp = visibleBias[1][movie - 1];
    for (unsigned int j = 0; j < nFeatures; ++j)
    {
        tmp += pVec[j] * weights2[j][movie - 1];
    }
    denominator += exp(tmp);
    //std::cout << denominator <<std::endl;
    tmp = visibleBias[2][movie - 1];
    for (unsigned int j = 0; j < nFeatures; ++j)
    {
        tmp += pVec[j] * weights3[j][movie - 1];
    }
    denominator += exp(tmp);
    //std::cout << denominator <<std::endl;
    tmp = visibleBias[3][movie - 1];
    for (unsigned int j = 0; j < nFeatures; ++j)
    {
        tmp += pVec[j] * weights4[j][movie - 1];
    }
    denominator += exp(tmp);
    //std::cout << denominator <<std::endl;
    tmp = visibleBias[4][movie - 1];
    for (unsigned int j = 0; j < nFeatures; ++j)
    {
        tmp += pVec[j] * weights5[j][movie - 1];
    }
    denominator += exp(tmp);

    std::cout << denominator <<std::endl;
    */
    denominator = exp(numerator1) + exp(numerator2) + exp(numerator3) + exp(numerator4) + exp(numerator5);
/*
    std::cout << denominator << std::endl;
    std::cout << "sum of exp(numerators) = " << exp(numerator1) + exp(numerator2) + exp(numerator3) + exp(numerator4) + exp(numerator5) << std::endl;
    std::cout << exp(numerator1) << " " << " " << exp(numerator2) << " " << exp(numerator3) << " " << exp(numerator4) << " " << exp(numerator5) << " " << denominator << std::endl;
    std::cout << exp(numerator1) / denominator << " " << " " << exp(numerator2) / denominator << " " << exp(numerator3) / denominator << " " << exp(numerator4) / denominator << " " << exp(numerator5) / denominator << " " << denominator << std::endl;
    std::cout << "---" << std::endl;
*/
    return (1 * exp(numerator1) + 2 * exp(numerator2) + 3 * exp(numerator3) + 4 * exp(numerator4) + 5 * exp(numerator5)) / denominator;

}

// Find the test error on the probe data set.
double RBM::find_error() {
    ofstream myfile1;
    probe = 0;
    probe = new double*[3];
    probe[0] = new double[1374739];
    probe[1] = new double[1374739];
    probe[2] = new double[1374739];
    probe = LoadData2::probe();
    double error = 0;
    std::string s = "probe_output.txt";
    myfile1.open(s);
    // Go through all the columns of probe
    // for (unsigned int i = 0; i < 16; ++i)
    for (unsigned int i = 0; i < 1374739; ++i)
    {
       // std::cout << probe[0][i] << " probe" << std::endl;
        unsigned int user = probe[0][i];
        unsigned int movie = probe[1][i];
        double rating = probe[2][i];

        //double predicted = user_vec[user] - global_mean + movie_vec[movie];
        //double predicted = 0;
        double predicted = predict(user, movie);
        if (predicted < 1)
            predicted = 1;
        else if (predicted > 5)
            predicted = 5;
        error += (rating - predicted) * (rating - predicted);
        myfile1 << predicted << "\n";
    }

    // Scale by the number of reviews
    myfile1.close();
    return sqrt(error / 1374739);
    //return error / 16;

}

void RBM::create_file()
{
    ofstream myfile1;
    std::string s = "rbm_40features_10epochs_0-01epsdecay.txt";
    myfile1.open(s);
    double **qual = 0;
    qual = new double *[2];
    qual[0] = new double[2749898];
    qual[1] = new double[2749898];
    qual = LoadData2::qual();
    cout << "Creating output file" << endl;
    int c = 0;

    for (unsigned int i = 0; i < 2749898; ++i)
    {
        unsigned int user = qual[0][i];
        unsigned int movie = qual[1][i];
        //double predicted = user_vec[user] - global_mean + movie_vec[movie];
        //double predicted = 0;
        double predicted = predict(user, movie);
        c += 1;

        if (predicted > 5)
            predicted = 5;
        else if (predicted < 1)
            predicted = 1;

        myfile1 << predicted << "\n";

    }
    myfile1.close();
}


double RBM::sigmoid(double x)
{
    return 1.0 / (1 + exp(-x));
}

void RBM::run_rbm()
{
    clock_t begin1 = clock();

    double old_error = 10000;
    double new_error;
    for (unsigned int epoch = 0; epoch < 10; ++epoch)
    {
        clock_t beginEpoch = clock();
        for (unsigned int user = 0; user < 458293; ++user) //458293
        {
            updateStep(user + 1);
            if (user % 100000 == 0)
                std::cout << user << std::endl;

        }
        /*
        for (unsigned int user = 1; user < 10; ++user)
        {
            std::cout << predict(user, 1) << std::endl;
            std::cout << "__####DDD___" << std::endl;
            std::cout << predict(1, user) << std::endl;
        }
        */

        numEpochs++;
        clock_t endEpoch = clock();
        std::cout << "epoch " << epoch << "took " << double(endEpoch - beginEpoch) / CLOCKS_PER_SEC / 60.0 << " min" << std::endl;
        /*
        new_error = find_error();
        std::cout << "epoch " << epoch << " has probe rmse = " << new_error << std::endl;
        std::cout << "old error: " << old_error << std::endl;
        std::cout << "new error: " << new_error << std::endl;

        clock_t endProbe = clock();
        std::cout << "time to get probe rmse in min " << double(endProbe - endEpoch) / CLOCKS_PER_SEC / 60.0 << std::endl;
        if (new_error + .001 >= old_error && epoch > 5)
        {
            break;
        }
        old_error = new_error;

        for (unsigned int user = 1; user < 10; ++user)
            std::cout << predict(user, 1) << std::endl;
        */

    }
    clock_t end1 = clock();
    double elapsed2 = double(end1 - begin1) / CLOCKS_PER_SEC;
    std::cout << "time to run in min " << elapsed2 / 60.0 << std::endl;
    /*
    for (unsigned int user = 1; user < 10; ++user)
    {
        std::cout << rbm.predict(user, 1) << std::endl;
        std::cout << "__####DDD___" << std::endl;
        std::cout << rbm.predict(1, user) << std::endl;
    }
    */
    clock_t end2 = clock();
    std::cout << "probe rmse = " << find_error() << std::endl;

    clock_t end3 = clock();
    std::cout << "writing qual predictions" << std::endl;
    create_file();
    clock_t end4 = clock();
    std::cout << "time to get probe rmse in min " << double(end3 - end2) / CLOCKS_PER_SEC / 60.0 << std::endl;
    std::cout << "time to write qual predictions in min " << double(end4 - end3) / CLOCKS_PER_SEC / 60.0 << std::endl;
}
int main()
{
    RBM rbm = RBM(10, 0.01, 0.001, 0.8);
    rbm.run_rbm();

    // 40 features, 20 epochs, 0.01 epsilon = 1.01932 (-7.14%) rmse on qual
}
