#include "LoadData3.h"

LoadData3::LoadData3()
{
    std::cout << "Loading data..." << std::endl;
    
    
    bool testingOnProbe = true; // change this in TimeSVDpp.cpp as well
    
    
    n_users = 458293;
    n_movies = 17770;
    if (testingOnProbe)
        n_datapoints = 1374739;
    else
        n_datapoints = 98291669;
    // Training set has 98291669 values
    data = 0;
    data = new double*[4];
    for (unsigned int i = 0; i < 4; i++)
        data[i] = new double[n_datapoints];
    
    totalMovies = 0;
    sumRatings = 0;
    
    movies_per_user = new set<int>[n_users];
    userAvgDate = new double[n_users];
    for (unsigned int i = 0; i < n_users; ++i)
        userAvgDate[i] = 0;
    
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
            int date = atoi(line.substr(space2 + 1, space3).c_str());
            int rating = atoi(line.substr(space3 + 1).c_str());
            
            totalMovies += 1;
            sumRatings += rating;

            data[0][c] = userIdx;
            data[1][c] = movieIdx;
            data[2][c] = rating;
            data[3][c] = date;
            
            movies_per_user[userIdx - 1].insert(movieIdx);
            userAvgDate[userIdx - 1] += date;
            
            
            c += 1;
            
            
        }

        std::cout << c << std::endl;
        for (unsigned int i = 0; i < n_users; ++i) {
            
            userAvgDate[i] /= movies_per_user[i].size();
        }
    }
}

LoadData3::~LoadData3()
{
    delete[] data;
    delete[] movies_per_user;
    delete[] userAvgDate;
}

// Load the data from the probe.dta file into a matrix
// Column 1: user
// Column 2: movie
// Column 3: rating
double **LoadData3::probe()
{
    std::cout << "starting probe" << std::endl;
    // 1374739 data points
    double **probe_mat = 0;
    probe_mat = new double*[4];
    for (unsigned int i = 0; i < 4; ++i)
        probe_mat[i] = new double[1374739];
    
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
            probe_mat[3][c] = atoi(line.substr(space2 + 1, space3).c_str());
                                   
            
            c += 1;
        }
    }
    
    return probe_mat;
    
}

// Load the data from qual.dta into a matrix
double **LoadData3::qual()
{
    std::cout << "starting qual" << std::endl;
    // 1374739 data points
    double **qual_mat = 0;
    qual_mat = new double*[3];
    for (unsigned int i = 0; i < 3; ++i)
        qual_mat[i] = new double[2749898];
    
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
            qual_mat[2][c] = atoi(line.substr(space2 + 1).c_str());
            
            c += 1;
        }
    }
    
    return qual_mat;
    
}

// Load the data from the train.dta file into a matrix
// Column 1: user
// Column 2: movie
// Column 3: rating
// Column 4: date
double **LoadData3::loadRatingsVector()
{
    return data;
}


double LoadData3::getGlobalMean()
{
    return sumRatings / totalMovies;
}


set<int> *LoadData3::getMoviesPerUser()
{
    return movies_per_user;
}

double *LoadData3::getUserAvgDate()
{
    return userAvgDate;
}

// For use in SVD++
std::vector<double> LoadData3::getNorms()
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


