
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


mat get_model_ref(int n)
{
    // Return a 1-d vector of the model: x
	// y = A*x
	// y = predictions. [n x p]
	// A = design matrix; [n x p]
	// x = model; [p x 1]
	mat model = randu<mat>(1,n);
	cout << "inside f " << model << endl;
	return model;
}

void get_measured(mat &model, int n)
{
    //
	cout << "inside get_m " << model << endl;
	for (int i=0; i<n; i++)
	{
        cout << "loop " << i << endl;
	}
}

double myfunc(const std::vector<double> &x, std::vector<double> &grad, void *my_func_data)
{
	// y = (x - xoffset)^2
	double xoffset;
    xoffset = 3.0;

    std::vector<double> f_data = *(std::vector<double> *)(my_func_data); // This works too!!
    cout << "f_data[0] " << f_data[0] << " " << f_data[1] << endl;

    if (!grad.empty()) {
        grad[0] = 2*(x[0] - xoffset);
        grad[1] = 2*x[1];
    }
    double xvalue;
    xvalue = (x[0] - xoffset)*(x[0] - xoffset) + x[1]*x[1];
    cout << x[0] << " " << x[1] << " " << xvalue <<endl;
    return xvalue;
}


int main(int argc, char* argv[])
{

int N = 2;  // Number of samples;
int p = 2;  // Number of features.

nlopt::opt opt(nlopt::LD_MMA, N);

mat model = randu<mat>(1,N);
model = get_model_ref(N);
cout << "model " << model<< endl;

get_measured(model, N);



//std::vector<double> lb(2);
//lb[0] = -HUGE_VAL; lb[1] = -10;
//opt.set_lower_bounds(lb);

//opt.set_min_objective(myfunc, NULL);

std::vector<double> f_data(2);
f_data[0] = 9.1; f_data[1] = 10.1;

//cout << "fdata " << f_data << endl; // Does not work.

//double f_data;
//f_data = 1.1;

//opt.set_min_objective(myfunc, f_data);
opt.set_min_objective(myfunc, &f_data);


//my_constraint_data data[2] = { {2,0}, {-1,1} };
//opt.add_inequality_constraint(myconstraint, &data[0], 1e-8);
//opt.add_inequality_constraint(myconstraint, &data[1], 1e-8);

opt.set_xtol_rel(1e-4);

//std::vector<double> x(2);
//x[0] = 1.234; x[1] = 5.678;

typedef std::vector<double> stdvec;

std::srand(1);  // To change random seed.
mat xb = randu<mat>(1,2);
//xb[0] = 2.234; xb[1] = 6.678;
cout << " arma mat x[0] " << xb[0] << endl;
stdvec x = conv_to< stdvec >::from(xb);
cout << " vector mat x[0] " << x[0] << endl;

//double minf;
//nlopt::result result = opt.optimize(x, minf);

//cout << "minimum: " << x[0] << " " << x[1] << " " << minf << endl;



}
