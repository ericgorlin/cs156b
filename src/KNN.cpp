#include "KNN.h"
#include "LoadData.h"
#include <typeinfo>
KNN::KNN()
{
    l = new LoadData();
    sp_data = l->loadRatingsVector();
}

KNN::KNN(arma::umat m)
{
    sp_data = m;
}

KNN::~KNN()
{
    delete l;
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

/*
int main()
{
    KNN k = KNN();
    LoadData *l2 = k.getLD();
    arma::vec kn = k.normalize(1);
    std::cout << l2->getUserMean(1) << std::endl;
    std::cout << l2->getUserStddev(1) << std::endl;
    std::cout << l2->getMovieMean(1) << std::endl;
    std::cout << l2->getMovieStddev(1) << std::endl;
    //std::cout << kn << std::endl;
    return 0;
}
*/

