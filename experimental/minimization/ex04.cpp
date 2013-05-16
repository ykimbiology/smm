
#include <iostream>

#include <nlopt.hpp>
#include <armadillo>


using namespace std;
using namespace arma;

/**
 *
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
 * g++ ex04.cpp -o ex04 -lnlopt -lm -O1 -larmadillo
 */

typedef std::vector<double> stdvec;

void print_row(const std::vector<double> &x)
{
  for (int i=0; i<x.size(); i++) {
	  cout << x[i] << " ";
  }
  cout << endl;
}


class FunctionData {
    mat A; // Design matrix.
    mat yobs; // observed.
  public:
    //void set_values (mat &A_test, mat &yobs_test);
    void set_values (mat &A_test, mat &yobs_test) { A = A_test; yobs = yobs_test; }
    mat get_design_matrix() { return A; }
    mat get_yobs() { return yobs; }
};

mat get_model_ref(int p)
{
    // Return a 1-d vector of the model: x
	// y = A*x
	// y = predictions. [n x 1]
	// A = design matrix; [n x p]
	// x = model; [p x 1]
	mat model = randu<mat>(p,1);
	//cout << "inside f " << model << endl;
	return model;
}

mat get_design_matrix(mat &model, int n, int p)
{
    //
	//mat y = randu<mat>(n,1);
	mat A = randu<mat>(n,p); // A random matrix of (n,p) dimensions.
	//cout << "inside get_m " << model << endl;
	return A;
}

//mat get_measured(mat &dm, mat &model, double noise)
//{
//    //noise = 0 = no noise; posiive value idicates amount of noise.
//    int num_samples = dm.n_rows;
//    //cout << "num samples " << num_samples << endl;
//	mat y;
//    mat vnoise = noise*randn<mat>(num_samples,1);
//    y = dm * model + vnoise;
//    cout << "get_measured y " << y << endl;
//    return y;
//}

mat get_measured(mat &dm, mat &model, double noise)
{
	// y = Ax
	// y = Ax + x*x
    //noise = 0 = no noise; posiive value idicates amount of noise.
    int num_samples = dm.n_rows;
    //cout << "num samples " << num_samples << endl;
	mat y;
    mat vnoise = noise*randn<mat>(num_samples,1);

    y = dm * model;
    y = y + vnoise;
    //cout << "get_measured y " << y << endl;
    return y;
}

double myfunc(const std::vector<double> &x, std::vector<double> &grad, void *my_func_data)
{

    // a1*x1 + a2*x2 + ... = y1
	// INPUT:
	//    A: Design matrix.
	//    yobs: observed
	// f = (Ax - yobs)^2
	// df/xi = 2(Ax - yobs)*a1
	//cout << "Begin myfunc" << endl;

    FunctionData f_data = *(FunctionData *)(my_func_data);   //
    mat xb = conv_to< mat >::from(x);  // Make sure dimensons match with the matrix multiplciation.
    //cout << "myfunc xb " << xb << endl;

    mat A = f_data.get_design_matrix();
    int num_samples = A.n_rows;  // 'n' variable;
    int num_features = x.size();  // 'p' variable.
    mat yobs = f_data.get_yobs();
    mat diff = A*xb - yobs;
    mat xvalue = diff.t()*diff;
    double xvalue_b = conv_to< double >::from(xvalue);
//    cout << "myfunc f_data.get_design_matrix() " << A << endl;
//    cout << "diff " << diff << endl;
//    cout << "xvalue " << xvalue << endl;
    cout << "xvalue_b " << xvalue_b << endl;

    if (!grad.empty()) {
    	for (int i=0; i<num_features; i++) {
            grad[i] = 0.0;
            mat xg = 2*A.col(i).t()*(A*xb - yobs); // yobs = [nx1]; A.col(i)=[nx1]; xb[i] = [1x1]
            //mat xg = -2*A.col(i).t()*(yobs - A.col(i)*xb[i]); // yobs = [nx1]; A.col(i)=[nx1]; xb[i] = [1x1]
            //mat xg = 2*A.col(i).t()*(yobs - A.col(i)*xb[i]); // yobs = [nx1]; A.col(i)=[nx1]; xb[i] = [1x1]
            grad[i] = conv_to< double >::from(xg);
            //cout << "grad " << grad[i] << endl;
    	}
    }

    //return (double)xvalue;
    //return 1.0;
    return xvalue_b;
}


int main(int argc, char* argv[])
{

	int N = 1000;  // Number of samples;
	int p = 200;  // Number of features.

	nlopt::opt opt(nlopt::LD_LBFGS, p);       // requires gradient. Works reallly well; much better than MMA.
	//nlopt::opt opt(nlopt::LD_MMA, p);       // requires gradient.
	//nlopt::opt opt(nlopt::LN_NELDERMEAD, p);  // Does not require gradient.

	mat model_ref = randu<mat>(p,1);
	model_ref = get_model_ref(p);
	//cout << "model " << model<< endl;

	mat A    = get_design_matrix(model_ref, N, p);
	mat y    = get_measured(A, model_ref, 0.0);
	mat yobs = get_measured(A, model_ref, 0.1);
	//mat yobs = get_measured(A, model_ref, 0.0);

	cout << " A " << A.n_rows << " " << A.n_cols << endl;
	cout << " model_ref " << model_ref.n_rows << " " << model_ref.n_cols << endl;
	cout << " yobs " << yobs.n_rows << " " << yobs.n_cols << endl;


//	cout << "Design matrix: " << A << endl;
//	cout << "true " << y << endl;
//	cout << "obs " << yobs << endl;




	//opt.set_min_objective(myfunc, NULL);


	FunctionData f_data;
	f_data.set_values(A, yobs);
	opt.set_min_objective(myfunc, &f_data);
	opt.set_xtol_rel(1e-4);



	//mat xb = randu<mat>(p,1);
	//stdvec x = conv_to< stdvec >::from(xb);
	//stdvec x;
	std::vector<double> x(p);
	double minf;
	cout << "initial x " << x[0] << endl;

	nlopt::result result = opt.optimize(x, minf);



	cout << "minimum: " << x[0] << " " << x[1] << " " << minf << endl;

	print_row(x);
	cout << "model_ref " << model_ref << endl;

}
