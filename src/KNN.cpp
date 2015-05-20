#include "KNN.h"
#include "LoadData2.h"
#include <typeinfo>
KNN::KNN()
{
    std::cout << "in KNN" << std::endl;
    l = new LoadData2();
    //data = l->loadRatingsVector();
    //sp_data = l->loadSparseData();

    std::string line;
    std::ifstream myfile("src/moviesOfUsers.txt");
    std::cout << myfile.is_open() << " opened moviesOfUsers" << std::endl;
    int c = 0;
    if (myfile.is_open())
    {
        while (getline(myfile, line))
        {
            std::istringstream iss(line);
            int key;
            int number;
            int rating;
            //std::vector<unsigned int> vec;
            //std::vector<unsigned int> vecInner;
            std::unordered_map<unsigned int, unsigned int> uMap;
            uMap.clear();
            iss >> key;
            while (iss >> number)
            {
                iss >> rating;
                //vecInner.clear();
                //vecInner.push_back(number);
                //vecInner.push_back(rating);
                //vec.push_back(vecInner);
                uMap[number] = rating;
            }
            moviesOfUsers[key] = uMap;
            if (c % 10000 == 0)
                std::cout << c << std::endl;
            c++;
        }
    }
    myfile.close();
    std::cout << c << std::endl;
    std::cout << "done with file read" << std::endl;
    std::cout << moviesOfUsers.size() << std::endl;
    std::unordered_map<unsigned int, unsigned int> vec2 = moviesOfUsers[458032];
    std::cout << vec2.size() << std::endl;

    std::string line2;
    std::ifstream myfile2("src/usersOfMovies.txt");
    std::cout << myfile2.is_open() << " open" << std::endl;
    c = 0;
    if (myfile2.is_open())
    {
        while (getline(myfile2, line2))
        {
            std::istringstream iss2(line2);
            int key2;
            unsigned int number2;
            unsigned int rating2;
            //std::vector<unsigned int> vec2;
            //std::vector<unsigned int> vec2Inner;
            std::unordered_map<unsigned int, unsigned int> mMap;
            mMap.clear();
            iss2 >> key2;
            while (iss2 >> number2)
            {
                iss2 >> rating2;
                //vec2Inner.clear();
                //vec2Inner.push_back(number2);
                //vec2Inner.push_back(rating2);
                //vec2.push_back(vec2Inner);
                mMap[number2] = rating2;
            }
            usersOfMovies[key2] = mMap;
            if (c % 1000 == 0)
                std::cout << c << std::endl;
            c++;
        }
    }
    myfile2.close();
    std::cout << c << std::endl;
    std::cout << "done with file read" << std::endl;
    std::cout << usersOfMovies.size() << std::endl;
    std::unordered_map<unsigned int, unsigned int> vec3 = usersOfMovies[1];
    std::cout << vec3.size() << std::endl;
    std::cout << "loaded knn data" << std::endl;
}

KNN::KNN(double** m)
{
    data = m;
}

KNN::~KNN()
{
    delete l;
}

// Accessor for LoadData instance
LoadData2 *KNN::getLD()
{
    return l;
}

unsigned int KNN::getRating(unsigned int user, unsigned int movie)
{
    //std::unordered_map<unsigned int, unsigned int> moviesOfUser = moviesOfUsers[user];
    //return moviesOfUser[movie];
    return moviesOfUsers[user][movie];
}

/*
 * See http://algorithmsanalyzed.blogspot.com/2008/05/bellkor-algorithm-global-effects.html for info about global effects
 */
double KNN::getMovieEffect(unsigned int movie)
{
    LoadData2 *l2 = getLD();
    std::unordered_map<int, vector<double>> movieMap = l2->getMovieMap();
    double globalMean = l2->getGlobalMean();
    double movieEffect = (movieMap[movie - 1][1] - globalMean) / (movieMap[movie - 1][0] + 25);
    return movieEffect;
}

double KNN::getUserEffect(unsigned int user)
{
    LoadData2 *l2 = getLD();
    double globalMean = l2->getGlobalMean();
    std::unordered_map<int, vector<double>> userMap = l2->getUserMap();
    std::unordered_map<unsigned int, unsigned int> moviesOfUser = moviesOfUsers[user];
    double sumResiduals2 = 0;
    for (auto kv : moviesOfUser)
    {
            unsigned int movie = kv.first;
            unsigned int rating = kv.second;
            sumResiduals2 += (rating - globalMean - getMovieEffect(movie));
    }
    return sumResiduals2 / (userMap[user - 1][0] + 7);
}

double KNN::getGlobalEffects(unsigned int user, unsigned int movie)
{
    LoadData2 *l2 = getLD();
    //std::unordered_map<int, vector<double>> userMap = l2->getUserMap();
    //std::unordered_map<int, vector<double>> movieMap = l2->getMovieMap();
    //double actualRating = getRating(user, movie)
    double globalMean = l2->getGlobalMean();
    double movieEffect = getMovieEffect(movie);
    double userEffect = getUserEffect(user);
    return globalMean + movieEffect + userEffect;
}

/*
 * Normalizes the movie ratings of a given user
 * normRating = (rating - avg rating) / std deviation(ratings)
 *
std::vector<double> KNN::normalizeUser(unsigned int user)
{
    LoadData2 *l2 = getLD();
    arma::sp_mat sp_data = l2->loadSparseData();
    std::vector<double> normedRatings = {};
    double mean = l2->getUserMean(user); // change this to getBetterUserMean
    std::cout << "mean is " << mean << std::endl;
    double stddev = l2->getUserStddev(user);
    //arma::SpSubview<double> userRatings = sp_data.row(user);
    for (arma::sp_mat::const_row_iterator it = sp_data.begin_row(user); it != sp_data.end_row(user); ++it)
    {
        normedRatings.push_back((sp_data.at(user, it.col())- mean) / stddev);
    }
    return normedRatings;
}

std::vector<double> KNN::normalizeMovie(unsigned int movie)
{
    LoadData2 *l2 = getLD();
    arma::sp_mat sp_data = l2->loadSparseData();
    std::vector<double> normedRatings = {};
    double mean = l2->getMovieMean(movie);
    double stddev = l2->getMovieStddev(movie);
    //arma::SpSubview<double> movieRatings = sp_data.col(movie);
    //for (unsigned int i = 0; i < movieRatings.size(); i++)
    for (arma::sp_mat::const_iterator it = sp_data.begin_col(movie); it != sp_data.end_col(movie); ++it)
    {
        normedRatings.push_back((sp_data.at(it.row(), movie) - mean) / stddev);
    }
    return normedRatings;
}
*/

// Returns a vector of movie indices that the user rated
std::vector<unsigned int> KNN::getMoviesOfUser(unsigned int user)
{
    //LoadData2 *l2 = getLD();
    //arma::sp_mat sp_data = l2->loadSparseData();
    std::vector<unsigned int> moviesVec;
    //std::unordered_map<unsigned int, unsigned int> moviesOfUser = moviesOfUsers[user];
    for (auto kv : moviesOfUsers[user])
    {
            //unsigned int movie = kv.first;
            moviesVec.push_back(kv.first);
    }
    return moviesVec;
}

// Returns a vector of user indices that rated the movie
std::vector<unsigned int> KNN::getUsersOfMovie(unsigned int movie)
{
    //LoadData2 *l2 = getLD();
    //arma::sp_mat sp_data = l2->loadSparseData();
    std::vector<unsigned int> usersVec;
    //std::unordered_map<unsigned int, unsigned int> usersOfMovie = usersOfMovies[movie];
    for (auto kv : usersOfMovies[movie])
    {
            //unsigned int user = kv.first;
            usersVec.push_back(kv.first);
    }
    return usersVec;
}

unsigned short KNN::calculatePearson(PearsonIntermediate pi)
{
    double val = 1.0 * (pi.cnt * pi.xy - pi.x * pi.y) / (sqrt(pi.xx * pi.cnt - pi.x * pi.x) * sqrt(pi.yy * pi.cnt - pi.y * pi.y));
    if (std::isnan(val))
    {
        return 0;
    }
    else
    {
        double val2 = val * pi.cnt / (pi.cnt + 10); // bayesian regression
        // Store correlations as unsigned __int8 between 0 - 255 and ignore negative correlations to save memory
        if (val2 < 0)
            return 0;
        else
            return static_cast<unsigned short> (val2 * 65535 + 0.5);
    }

}

std::vector<unsigned short> KNN::getCorrelations(unsigned int movie)
{
    //clock_t begin1 = clock();
    //PearsonIntermediate *corrArr = new PearsonIntermediate[17770];
    PearsonIntermediate corrArr[17770];
    std::vector<unsigned short> allCorrelations;
    allCorrelations.reserve(17770);
    //clock_t begin2 = clock();
    //double elapsed_min1 = double(begin2 - begin1) / CLOCKS_PER_SEC;
    //std::cout << "time to load data " << elapsed_min1 / CLOCKS_PER_SEC << std::endl;
    //clock_t begin21 = clock();
    //std::vector<unsigned int> userVec = getUsersOfMovie(movie); // all users who rated this movie
    //clock_t begin22 = clock();
    //double elapsed_min2 = double(begin22 - begin21) / CLOCKS_PER_SEC;
    //std::cout << "time to get users of movie " << elapsed_min2 << std::endl;
    //double totalGetMoviesTime = 0;
    //double totalInnerLoopMinusSparse = 0;
    //double totalGetMovie = 0;
    //double totalGetOther = 0;
    for (auto & userIdx : getUsersOfMovie(movie))
    {
        //unsigned int userIdx = *it;
        //clock_t begin221 = clock();
        //std::vector<unsigned int> movieVec = getMoviesOfUser(userIdx); // all movies rated by this user
        //clock_t begin222 = clock();
        //double elapsed_min22 = double(begin222 - begin221) / CLOCKS_PER_SEC;
        //totalGetMoviesTime += elapsed_min22;
        for (auto & otherMovie : getMoviesOfUser(userIdx)) //std::vector<unsigned int>::iterator it2 = movieVec.begin(); it2 != movieVec.end(); ++it2)
        {
            // update the PearsonIntermediate struct
            unsigned int movieRating = getRating(userIdx, movie);
            //clock_t begin333 = clock();
            unsigned int otherMovieRating = getRating(userIdx, otherMovie);
            //clock_t begin3333 = clock();
            PearsonIntermediate currentPI = corrArr[otherMovie - 1];
            PearsonIntermediate newPI;
            newPI.x = currentPI.x + movieRating;
            newPI.y = currentPI.y + otherMovieRating;
            newPI.xy = currentPI.xy + movieRating * otherMovieRating;
            newPI.xx = currentPI.xx + movieRating * movieRating;
            newPI.yy = currentPI.yy + otherMovieRating * otherMovieRating;
            newPI.cnt = currentPI.cnt + 1;

            corrArr[otherMovie - 1] = newPI;
            //clock_t begin4 = clock();
            //double elapsed_min3 = double(begin4 - begin3333) / CLOCKS_PER_SEC;
            //double elapsed_min33 = double(begin3333 - begin333) / CLOCKS_PER_SEC;
            //double elapsed_min333 = double(begin333 - begin3) / CLOCKS_PER_SEC;
            //totalInnerLoopMinusSparse += elapsed_min3;
            //totalGetMovie += elapsed_min333;
            //totalGetOther += elapsed_min33;
        }

    }
    //clock_t end1 = clock();
    //double elapsed_total = double(end1 - begin1) / CLOCKS_PER_SEC;
    //std::cout << "total movie rating time " << totalGetMovie << std::endl;
    //std::cout << "total other movie rating time " << totalGetOther << std::endl;
    //std::cout << "inner loop without sparse took " << totalInnerLoopMinusSparse << std::endl;
    //std::cout << "total time to make structs " << elapsed_total << std::endl;
    //clock_t begin5 = clock();
    for (unsigned int i = 0; i < 17770; ++i)
    {
        //uint8_t corr = ;
        allCorrelations[i] = calculatePearson(corrArr[i]);
    }
    //clock_t end2 = clock();
    //double elapsed_min3 = double(end2 - begin5) / CLOCKS_PER_SEC;
    //std::cout << "seconds for correlations loop: " << elapsed_min3 << std::endl;

    return allCorrelations;
}

int main()
{
    KNN k = KNN();
    //LoadData2 *l2 = k.getLD();

    clock_t t1 = clock();
    std::vector<unsigned short> corrArr1 = k.getCorrelations(1);
    clock_t t2 = clock();
    std::vector<unsigned short> corrArr2 = k.getCorrelations(2);
    clock_t t3 = clock();
    double elapsed1 = double(t2 - t1) / CLOCKS_PER_SEC;
    double elapsed2 = double(t3 - t2) / CLOCKS_PER_SEC;
    std::cout << "movie 1 " << elapsed1 << std::endl;
    std::cout << "movie 2 " << elapsed2 << std::endl;
    std::cout << "size of vector1 is " << corrArr1.size() << std::endl;
    std::cout << "size of vector2 is " << corrArr2.size() << std::endl;
    std::cout << (static_cast<double> (corrArr1[0])) / 65535.0 << " " << (static_cast<double> (corrArr1[1])) / 65535.0 << " " << (static_cast<double> (corrArr1[2])) / 65535.0 << std::endl;
    std::cout << (static_cast<double> (corrArr2[0])) / 65535.0 << " " << (static_cast<double> (corrArr2[1])) / 65535.0 << " " << (static_cast<double> (corrArr2[2])) / 65535.0 << std::endl;
    std::cout << "writing to file now" << std::endl;
    ofstream outfile;
    outfile.open("src/movieCorrelationMatrix5.txt");
    for (unsigned int i = 0; i < 17770; ++i)
    {
        std::vector<unsigned short> corrArr = k.getCorrelations(i + 1);
        outfile << i + 1 << " "; // write movie index
        for (unsigned int j = 0; j < 17770; ++j)
        {
            outfile << corrArr[j] << " "; // write correlations
        }
        outfile << "\n";
        if (i % 1000 == 0)
        {
            std::cout << i << std::endl;
        }
    }
    outfile.close();

    return 0;
}

