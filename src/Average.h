#ifndef AVERAGE_H
#define AVERAGE_H
#include <armadillo>

class Average
{

    public:
        Average();
        Average(arma::sp_mat m);
        arma::vec getUserAverages();
        arma::vec getMovieAverages();
        virtual ~Average();
    protected:
    private:
        arma::sp_mat sp_data;
};

#endif // AVERAGE_H
