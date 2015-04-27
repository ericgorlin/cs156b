#include "KNN.h"
#include "LoadData.h"
#include <typeinfo>

KNN::KNN()
{
    sp_data = LoadData::loadRatingsVector();
}

KNN::KNN(arma::umat m)
{
    sp_data = m;
}

KNN::~KNN()
{
    //dtor
}

/*
 * Normalizes the movie ratings of a given user
 * normRating = (rating - avg rating) / std deviation(ratings)
 */
arma::vec KNN::normalize(unsigned int user)
{
    //arma::vec row = sp_data.row(user);
    //cout << typeid(sp_data.row(user)).name() << endl;
    //cout << typeid(sp_data.col(user)).name() << endl;
    //cout << typeid(sp_data).name() << endl;
    //return sp_data.col(user);// - arma::mean(sp_data.row(user));// / arma::stddev(sp_data.row(user));
    return arma::vec(671);
}

int main()
{
    //LoadData l;
    KNN k = KNN();
    //arma::vec kn = k.normalize(1);
    //std::cout << l.getUserMean(1) << std::endl;
    //std::cout << LoadData::getUserStddev(1) << std::endl;
    //std::cout << LoadData::getMovieMean(1) << std::endl;
    //std::cout << LoadData::getMovieStddev(1) << std::endl;
    //std::cout << kn << std::endl;
    return 0;
}

