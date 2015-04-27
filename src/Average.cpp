#include "Average.h"
#include "LoadData.h"

Average::Average()
{
    sp_data = LoadData::start();
}

Average::Average(arma::sp_mat m)
{
    sp_data = m;
}

arma::vec Average::getUserAverages()
{
    std::cout << "in averages now" << std::endl;
    std::cout << sp_data.n_rows << std::endl;
    std::cout << sp_data.n_cols << std::endl;
    time_t start = std::time(0);
    //arma::sp_mat userAvg = arma::mean(sp_data, 1); //userAvg(sp_data.n_rows);
    //userAvg = arma::mean(sp_data, 1);
    arma::vec userAvg(100);//sp_data.n_rows);
    for (int i=0; i < 100; i++) //sp_data.n_rows; i++)
    {
        userAvg.at(i) = mean(mean(sp_data.row(i)));
    }
    // test out first few
//    std::cout << userAvg(0) << endl;
//    std::cout << userAvg(1) << endl;
    std::cout << userAvg(0) << std::endl;
    std::cout << userAvg(1) << std::endl;
    time_t send = std::time(0);
    std::cout << difftime(send, start) * 1000.0 << std::endl;
    return userAvg;
}

arma::vec Average::getMovieAverages()
{
    arma::vec movieAvg(sp_data.n_cols);
    for (unsigned int j=0; j < sp_data.n_cols; j++)
    {
        movieAvg(j) = mean(mean(sp_data.col(j)));
    }
    // test out
 //   std::cout << movieAvg(0) << endl;
 //   std::cout << movieAvg(1) << endl;
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
    //av.getMovieAverages();
    return 0;
}
*/

