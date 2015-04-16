#include <iostream>
#include <fstream>
#include <ctime>
#include <boost/archive/text_oarchive.hpp>
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
    if (myfile.is_open())
    {
        while (getline(myfile, line))
        {
            //cout << line << endl;
            if (c % 100 == 0)
                cout << c<< endl;


            int space1 = line.find(" ");
            int space2 = line.find(" ", space1 + 1);
            int space3 = line.find(" ", space2 + 1);

            // Insert into our temporary data vectors
            locations(0, c) = atoi(line.substr(0, space1).c_str());
            locations(1, c) = atoi(line.substr(space1 + 1, space2).c_str());
            ratings.push_back(atoi(line.substr(space3 + 1).c_str()));
            // cout << user << " " << review << " " << rating << endl;
            /*
            boost::split(split_line, line, boost::is_any_of(" "));

            // Convert data to ints
            for (unsigned int i = 0; i < split_line.size(); ++i)
            {
                line_data.push_back(atoi(split_line[i].c_str()));
            }
            */

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

    cout << m;

}
