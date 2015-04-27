#ifndef KNN_H
#define KNN_H
#include <armadillo>

class KNN
{
    public:
        KNN();
        KNN(arma::umat m);
        arma::vec normalize(unsigned int user);
        virtual ~KNN();
    protected:
    private:
        arma::umat sp_data;
};

#endif // KNN_H
