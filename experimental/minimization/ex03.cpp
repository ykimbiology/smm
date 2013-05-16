
#include <iostream>

#include <nlopt.hpp>
#include <armadillo>


using namespace std;
using namespace arma;

/**
 * GOALS:
 * 1) Use both armadillo lib and nlopt to minimize a complex function.
 *
 * 1) Get design matrix: A
 * y = Ax;
 * A = Design matrix; [N x p]; N = number of samples; p = number of features.
 * x = model to learn; [p x 1]
 * y = predicted; [N x 1]
 * y_meas = measured values.
 * minimizing function: (Ax - y)^2 + t(x)*x
 *
 * compile:
 * g++ ex03.cpp -o ex03 -lnlopt -lm -O1 -larmadillo
 */

typedef std::vector<double> stdvec;

void print_row(const std::vector<double> &x)
{
  for (int i=0; i<x.size(); i++) {
	  cout << x[i] << " ";
  }
  cout << endl;
}

double myfunc(const std::vector<double> &x, std::vector<double> &grad, void *my_func_data)
{
	// y = (x - xoffset)^2
	// INPUT:
	//    A: Design matrix.
	//    yobs: observed
	// f = (Ax - yobs)^2
	// df = 2(Ax - yobs)A?
	double xoffset;
    xoffset = 3.0;

    //std::vector<double> f_data = *(std::vector<double> *)(my_func_data); // This works too!!
    //cout << "f_data[0] " << f_data[0] << " " << f_data[1] << endl;

    if (!grad.empty()) {
        grad[0] = 2*(x[0] - xoffset);
        grad[1] = 2*x[1];
        grad[2] = 2*x[2];
    }
    double xvalue;
    xvalue = (x[0] - xoffset)*(x[0] - xoffset) + x[1]*x[1];
    //cout << "myfunc " << x[0] << " " << x[1] << " " << xvalue <<endl;
    cout << "myfunct ";
    print_row(x);
    return xvalue;
}


int main(int argc, char* argv[])
{

int N = 2;  // Number of samples;
int p = 4;  // Number of features.

//nlopt::opt opt(nlopt::LD_MMA, N);       // requires gradient.
nlopt::opt opt(nlopt::LN_NELDERMEAD, p);  // Does not require gradient.


std::vector<double> lb(p);
cout << " lbs size " << lb.size() << endl;

lb[0] = -HUGE_VAL; lb[1] = -10; lb[2] = -10;
opt.set_lower_bounds(lb);


opt.set_min_objective(myfunc, NULL);
opt.set_xtol_rel(1e-4);


std::vector<double> x(p);
double minf;
cout << "initial x " << x[0] << endl;

nlopt::result result = opt.optimize(x, minf);



////cout << "minimum: " << x[0] << " " << x[1] << " " << minf << endl;



}
