#include "LoadData.h"

LoadData::LoadData()
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

    // Training set has 98291669 values
    data = arma::umat(3, 98291669);
    std::cout << "created a vector" << std::endl;
    //arma::umat locations = arma::umat(2, 716);

    //arma::umat locations = arma::umat(2, 716);
    //int n_users = 458293;
    //int n_movies = 17770;

    // Open the file
    string line;
    //ifstream myfile("../src/um/train.dta");
    ifstream myfile("../src/um/train.dta");
    //ifstream myfile("um/all.dta");
    //ifstream myfile("um/shortall.dta");
    std::cout << myfile.is_open() << std::endl;
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
            vector<double> userArr;
            vector<double> movieArr;

            if (userMap.count(userIdx) > 0)
                userArr = userMap[userIdx];
            else
                userArr = {0, 0, 0};

            if (movieMap.count(movieIdx) > 0)
                movieArr = movieMap[movieIdx];
            else
                movieArr = {0, 0, 0};
            double uN = userArr[0] + 1;
            double uM = userArr[1] + (rating - userArr[1]) / uN;
            double uS = userArr[2] + (rating - userArr[1]) * (rating - uM);
            vector<double> newUser = {uN, uM, uS};

            double mN = movieArr[0] + 1;
            double mM = movieArr[1] + (rating - movieArr[1]) / mN;
            double mS = movieArr[2] + (rating - movieArr[1]) * (rating - mM);
            vector<double> newMovie = {mN, mM, mS};

            data(0, c) = userIdx;
            data(1, c) = movieIdx;
            data(2, c) = rating;

            userMap[userIdx] = newUser;
            movieMap[movieIdx] = newMovie;

            c += 1;


        }
        std::cout << c << std::endl;
        std::cout << userMap.size() << std::endl;
    }
}

LoadData::~LoadData()
{
    //dtor
}

/*
 * Read in data from the training set (train.dta)
 * and store in a sparse vector. There are
 * 458293 users and 17770 movies.
 */
arma::sp_mat LoadData::start()
{
    std::cout << "starting start..." << std::endl;
    vector<double> ratings;
    // Training set has 98291669 values
    arma::umat locations = arma::umat(2, 98291669);
    //arma::umat locations = arma::umat(2, 716);

    // Open the file
    string line;
    ifstream myfile("um/train.dta");
    //ifstream myfile("um/all.dta");
    //ifstream myfile("um/shortall.dta");
    int c = 0;
    if (myfile.is_open())
    {
        while (getline(myfile, line))
        {
            int space1 = line.find(" ");
            int space2 = line.find(" ", space1 + 1);
            int space3 = line.find(" ", space2 + 1);

            // Insert into our temporary data vectors
            locations(0, c) = atoi(line.substr(0, space1).c_str()) - 1;
            locations(1, c) = atoi(line.substr(space1 + 1, space2).c_str()) - 1;
            ratings.push_back(atoi(line.substr(space3 + 1).c_str()));

            c += 1;


        }
    }

    cout << "creating matrix" << endl;

    // Create the sparse matrix
    arma::sp_mat m = arma::sp_mat(locations, arma::vec(ratings));

    return m;
}

/*
arma::sp_mat LoadData::sparseFromMat(arma::umat y) {
    std::cout << y.row(2);
    //arma::sp_mat m = arma::sp_mat(y.rows(0,1), arma::vec(y.row(2));
}
*/

// Load the data from the probe.dta file into a matrix
// Column 1: user
// Column 2: movie
// Column 3: rating
arma::umat LoadData::probe()
{
    std::cout << "starting probe" << std::endl;
    // 1374739 data points
    arma::umat probe_mat(3, 1374739);
    //arma::mat probe_mat(3, 16);

    // Open the file
    string line;
    ifstream myfile("../src/um/probe.dta");
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
            probe_mat(0, c) = atoi(line.substr(0, space1).c_str());
            probe_mat(1, c) = atoi(line.substr(space1 + 1, space2).c_str());
            probe_mat(2, c) = atoi(line.substr(space3 + 1).c_str());

            c += 1;
        }
    }

    return probe_mat;

}

// Load the data from qual.dta into a matrix
arma::mat LoadData::qual()
{
    std::cout << "starting qual" << std::endl;
    // 1374739 data points
    arma::mat qual_mat(2, 2749898);
    //arma::mat probe_mat(3, 16);

    // Open the file
    string line;
    ifstream myfile("../src/um/qual.dta");
    //ifstream myfile("um/shortprobe.dta");

    int c = 0;
    if (myfile.is_open())
    {
        while (getline(myfile, line))
        {
            int space1 = line.find(" ");
            int space2 = line.find(" ", space1 + 1);

            // Insert into the probe matrix
            qual_mat(0, c) = atoi(line.substr(0, space1).c_str());
            qual_mat(1, c) = atoi(line.substr(space1 + 1, space2).c_str());

            c += 1;
        }
    }

    return qual_mat;

}

// Load the data from the train.dta file into a matrix
// Column 1: user
// Column 2: movie
// Column 3: rating
arma::umat LoadData::loadRatingsVector()
{
    return data;
}

double LoadData::getUserMean(int userIdx)
{
    std::vector<double> curr = userMap[userIdx];
    return curr[1];
}

double LoadData::getUserStddev(int userIdx)
{
    if (userMap[userIdx][0] < 2)
        return 0;
    else
        return sqrt(userMap[userIdx][2] / (userMap[userIdx][0] - 1));
}

double LoadData::getMovieMean(int movieIdx)
{
    return movieMap[movieIdx][1];
}

double LoadData::getMovieStddev(int movieIdx)
{
    if (movieMap[movieIdx][0] < 2)
        return 0;
    else
        return sqrt(movieMap[movieIdx][2] / (movieMap[movieIdx][0] - 1));
}
/*
int main() {
    LoadData l = LoadData();
    std::cout << l.getUserMean(1) << std::endl;
}
*/

