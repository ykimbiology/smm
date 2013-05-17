
/**
 * Does not work; probably because gcc 4.4.3 does not implement C++11?
 *http://gcc.gnu.org/projects/cxx0x.html
 *
 *
 * compile:
 * g++ ex_transform.cpp -o ex_transform -lnlopt -lm -O1 -larmadillo
 */

#include <iostream>

#include <nlopt.hpp>
#include <armadillo>


using namespace std;
using namespace arma;



// C++11 only example

mat A = ones<mat>(4,5);

// add 123 to every element
A.transform( [](double val) { return (val + 123.0); } );
