#include <iostream>
#include <fstream>
#include <ctime>
#define ARMA_64BIT_WORD
#include <armadillo>

/*
 * Read in data from the um.zip file all.dta
 * and store in a sparse vector. There are
 * 458293 reviewers and 17770 reviews.
 */

int main()
{
    using namespace std;
    clock_t begin = clock();

    vector<double> ratings;
    arma::umat locations = arma::umat(2, 102416306);

    // Open the file
    string line;
    ifstream myfile("um/all.dta");
    int c = 0;
    cout << "starting" << endl;
    if (myfile.is_open())
    {
        cout << "HI" << endl;
        while (getline(myfile, line))
        {
            int space1 = line.find(" ");
            int space2 = line.find(" ", space1 + 1);
            int space3 = line.find(" ", space2 + 1);

            // Insert into our temporary data vectors
            locations(0, c) = atoi(line.substr(0, space1).c_str());
            locations(1, c) = atoi(line.substr(space1 + 1, space2).c_str());
            ratings.push_back(atoi(line.substr(space3 + 1).c_str()));

            c += 1;


        }
    }

    // Create the sparse matrix
    arma::sp_mat m = arma::sp_mat(locations, arma::vec(ratings));

    // Serialize the sparse matrix object
    //fstream ofs("sparse_matrix_eigen");
    //boost::archive::text_oarchive ar(ofs);

    // Write data
    //ar & m;

    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout << elapsed_secs << endl;

    //cout << m;

}
