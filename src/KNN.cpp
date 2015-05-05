#include "KNN.h"
#include "LoadData2.h"
#include <typeinfo>
KNN::KNN()
{
    std::cout << "in KNN" << std::endl;
    l = new LoadData2();
    //data = l->loadRatingsVector();
    //sp_data = l->loadSparseData();
    std::cout << "loaded knn data" << std::endl;
}

KNN::KNN(double** m)
{
    data = m;
}

KNN::~KNN()
{
    delete l;
}

// Accessor for LoadData instance
LoadData2 *KNN::getLD()
{
    return l;
}

/*
 * Normalizes the movie ratings of a given user
 * normRating = (rating - avg rating) / std deviation(ratings)
 */
std::vector<double> KNN::normalizeUser(unsigned int user)
{
    LoadData2 *l2 = getLD();
    arma::sp_mat sp_data = l2->loadSparseData();
    std::vector<double> normedRatings = {};
    double mean = l2->getUserMean(user);
    std::cout << "mean is " << mean << std::endl;
    double stddev = l2->getUserStddev(user);
    //arma::SpSubview<double> userRatings = sp_data.row(user);
    for (arma::sp_mat::const_row_iterator it = sp_data.begin_row(user); it != sp_data.end_row(user); ++it)
    {
        normedRatings.push_back((sp_data.at(user, it.col())- mean) / stddev);
    }
    return normedRatings;
}

std::vector<double> KNN::normalizeMovie(unsigned int movie)
{
    LoadData2 *l2 = getLD();
    arma::sp_mat sp_data = l2->loadSparseData();
    std::vector<double> normedRatings = {};
    double mean = l2->getMovieMean(movie);
    double stddev = l2->getMovieStddev(movie);
    //arma::SpSubview<double> movieRatings = sp_data.col(movie);
    //for (unsigned int i = 0; i < movieRatings.size(); i++)
    for (arma::sp_mat::const_iterator it = sp_data.begin_col(movie); it != sp_data.end_col(movie); ++it)
    {
        normedRatings.push_back((sp_data.at(it.row(), movie) - mean) / stddev);
    }
    return normedRatings;
}

// Returns a vector of movie indices that the user rated
std::vector<unsigned int> KNN::getMoviesOfUser(unsigned int user)
{
    LoadData2 *l2 = getLD();
    arma::sp_mat sp_data = l2->loadSparseData();
    std::vector<unsigned int> moviesVec;
    //PearsonIntermediate corrArr[17770]; // 17770 movies
    for(arma::sp_mat::row_iterator it = sp_data.begin_row(user); it != sp_data.end_row(user); ++it)
    {
        //std::cout << it.row() << ", " << it.col() << std::endl;
        double idx = it.col();
        unsigned int UIidx = static_cast<unsigned int>(idx + 0.5);
        moviesVec.push_back(UIidx);
    }
    return moviesVec;
}

// Returns a vector of user indices that rated the movie
std::vector<unsigned int> KNN::getUsersOfMovie(unsigned int movie)
{
    LoadData2 *l2 = getLD();
    arma::sp_mat sp_data = l2->loadSparseData();
    std::vector<unsigned int> usersVec;
    //PearsonIntermediate corrArr[17770]; // 17770 movies
    for (arma::sp_mat::const_iterator it = sp_data.begin_col(movie); it != sp_data.end_col(movie); ++it)
    {
        double idx = it.row();
        unsigned int UIidx = static_cast<unsigned int>(idx + 0.5);
        usersVec.push_back(UIidx);
    }
    return usersVec;
}

double KNN::calculatePearson(PearsonIntermediate pi)
{
    return ((pi.xy - pi.x * pi.y) / pi.cnt) / (sqrt(pi.xx / pi.cnt - (pi.x / pi.cnt) * (pi.x / pi.cnt)) * sqrt(pi.yy / pi.cnt - (pi.y / pi.cnt) * (pi.y / pi.cnt)));
}

double *KNN::getCorrelations(unsigned int movie)
{
    LoadData2 *l2 = getLD();
    arma::sp_mat sp_data = l2->loadSparseData();
    PearsonIntermediate *corrArr = new PearsonIntermediate[17770];
    double allCorrelations[17770];

    std::vector<unsigned int> userVec = getUsersOfMovie(movie); // all users who rated this movie
    for (std::vector<unsigned int>::iterator it = userVec.begin(); it != userVec.end(); ++it)
    {
        unsigned int userIdx = *it;
        std::vector<unsigned int> movieVec = getMoviesOfUser(userIdx); // all movies rated by this user
        for (std::vector<unsigned int>::iterator it2 = movieVec.begin(); it2 != movieVec.end(); ++it2)
        {
            // update the PearsonIntermediate struct
            unsigned int otherMovie = *it2;
            double movieRating = sp_data.at(userIdx, movie);
            double otherMovieRating = sp_data.at(userIdx, otherMovie);
            PearsonIntermediate currentPI = corrArr[otherMovie];
            double x = currentPI.x + movieRating;
            double y = currentPI.y + otherMovieRating;
            double xy = currentPI.xy + movieRating * otherMovieRating;
            double xx = currentPI.xx + movieRating * movieRating;
            double yy = currentPI.yy + otherMovieRating * otherMovieRating;
            unsigned int cnt = currentPI.cnt + 1;
            PearsonIntermediate newPI;
            newPI.x = x;
            newPI.y = y;
            newPI.xy = xy;
            newPI.xx = xx;
            newPI.yy = yy;
            newPI.cnt = cnt;
            corrArr[otherMovie] = newPI;
        }

    }
    for (unsigned int i = 0; i < 17770; ++i)
    {
        double corr = calculatePearson(corrArr[i]);
        allCorrelations[i] = corr;
    }

    return allCorrelations;
}

int main()
{
    KNN k = KNN();
    LoadData2 *l2 = k.getLD();
    //std::vector<double> moviesVec = k.getMoviesOfUser(1);
    std::vector<double> normU = k.normalizeUser(1);
    std::cout << "first = " << normU[0] << std::endl;
    std::cout << "second = " << normU[1] << std::endl;
    double *corrArr = k.getCorrelations(1);
    std::cout << "corr with 0 = " << corrArr[0];
    std::cout << "corr with 1 = " << corrArr[1];
    std::cout << "corr with 2 = " << corrArr[2];
    std::cout << "corr with 3 = " << corrArr[3];
    //arma::sp_mat ddsp = l2->loadSparseData();
    //std::cout << "sparse mat has nrows = " << ddsp.n_rows << std::endl;
    //std::cout << "sparse mat has ncols = " << ddsp.n_cols << std::endl;

    /*
    LoadData *l2 = k.getLD();
    //arma::vec kn = k.normalize(1);
    std::cout << l2->getUserMean(1) << std::endl;
    std::cout << l2->getUserStddev(1) << std::endl;
    std::cout << l2->getMovieMean(1) << std::endl;
    std::cout << l2->getMovieStddev(1) << std::endl;
    //std::cout << kn << std::endl;
    */
    /*
    std::vector<unsigned int> rating = k.normalizeUser(1);
    std::cout << rating.size() << rating[0] << rating[1] << std::endl;
    //26142949672944294967295
    */
    return 0;
}

