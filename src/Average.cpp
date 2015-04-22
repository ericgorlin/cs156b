#include "Average.h"
#include "LoadData.h"

Average::Average()
{
    arma::sp_mat sp_data = LoadData::start();
}

Average::Average(arma::sp_mat m)
{
    arma::sp_mat sp_data = m;
}

arma::vec Average::getUserAverages()
{
    arma::vec userAvg(sp_data.n_rows);
    for (int i=0; i < sp_data.n_rows; i++)
    {
        userAvg(i) = mean(mean(sp_data.row(i)));
    }
    // test out first few
    std::cout << userAvg(0) << endl;
    std::cout << userAvg(1) << endl;
    return userAvg;
}

arma::vec Average::getMovieAverages()
{
    arma::vec movieAvg(sp_data.n_cols);
    for (int j=0; j < sp_data.n_cols; j++)
    {
        movieAvg(j) = mean(mean(sp_data.col(j)));
    }
    // test out
    std::cout << movieAvg(0) << endl;
    std::cout << movieAvg(1) << endl;
    return movieAvg;
}

Average::~Average()
{
    //dtor
}

/*
int main()
{
    Average av = Average();
    av.getUserAverages();
    av.getMovieAverages();
    return 0;
}
*/
