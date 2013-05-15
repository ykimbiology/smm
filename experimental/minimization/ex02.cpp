

/**
 * C++ version of minimization.
 *
 * compile: g++ ex02.cpp -o ex02 -lnlopt -lm
 */
#include <iostream>
using namespace std;

#include <nlopt.hpp>


typedef struct {
    double a, b;
} my_constraint_data;


//double myfunc(const std::vector<double> &x, std::vector<double> &grad, void *my_func_data)
//{
//    if (!grad.empty()) {
//        grad[0] = 0.0;
//        grad[1] = 0.5 / sqrt(x[1]);
//    }
//    return sqrt(x[1]);
//}

// To be passed as additional to the minmizing function.
//std::vector<double> x(2);
//std::vector<double> k(2);

double myfunc(const std::vector<double> &x, std::vector<double> &grad, void *my_func_data)
{
	// y = (x1 - 3.0)^2 + x2^2
	double xoffset;
    xoffset = 3.0;
    //std::vector<double> f_data;
	//f_data = (std::vector<double>) (my_func_data);
    //double f_data;
    //f_data = (double) my_func_data;
    //f_data = (double) (&my_func_data);
    //f_data = *reinterpret_cast<double *> (&my_func_data);
	//xoffset = (double) my_func_data[0];
    //cout << "f_data " << f_data << endl;
    //double f_data = *double (my_func_data);
    //double& f_data = static_cast<double&>(*my_func_data); // does not work.

    //double f_data = *(double *)(my_func_data);  // This worked!! http://stackoverflow.com/questions/7949761/c-error-void-is-not-a-pointer-to-object-type
    //cout << "f_data " << f_data << endl;

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

double myconstraint(const std::vector<double> &x, std::vector<double> &grad, void *data)
{
    my_constraint_data *d = reinterpret_cast<my_constraint_data*>(data);
    double a = d->a, b = d->b;
    if (!grad.empty()) {
        grad[0] = 3 * a * (a*x[0] + b) * (a*x[0] + b);
        grad[1] = -1.0;
    }
    return ((a*x[0] + b) * (a*x[0] + b) * (a*x[0] + b) - x[1]);
}


int main(int argc, char* argv[])
{

nlopt::opt opt(nlopt::LD_MMA, 2);

//std::vector<double> lb(2);
//lb[0] = -HUGE_VAL; lb[1] = -10;
//opt.set_lower_bounds(lb);

//opt.set_min_objective(myfunc, NULL);

std::vector<double> f_data(2);
f_data[0] = 9.1; f_data[1] = 10.1;

//double f_data;
//f_data = 1.1;

//opt.set_min_objective(myfunc, f_data);
opt.set_min_objective(myfunc, &f_data);


//my_constraint_data data[2] = { {2,0}, {-1,1} };
//opt.add_inequality_constraint(myconstraint, &data[0], 1e-8);
//opt.add_inequality_constraint(myconstraint, &data[1], 1e-8);

opt.set_xtol_rel(1e-4);

std::vector<double> x(2);
x[0] = 1.234; x[1] = 5.678;
double minf;
nlopt::result result = opt.optimize(x, minf);

cout << "minimum: " << x[0] << " " << x[1] << " " << minf << endl;

}





