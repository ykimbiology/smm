

// compile:
// g++ ex_class.cpp -o ex_class -lnlopt -lm -O1 -larmadillo

#include <iostream>

#include <nlopt.hpp>
#include <armadillo>


using namespace std;
using namespace arma;

// Create a class holding matrices from the armadillo lib:


class FunctionData {
    mat A; // Design matrix.
    mat yobs; // observed.
  public:
    //void set_values (mat &A_test, mat &yobs_test);
    void set_values (mat &A_test, mat &yobs_test) { A = A_test; yobs = yobs_test; }
    mat get_design_matrix() { return A; }
    mat get_yobs() { return yobs; }
};



int main () {
    mat A = randu<mat>(2,2);
	mat yobs = randu<mat>(2,1);

	FunctionData rect, rectb;
    rect.set_values (A, yobs);

    //cout << "rect area: " << rect.area() << endl;
    //cout << "A " << rect.A << endl;
    cout << "A " << rect.get_design_matrix() << endl;

    return 0;
}
