#include "catch.hpp"
#include "utilities.h"

#include "directories.h"
using namespace purify;
using namespace purify::notinstalled;

TEST_CASE("utilities [mod]", "[mod]") {
    Array<t_real> range;
    range.setLinSpaced(201, -100, 100);
    Array<t_real> output(range.size());
    for (t_int i = 0; i < range.size(); ++i)
    {
    	output(i) = utilities::mod(range(i), 20);
    }
    Array<t_real> expected(201);
    expected << 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
    			0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
    			0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
    			0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
    			0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
    			0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
    			0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
    			0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
    			0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
    			0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
    			0;
    CHECK(expected.isApprox(output, 1e-13));
}

TEST_CASE("utilities [reshape]", "[reshape]"){
	//Testing if resize works the same as matlab's reshape
	Matrix<t_real> magic(4, 4);
	magic <<     16, 2, 3, 13,
     			5, 11, 10, 8,
     			9 ,7, 6, 12,
     			4, 14, 15, 1;
    std::cout << magic << '\n' << '\n'; 
    Vector<t_real> magic_vector(16);
    magic_vector << 16, 5, 9, 4, 2, 11, 7, 14, 3, 10, 6, 15, 13, 8, 12, 1;
    Matrix<t_real> magic_matrix(16, 1);
    magic_matrix <<  16, 5, 9, 4, 2, 11, 7, 14, 3, 10, 6, 15, 13, 8, 12, 1;
    magic_matrix.resize(4, 4);
    CHECK(magic.isApprox(magic_matrix, 1e-13));
    magic.resize(16, 1);
    std::cout << magic << '\n' << '\n';
    std::cout << magic_vector << '\n' << '\n';
    CHECK(magic.isApprox(magic_vector, 1e-13));
}