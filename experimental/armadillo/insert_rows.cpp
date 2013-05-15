#include <iostream>
#include <armadillo>

// compile:
// g++ insert_rows.cpp -o ex01 -O1 -larmadillo

using namespace std;
using namespace arma;

int main(int argc, char** argv)
  {
	mat A = randu<mat>(5,10);
	mat B = ones<mat>(5,2);

	cout << A << endl;
	cout << B << endl;

	// at column 2, insert a copy of B;
	// A will now have 12 columns
	A.insert_cols(2, B);

	// at column 1, insert 5 zeroed columns;
	// B will now have 7 columns
	B.insert_cols(1, 5);

	cout << A << endl;
    cout << B << endl;

    // Remove specified row.
    B.shed_row(1);
    cout << B << endl;

    // Demonstrates replace a column in a matrix.
    B.col(0) = randn<mat>(4,1);
    cout << B << endl;


  return 0;
  }
