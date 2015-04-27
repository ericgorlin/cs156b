#ifndef KNN_H
#define KNN_H
#include <armadillo>
#define ARMA_64BIT_WORD
class LoadData;

class KNN
{
    public:
        KNN();
        KNN(arma::umat m);
        arma::vec normalize(unsigned int user);
        virtual ~KNN();
        LoadData *getLD();
    protected:
    private:
        arma::umat sp_data;
        LoadData *l;
};

#endif // KNN_H
