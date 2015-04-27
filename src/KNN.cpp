#include "KNN.h"
#include "LoadData.h"
#include <typeinfo>
KNN::KNN()
{
    LoadData l = LoadData();
    std::cout << "load data object created" << std::endl;
    sp_data = l.loadRatingsVector();
    std::cout << "we loaded the ratings" << std::endl;
}

KNN::KNN(arma::umat m)
{
    sp_data = m;
}

KNN::~KNN()
{
    //dtor
}

// Accessor for LoadData instance
LoadData *KNN::getLD()
{
    return l;
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
    KNN k = KNN();
    LoadData *l = k.getLD();
    std::cout << "what is goin on" << std::endl;
    arma::vec kn = k.normalize(1);
    std::cout << l->getUserMean(1) << std::endl;
    std::cout << l->getUserStddev(1) << std::endl;
    std::cout << l->getMovieMean(1) << std::endl;
    std::cout << l->getMovieStddev(1) << std::endl;
    std::cout << kn << std::endl;
    return 0;
}

