#ifndef KNN_H
#define KNN_H
#include <armadillo>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <math.h>
#define ARMA_64BIT_WORD
class LoadData2;

class KNN
{
    public:
        KNN();

        KNN(double** m);
        struct PearsonIntermediate
        {
            double x; // sum of ratings for movie X
            double y; // sum of ratings for movie Y
            double xy; // sum of product of ratings for movies X and Y
            double xx; // sum of square of ratings for movie X
            double yy; // sum of square of ratings for movie Y
            unsigned int cnt; // number of users who rated both X and Y

            PearsonIntermediate() // constructor, set all values to 0
            {
                x = 0;
                y = 0;
                xy = 0;
                xx = 0;
                yy = 0;
                cnt = 0;
            }
        };
        unsigned int getRating(unsigned int user, unsigned int movie);
        double getMovieEffect(unsigned int movie);
        double getUserEffect(unsigned int user);
        double getGlobalEffects(unsigned int user, unsigned int movie);
        std::unordered_map<int, std::unordered_map<unsigned int, unsigned int>> usersOfMovies; // {movie1: {user1: rating1, ... }, movie2: {}, ...}
        std::unordered_map<int, std::unordered_map<unsigned int, unsigned int>> moviesOfUsers; // {user1: {movie1: rating1, ... }, user2: {}, ...}
        //std::vector<double> normalizeUser(unsigned int user);
        //std::vector<double> normalizeMovie(unsigned int movie);
        std::vector<unsigned int> getMoviesOfUser(unsigned int user);
        std::vector<unsigned int> getUsersOfMovie(unsigned int movie);
        uint8_t calculatePearson(PearsonIntermediate pi);
        std::vector<uint8_t> getCorrelations(unsigned int movie);
        virtual ~KNN();
        LoadData2 *getLD();
        int getUserMean();

    protected:
    private:
        double** data;
        arma::sp_mat sp_data;
        LoadData2 *l;

};

#endif // KNN_H
