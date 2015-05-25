#ifndef RBM_H
#define RBM_H
#include <armadillo>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <math.h>
#define ARMA_64BIT_WORD
class KNN;
class LoadData2;

class RBM
{
    public:
        RBM(unsigned int nFeatures, double eps, double wd, double mom);
        virtual ~RBM();
        double **weights1;
        double **weights2;
        double **weights3;
        double **weights4;
        double **weights5;
        double *hiddenBias;
        double **visibleBias;
        unsigned int **V0;
        unsigned int **V1;
        unsigned int **V0_old;
        unsigned int **V1_old;
        unsigned int *h0;
        double *h0probs;
        double *h1;
        double *h0_old;
        double *h1_old;
        KNN *getKNN();
        void updateStep(unsigned int user);
        double predict(unsigned int user, unsigned int movie);
        double find_error();
        void create_file();
        double sigmoid(double x);
        void run_rbm();
    protected:
    private:
        KNN *knn;
        LoadData2 *l;
        double **probe;
        unsigned int nFeatures;
        double epsilon0;
        double weightDecay;
        double momentum;
        int numEpochs;

};

#endif // RBM_H
