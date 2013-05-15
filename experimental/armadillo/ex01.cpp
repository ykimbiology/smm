#include <iostream>
#include <armadillo>

// compile: g++ ex01.cpp -o ex01 -O1 -larmadillo

using namespace std;
using namespace arma;

int main(int argc, char** argv)
  {
//  mat A = randu<mat>(4,5);
//  mat B = randu<mat>(4,5);
  mat A = randu<mat>(1,5);
  mat B = randu<mat>(1,5);

  cout << "A"<<A << endl;
  cout << "B"<<B << endl;
  cout << A*B.t() << endl;
  cout << A - B << endl;

  return 0;
  }
