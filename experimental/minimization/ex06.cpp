
#include <iostream>

#include <nlopt.hpp>
#include <armadillo>


using namespace std;
using namespace arma;

//#define WPAIR 0.0  // Minimization was successful.
//#define WPAIR 1.0
//#define WPAIR 0.01
#define WPAIR 0.1

/**
 * This version tries global minimization.
 *
 * http://permalink.gmane.org/gmane.science.analysis.nlopt.general/153
 *
 * ToDo:
 * 1) Update design matrix generation so that for each bin of 10 elements, only 1 has '1' and rest '0's.
 *    This may make the minimizing function easier?
 *
 * GOALS:
 * 1) Use both armadillo lib and nlopt to minimize a complex function.
 *
 * 1) Get design matrix: A
 * y = Ax;
 * A = Design matrix; [n x p]; n = number of samples; p = number of features.
 * x = model to learn; [p x 1]
 * y = predicted; [n x 1]
 * y_meas = measured values.
 * minimizing function:
 *     1) f(x) = (y - Ax)^2
 *     2) f(x) = (y - (Ax + g))^2
 *
 * PairwiseTerms:
 * --------------
 * g = pair-wise combinations of -1.0*a1*a2*x1*x2 + ...
 * g = [n x 1]
 * g[i] =  sum(-1.0*a[j]*a[k]*x[j]*x[k]), where i = (1..n); j,k = (1..p)  // a[i] = [n x 1]; x[j] = [1 x 1]
 * dg[i]/x[j] = sum(-1.0*a[j]*a[k]*x[k]), where i = (1..n); j,k = (1..p); j != k  // a[i] = [n x 1]; x[j] = [1 x 1]
 *
 * Results:
 * --------
 * 1) Confirmed that non-derivative algorithm works on the model with pair terms.
 *
 * compile:
 * g++ ex06.cpp -o ex06 -lnlopt -lm -O1 -larmadillo
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
    // 1) Entries will have only 1 and 0s.
	//mat y = randu<mat>(n,1);
	mat A = randu<mat>(n,p); // A random matrix of (n,p) dimensions.
	for (int i=0; i<n; i++) {
		for (int j=0; j<p; j++) {
            if (A(i,j) < 0.9) {
            	A(i,j) = 0.0;
            } else {
            	A(i,j) = 1.0;
            }
		}
	}
	//cout << "inside get_m " << model << endl;
	return A;
}

mat get_pair_terms(mat &A, mat &x)
{
    /**
	* PairwiseTerms:
    * --------------
    * g = pair-wise combinations of -1.0*a1*a2*x1*x2 + ...
    * g = [n x 1]
    * g[i] =  sum(-1.0*a[j]*a[k]*x[j]*x[k]), where i = (1..n); j,k = (1..p)  // a[j] = [n x 1]; x[j] = [1 x 1]
    * dg[i]/x[j] = sum(-1.0*a[j]*a[k]*x[k]), where i = (1..n); j,k = (1..p); j != k  // a[j] = [n x 1]; x[j] = [1 x 1]
    *
    * OUTPUT:
	* g = [n x 1]
	*/

	int num_samples = A.n_rows;
	int num_features = A.n_cols;

    mat g = randu<mat>(num_samples, 1); // [nx1]
    g.zeros();   // Initialize to zero values.
    for (int j=0; j<num_features; j++) {
    	for (int k=0; k<num_features; k++) {
            g = g + (A.col(j) % A.col(k)) * x(j)*x(k);
    	}
    }
    g = -1.0 * g;
    return g;
}

mat get_measured(mat &A, mat &x, double noise)
{
	/**
	 */
	// y = Ax
	// y = Ax + x*x
    //noise = 0 = no noise; posiive value idicates amount of noise.
    int num_samples = A.n_rows;
    //cout << "num samples " << num_samples << endl;
	mat y;
    mat vnoise = noise*randn<mat>(num_samples,1);

    mat g = get_pair_terms(A, x);
    y = A * x + WPAIR*g;
    y = y + vnoise;
    //cout << "get_measured y " << y << endl;
    return y;
}

double fopt(const std::vector<double> &x, std::vector<double> &grad, void *my_func_data)
{
	/**
* PairwiseTerms:
    * --------------
    * g = pair-wise combinations of -1.0*a1*a2*x1*x2 + ...
    * g = [n x 1]
    * dg/x[j] = [n x 1]  # derivative of g.
    * g[i] =  sum(-1.0*A[j]*A[k]*x[j]*x[k]), where i = (1..n); j,k = (1..p)  // a[j] = [n x 1]; x[j] = [1 x 1]
    * dg[i]/x[j] = sum(-1.0*A[i,j]*A[i,k]*x[k] - 2*a[i,j]*a[i,j]*x[j]), where i = (1..n); j,k = (1..p); j != k  // a[j] = [n x 1]; x[j] = [1 x 1]
    *    A.col(j)  # [nx1]
    *    A.col(k)  # [nx1]
    *    x[k]      # [1x1]
 *
 * f = ( (Ax + g(x)) - yobs )^2
 * df/dx = 2*((Ax + g(x)) - yobs) * (A[j] + dg/dx)  //dg/dx[i] = [nx1]
	 */

    FunctionData f_data = *(FunctionData *)(my_func_data);   //
    mat xb = conv_to< mat >::from(x);  // Make sure dimensons match with the matrix multiplciation.
    mat A = f_data.get_design_matrix();
    int num_samples = A.n_rows;   // 'n' variable;
    int num_features = x.size();  // 'p' variable.
    mat yobs = f_data.get_yobs();

    //DEBUG:  10.4802 earlier; at the specified coordinates, fopt not minimal; gradient not zeros.
//    xb(0) = 0.7831;
//    xb(1) = 0.7984;
    //cout << "DEBUG: xb " << xb << endl;

    mat g = get_pair_terms(A, xb);

    mat ypred = A*xb + WPAIR*g;
    mat diff = ypred - yobs;
    mat xvalue = diff.t()*diff;  // diff = [nx1]
    double xvalue_b = as_scalar(xvalue);
    cout << "DEBUG: fopt " << xvalue_b << endl;



    if (!grad.empty()) {
        mat grad_m = randu<mat>(num_features,1);   // [p x 1]
        grad_m.zeros();

        for (int j=0; j<num_features; j++) {
        	mat dg_pair = randu<mat>(num_samples,1);
        	dg_pair.zeros();
        	for (int k=0; k<num_features; k++) {
        		dg_pair = dg_pair + 2.0*(A.col(j) % A.col(k))*xb(k);  // A.col(j) =[nx1]
        	}
        	//dg_pair = dg_pair + (A.col(j) % A.col(j))*xb(j);
        	dg_pair = -1.0*dg_pair;

        	double dg_x = 2*as_scalar( (A.col(j) + WPAIR*dg_pair).t()  * diff);

        	grad_m(j) = dg_x;


        }
        grad = conv_to< stdvec >::from(grad_m);

//        cout << "Gradient: ";
//        print_row(grad);
    }

    return xvalue_b;
}


int main(int argc, char* argv[])
{

	int N = 1000;  // Number of samples;
	int p = 200;  // Number of features.
//	int N = 100;  // Number of samples;
//	int p = 2;  // Number of features.
//	int N = 10;  // Number of samples;
//	int p = 2;  // Number of features.


	//nlopt::opt opt(nlopt::GN_DIRECT_L, p);  // Global minimization:
	//nlopt::opt opt(nlopt::GN_ISRES, p);  // Global minimization:
	//nlopt::opt opt(nlopt::G_MLSL_LDS, p);  // Global minimization:
	//nlopt::opt opt(nlopt::GN_CRS2_LM, p);  // Global minimization:



	//nlopt::opt local_opt(nlopt::LD_LBFGS, p);
	//opt.set_local_optimizer(local_opt);

	nlopt::opt opt(nlopt::LD_LBFGS, p);       // requires gradient. Works reallly well; much better than MMA.
	//nlopt::opt opt(nlopt::LD_MMA, p);       // requires gradient.
	//nlopt::opt opt(nlopt::LN_NELDERMEAD, p);  // Does not require gradient.

	// Set lower and upper bounds;
	std::vector<double> lb(p);
	std::vector<double> ub(p);
	mat mlb = randu<mat>(p,1);
	mat mub = randu<mat>(p,1);
    mlb.fill(-5.0);
    mub.fill(5.0);
    lb = conv_to< stdvec >::from(mlb);
    ub = conv_to< stdvec >::from(mub);
	opt.set_lower_bounds(lb);
	opt.set_upper_bounds(ub);



	mat model_ref = randu<mat>(p,1);
	model_ref = get_model_ref(p);
	//cout << "model " << model<< endl;

	mat A    = get_design_matrix(model_ref, N, p);
	mat y    = get_measured(A, model_ref, 0.0);
	//mat yobs = get_measured(A, model_ref, 0.1);
	mat yobs = get_measured(A, model_ref, 0.0);

	cout << " A " << A.n_rows << " " << A.n_cols << endl;
	cout << " model_ref " << model_ref.n_rows << " " << model_ref.n_cols << endl;
	cout << " yobs " << yobs.n_rows << " " << yobs.n_cols << endl;


	//cout << "Design matrix: " << A << endl;
//	cout << "true " << y << endl;
//	cout << "obs " << yobs << endl;



	FunctionData f_data;
	f_data.set_values(A, yobs);
	opt.set_min_objective(fopt, &f_data);
	opt.set_xtol_rel(1e-4);


	mat xtemp = randn<mat>(p,1);
	cout << "xinitial: " << xtemp << endl;
	//stdvec x(p);
	stdvec x = conv_to< stdvec >::from(xtemp);
	double minf;
	//cout << "initial x " << x[0] << endl;

	nlopt::result result = opt.optimize(x, minf);



	cout << "minimum: " << x[0] << " " << x[1] << " " << minf << endl;

	print_row(x);
	cout << "model_ref " << model_ref << endl;

}
