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


    bool testingOnProbe = false; // change this in SGDpp.cpp as well


    n_users = 458293;
    n_movies = 17770;
    if (testingOnProbe)
        n_datapoints = 1374739;
    else
        n_datapoints = 98291669;
    // Training set has 98291669 values
    data = 0;
    data = new double*[3];
    for (unsigned int i = 0; i < 3; i++)
        data[i] = new double[n_datapoints];

    totalMovies = 0;
    sumRatings = 0;

    movies_per_user = new set<int>[n_users];

    // Open the file
    string line;
    string filename;
    if (testingOnProbe) {
        filename = "../um/probe.dta";
    } else {
        filename = "../um/train.dta";
    }
    ifstream myfile(filename.c_str());

    //ifstream myfile("um/all.dta");
    //ifstream myfile("um/shortall.dta");
    cout << myfile.is_open() << " open" << endl;
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


            totalMovies += 1;
            sumRatings += rating;
//            vector<double> userArr;
//            vector<double> movieArr;

//            if (userMap.count(userIdx) > 0)
//                userArr = userMap[userIdx];
//            else
//                userArr = {0, 0, 0};
//
//            if (movieMap.count(movieIdx) > 0)
//                movieArr = movieMap[movieIdx];
//            else
//                movieArr = {0, 0, 0};
//            double uN = userArr[0] + 1;
//            double uM = userArr[1] + (rating - userArr[1]) / uN;
//            double uS = userArr[2] + (rating - userArr[1]) * (rating - uM);
//            vector<double> newUser = {uN, uM, uS};
//
//            double mN = movieArr[0] + 1;
//            double mM = movieArr[1] + (rating - movieArr[1]) / mN;
//            double mS = movieArr[2] + (rating - movieArr[1]) * (rating - mM);
//            vector<double> newMovie = {mN, mM, mS};

            data[0][c] = userIdx;
            data[1][c] = movieIdx;
            data[2][c] = rating;

            movies_per_user[userIdx - 1].insert(movieIdx);

//            userMap[userIdx] = newUser;
//            movieMap[movieIdx] = newMovie;


            c += 1;


        }
        std::cout << c << std::endl;
//        std::cout << userMap.size() << std::endl;
    }
}

LoadData2::~LoadData2()
{
    delete[] data;
    delete[] movies_per_user;
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
    ifstream myfile("../um/probe.dta");
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
    ifstream myfile("../um/qual.dta");
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
    return (1.0 * sumRatings) / totalMovies;
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

// For use in SVD++
set<int> *LoadData2::getMoviesPerUser()
{
    return movies_per_user;
}

// For use in SVD++
std::vector<double> LoadData2::getNorms()
{
    std::vector<double> norms;

    for (int i = 0; i < n_users; i++) {
        norms.push_back(pow(movies_per_user[i].size(), -0.5));
    }
    return norms;
}


/*
int main() {
    LoadData l = LoadData();
    std::cout << l.getUserMean(1) << std::endl;
}
*/


