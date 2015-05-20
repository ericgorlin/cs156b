#ifndef QUIZBLEND_H
#define QUIZBLEND_H
#include <armadillo>
#include <ifstream>
#include <iostream>
#include <fstream>
#include <ofstream>



class QuizBlend
{
    public:
        QuizBlend();
        virtual ~QuizBlend();
        void run_blend();
    protected:
    private:
        arma::mat ratings;
        double variance;
        std::vector<double> rmse;
        double qual_size;
        double n;
        int num_files;
        double lambda;
        std::string outfile;
        std::string outcoef;
};

#endif // QUIZBLEND_H
