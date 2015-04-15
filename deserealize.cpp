#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <fstream>
#include <boost/archive/text_iarchive.hpp>

//void desearlize() {
int main() {
    clock_t begin = clock();
    const char* fileName = "sparse_matrix_ublas";

    boost::numeric::ublas::mapped_matrix<int> m(458293, 17770);

    // Restore data
    std::ifstream ifs(fileName);
    boost::archive::text_iarchive ar(ifs);

    ar & m;

    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    std::cout << elapsed_secs;
}
