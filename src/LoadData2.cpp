#include "LoadData2.h"

LoadData2::LoadData2()
{
/*
     * userMap = {user: [N, M(N), S(N)]} where M(N) and S(N) are recurrence relations to find the
     * mean and standard devation of continual input:
     * M[0] = x[0], M[k] = M[k-1] + (x[k] - M[k-1]) / k
     * S[0] = 0, S[k] = S[k-1] + (x[k] - M[k-1]) * (x[k] - M[k])
     * mean = M[n], standard dev = sqrt(S[N] / (n - 1))
     */
    //unordered_map<int, array<int, 3>> userMap;
    //unordered_map<int, array<int, 3>> movieMap;
    std::cout << "Loading rating vectors..." << std::endl;

    //std::vector<double> ratings;
    //arma::umat locations = arma::umat(2, 98291669);

    // Training set has 98291669 values
    data = 0;
    data = new double*[3];
    for (unsigned int i = 0; i < 3; i++)
        data[i] = new double[98291669];

    //usersOfMovies.resize(17770);//, std::vector<unsigned int>);
    //moviesOfUsers.resize(458293);//, std::vector<unsigned int>);
    std::cout << "created a vector" << std::endl;
    //arma::umat locations = arma::umat(2, 716);

    //arma::umat locations = arma::umat(2, 716);
    //int n_users = 458293;
    //int n_movies = 17770;
    totalMovies = 0;
    sumRatings = 0;

    // Open the file
    string line;
    //ifstream myfile("../src/um/train.dta");
    ifstream myfile("src/um/train.dta");
    //ifstream myfile("um/all.dta");
    //ifstream myfile("um/shortall.dta");
    std::cout << myfile.is_open() << " open" << std::endl;
    int c = 0;
    if (myfile.is_open())
    {
        while (getline(myfile, line))
        {
            int space1 = line.find(" ");
            int space2 = line.find(" ", space1 + 1);
            int space3 = line.find(" ", space2 + 1);

            // Insert into our temporary data vectors
            int userIdx = atoi(line.substr(0, space1).c_str());
            int movieIdx = atoi(line.substr(space1 + 1, space2).c_str());
            int rating = (atoi(line.substr(space3 + 1).c_str()));

            //locations(0, c) = userIdx - 1;
            //locations(1, c) = movieIdx - 1;
            //ratings.push_back(rating);

            totalMovies += 1;
            sumRatings += rating;
            vector<double> userArr;
            vector<double> movieArr;
            // std::vector<int> moviesOfUserMap; // not needed - file already written
            // std::vector<int> usersOfMovieMap; // not needed anymore

            if (userMap.count(userIdx) > 0)
            {
                userArr = userMap[userIdx];
                //moviesOfUserMap = moviesOfUsers[userIdx];
            }
            else
            {
                userArr = {0, 0, 0};
            }

            if (movieMap.count(movieIdx) > 0)
            {
                movieArr = movieMap[movieIdx];
                //usersOfMovieMap = usersOfMovies[movieIdx];
            }
            else
            {
                movieArr = {0, 0, 0};
            }

            double uN = userArr[0] + 1;
            double uM = userArr[1] + (rating - userArr[1]) / uN;
            double uS = userArr[2] + (rating - userArr[1]) * (rating - uM);
            vector<double> newUser = {uN, uM, uS};

            double mN = movieArr[0] + 1;
            double mM = movieArr[1] + (rating - movieArr[1]) / mN;
            double mS = movieArr[2] + (rating - movieArr[1]) * (rating - mM);
            vector<double> newMovie = {mN, mM, mS};

            data[0][c] = userIdx;
            data[1][c] = movieIdx;
            data[2][c] = rating;

            // moviesOfUserMap.push_back(movieIdx);
            //moviesOfUserMap.push_back(rating);
            //usersOfMovieMap.push_back(userIdx);
            //usersOfMovieMap.push_back(rating);

            userMap[userIdx] = newUser;
            movieMap[movieIdx] = newMovie;
            //usersOfMovies[movieIdx] = usersOfMovieMap;
            //moviesOfUsers[userIdx] = moviesOfUserMap;

            c += 1;

            if (c % 1000000 == 0)
                std::cout << c << std::endl;


        }
        std::cout << c << std::endl;
        std::cout << userMap.size() << std::endl;
    }

    // Create the sparse matrix
    //sp_data = arma::sp_mat(locations, arma::vec(ratings));

    // Files below already written
    /*
    std::cout << "done with loading; writing to file now" << std::endl;
    ofstream outfile;
    outfile.open("src/usersOfMovies.txt");
    for (auto i : usersOfMovies)
    {
        //std::unordered_map<unsigned int, unsigned int> userRatingsMap = i.second;
        std::vector<int> userRatingsMap = i.second;
        outfile << i.first << " "; // write the movie index

        for (std::vector<int>::iterator it = userRatingsMap.begin(); it != userRatingsMap.end(); ++it)
        {
            int val = *it;
            outfile << val << " ";
        }
        outfile << "\n";
    }
    outfile.close();
    std::cout << "wrote first file, onto second one" << std::endl;
    ofstream outfile2;
    outfile2.open("src/moviesOfUsers.txt");
    for (auto i : moviesOfUsers)
    {
        //std::unordered_map<unsigned int, unsigned int> movieRatingsMap = i.second;
        std::vector<int> movieRatingsMap = i.second;
        outfile2 << i.first << " "; // write user index
        for (std::vector<int>::iterator it = movieRatingsMap.begin(); it != movieRatingsMap.end(); ++it)
        {
            int val = *it;
            outfile2 << val << " ";
        }
        outfile2 << "\n";
    }
    outfile2.close();
    std::cout << "wrote second file" << std::endl;
    */
}

LoadData2::~LoadData2()
{
    delete[] data;
}

// Load the data from the probe.dta file into a matrix
// Column 1: user
// Column 2: movie
// Column 3: rating
double **LoadData2::probe()
{
    std::cout << "starting probe" << std::endl;
    // 1374739 data points
    double **probe_mat = 0;
    probe_mat = new double*[3];
    for (unsigned int i = 0; i < 3; ++i)
        probe_mat[i] = new double[1374739];
    //arma::mat probe_mat(3, 16);

    // Open the file
    string line;
    ifstream myfile("src/um/probe.dta");
    //ifstream myfile("um/shortprobe.dta");

    int c = 0;
    if (myfile.is_open())
    {
        while (getline(myfile, line))
        {
            int space1 = line.find(" ");
            int space2 = line.find(" ", space1 + 1);
            int space3 = line.find(" ", space2 + 1);

            // Insert into the probe matrix
            probe_mat[0][c] = atoi(line.substr(0, space1).c_str());
            probe_mat[1][c] = atoi(line.substr(space1 + 1, space2).c_str());
            probe_mat[2][c] = atoi(line.substr(space3 + 1).c_str());

            c += 1;
        }
    }

    return probe_mat;

}

// Load the data from qual.dta into a matrix
double **LoadData2::qual()
{
    std::cout << "starting qual" << std::endl;
    // 1374739 data points
    double **qual_mat = 0;
    qual_mat = new double*[2];
    qual_mat[0] = new double[2749898];
    qual_mat[1] = new double[2749898];
    //arma::mat probe_mat(3, 16);

    // Open the file
    string line;
    ifstream myfile("src/um/qual.dta");
    //ifstream myfile("um/shortprobe.dta");

    int c = 0;
    if (myfile.is_open())
    {
        while (getline(myfile, line))
        {
            int space1 = line.find(" ");
            int space2 = line.find(" ", space1 + 1);

            // Insert into the probe matrix
            qual_mat[0][c] = atoi(line.substr(0, space1).c_str());
            qual_mat[1][c] = atoi(line.substr(space1 + 1, space2).c_str());

            c += 1;
        }
    }

    return qual_mat;

}

// Load the data from the train.dta file into a matrix
// Column 1: user
// Column 2: movie
// Column 3: rating
double **LoadData2::loadRatingsVector()
{
    return data;
}

arma::sp_mat LoadData2::loadSparseData()
{
    return sp_data;
}

double LoadData2::getUserMean(int userIdx)
{
    std::vector<double> curr = userMap[userIdx];
    return curr[1];
}

double LoadData2::getUserStddev(int userIdx)
{
    if (userMap[userIdx][0] < 2)
        return 0;
    else
        return sqrt(userMap[userIdx][2] / (userMap[userIdx][0] - 1));
}

double LoadData2::getMovieMean(int movieIdx)
{
    return movieMap[movieIdx][1];
}

double LoadData2::getMovieStddev(int movieIdx)
{
    if (movieMap[movieIdx][0] < 2)
        return 0;
    else
        return sqrt(movieMap[movieIdx][2] / (movieMap[movieIdx][0] - 1));
}

double LoadData2::getGlobalMean()
{
    return sumRatings / totalMovies;
}

std::vector<double> LoadData2::getBetterUserMean()
{
    unsigned int k = 25;
    std::vector<double> curr;
    std::vector<double> user_vec;
    std::cout << "better user mean beginning" << std::endl;
    for (int i = 1; i < 458294; ++i) {
        //curr = userMap[i];
        user_vec.push_back((getGlobalMean() * k + userMap[i][1] * userMap[i][0]) / (k + userMap[i][0]));
    }
    std::cout << "better user mean end" << std::endl;
    return user_vec;
}

std::vector<double> LoadData2::getBetterMovieMean()
{
    unsigned int k = 25;
    std::vector<double> movie_vec;
    std::vector<double> curr;
    for (int i = 1; i < 17771; ++i) {
        //curr = movieMap[i];
        movie_vec.push_back((getGlobalMean() * k + movieMap[i][1] * movieMap[i][0]) / (k + movieMap[i][0]));
    }
    return movie_vec;
}

std::unordered_map<int, vector<double>> LoadData2::getUserMap()
{
    return userMap;
}

std::unordered_map<int, vector<double>> LoadData2::getMovieMap()
{
    return movieMap;
}

/*
std::vector<unsigned int> LoadData2::getUserRatings(unsigned int user)
{
    vector<unsigned int> userRatingArr;
    if (userRatings.count(user) > 0)
        userRatingArr = userRatings[user];
    else
        userRatingArr = {};vector resize
    return userRatingArr;
}

std::vector<unsigned int> LoadData2::getMovieRatings(unsigned int movie)
{
    vector<unsigned int> movieRatingArr;
    if (movieRatings.count(movie) > 0)
        movieRatingArr = movieRatings[movie];
    else
        movieRatingArr = {};
    return movieRatingArr;
}
*/

/*
int main() {
    std::cout << "starting main" << std::endl;
    LoadData2 l = LoadData2();
    std::cout << l.getUserMean(1) << std::endl;
}
*/


