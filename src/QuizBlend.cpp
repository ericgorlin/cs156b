#include "QuizBlend.h"

QuizBlend::QuizBlend()
{
    outfile = "5192quizblend.dta";
    outcoef = "5192coefs.dta";
    variance = 1.274;
    mean = 3.674;
    qual_size = 2749898;
    lambda = .0014;

    // what
    n = 2749898;
    // Create a matrix of specified size.

    // Open a buncha files and add to the matrix
    std::vector<std::string> files;// = ["sgd_results26.txt", "SGDpp_results19.txt"];
    files.push_back("sgd_results26.txt");
    files.push_back("SGDpp_results19.txt");
    files.push_back("SGDpp_results17.txt");
    files.push_back("../../552sgd_results.dta");
    files.push_back("../../sgd_results24.txt");
    files.push_back("../../SGDppelaine_results10.txt");
    files.push_back("../../SGDpp_results13.txt");
    files.push_back("../../sgd_results21.txt");
    files.push_back("../../56best_sgd_results.dta");
    files.push_back("../../SGDppelaine_results9.txt");
    files.push_back("../../SGDpp_results11.txt");
    files.push_back("../../SGDpp_results12.txt");
    files.push_back("../../SGDpp_results15.txt");
    files.push_back("../../SGDpp_results16.txt");
    files.push_back("../../sgd_results22.txt");
    files.push_back("../../sgd_results23.txt");
    files.push_back("../../sgd_results18.txt");
    num_files = files.size();
    ratings = arma::mat(qual_size, num_files);
    //rmse = [.90399, .90242];
    rmse.push_back(.90399);
    rmse.push_back(.902424);
    rmse.push_back(.90316);
    rmse.push_back(.90536);
    rmse.push_back(.90465);
    rmse.push_back(.90323);
    rmse.push_back(.90369);
    rmse.push_back(.90541);
    rmse.push_back(.90658);
    rmse.push_back(.90472);
    rmse.push_back(.90589);
    rmse.push_back(.90552);
    rmse.push_back(.90322);
    rmse.push_back(.90319);
    rmse.push_back(.90571);
    rmse.push_back(.90666);
    rmse.push_back(.90638);

    std::cout << "Creating matrix" << std::endl;
    for (int i = 0; i < num_files; ++i) {
        std::ifstream f0;
        std::string line;
        f0.open(files[i]);
        if (f0.is_open()) {
            int c = 0;
            double val = 0;
            while (getline(f0, line)) {
                std::istringstream iss(line);
                iss >> val;
                ratings.at(c, i) = val - mean;
                c += 1;
            }
        }
        f0.close();
    }
}

QuizBlend::~QuizBlend()
{
    //dtor
}

void QuizBlend::run_blend()
{
    // Calculate the y matrix
    arma::mat y = arma::mat(num_files, 1);

    std::cout << "finding y" << std::endl;
    for (int i = 0; i < num_files; ++i)
    {
        double term1 = n * variance;
        double term2 = 0;
        for (int j = 0; j < qual_size; ++j)
        {
            double val = ratings.at(j,i);//i, j);
            term2 += val * val;
        }
        double term3 = n * rmse[i] * rmse[i];

        y.at(i, 0) = 0.5 * (term1 + term2 - term3);
        std::cout << term1 << " " << term2 << " " << term3 << std::endl;
    }

    // Find beta using ridge regression
    std::cout << "finding beta" << std::endl;
    arma::mat beta = arma::mat(num_files, 1);
    arma::mat iden = arma::eye<arma::mat>(num_files, num_files);
    arma::mat x = ratings;
    beta = (x.t() * x + lambda * n * iden).i() * y;

    std::cout << "writing to files" << std::endl;
    std::ofstream outc;
    outc.open(outcoef);
    for (int i = 0; i < num_files; ++i)
        outc << beta(i,0) << "\n";
    outc.close();
    // Find the new ratings vector
    arma::mat new_ratings = arma::mat(qual_size, 1);
    new_ratings = x * beta;

    // Write to an output file.
    std::ofstream outf;
    outf.open(outfile);

    for (int i = 0; i < qual_size; ++i)
        outf << new_ratings(i, 0) + mean << "\n";
    outf.close();
}

/*
int main()
{
    QuizBlend q = QuizBlend();
    q.run_blend();
    return 0;
}
*/
