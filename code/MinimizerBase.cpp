/*
www.mhc-pathway.net/smm
Original file by Bjoern Peters.

This software is provided 'as-is', without any express or implied 
warranty. In no event will the authors be held liable for any 
damages arising from the use of this software.

Permission is granted to anyone to use this software for any 
purpose, including commercial applications, and to alter it and 
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must 
not claim that you wrote the original software. If you use this 
software in a product, an acknowledgment in the product documentation 
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source 
distribution.
*/
#include "stdafx.h"
#include "MinimizerBase.h"
#include "math.h"


#include "BPException.h"

CMinimizerBase::CMinimizerBase(void)
{
}

CMinimizerBase::~CMinimizerBase(void)
{
}

// ex_ofstream _log;

double CMinimizerBase::LineDistance(double x)
{
	CNumVec pos;
	pos=*LM_direction;
	pos*=x;
	pos+=*LM_start_pos;
	double dist=MinimizerDistance(pos);
//	_log << x << "\t" << dist << endl;
	return(dist);
}


double CMinimizerBase::LineMinimizer(CNumVec &position, double dist0, const CNumVec &direction, bool roughly)
{
	LM_direction=&direction;
	LM_start_pos=&position;

	double pleft, pmin, pright, pnext;
	double dleft, dmin, dright, dnext;

	pleft=0;
	dleft=dist0;
	pnext=1;
	dnext=LineDistance(pnext);

	if(dleft<dnext) // minimum is less than precision (= gradient norm) deep
		return(dist0);

	// try to bracket a minimum along "direction" 
	pmin=pleft;
	dmin=dleft;
	do
	{
		pleft=pmin;
		dleft=dmin;

		pmin=pnext;
		dmin=dnext;

		pnext*=2;
		dnext=LineDistance(pnext);
	}
	while(dmin>dnext);

	if(roughly && pmin>50) // stops exact determination if minimum is far from optimal value
	{
		CNumVec p=direction;
		p*=pleft;
		position+=p;
		return(dmin);
	}
	pright=pnext;
	dright=dnext;

	// Further bracket minimum using "GoldenCut"
	// Ideally this should be replaced by some function taking into account the
	// pretty good reproducible function along the line minimization

	bool longer_interval_right=true;

	while(pright-pleft>1.0)
	{
		if(longer_interval_right)
		{
			pnext=pmin+(pright-pmin)*0.38;
			dnext=LineDistance(pnext);

			if(dnext<dmin)
			{
				pleft=pmin;
				dleft=dmin;
				
				pmin=pnext;
				dmin=dnext;
			}
			else
			{
				pright=pnext;
				dright=dnext;
				longer_interval_right=false;
			}
		}
		else
		{
			pnext=pmin+(pleft-pmin)*0.38;
			dnext=LineDistance(pnext);

			if(dnext<dmin)
			{
				pright=pmin;
				dright=dmin;

				pmin=pnext;
				dmin=dnext;
			}
			else
			{
				pleft=pnext;
				dleft=dnext;
				longer_interval_right=true;
			}
		}
	}
	// Change 'position to Minimum position 
	CNumVec p=direction;
	p*=pmin;
	position+=p;
	return(dmin);
}

double	CMinimizerBase::MinimizeConjugateGradient(CNumVec &param, const double min_precission, unsigned restart_repeats)
{
	
	// First move broadly towards the minimum:
	AdjustParameters(param);
	CNumVec new_direction;
	double last_dist=MinimizerGradient(param,new_direction);
	double norm=new_direction.ScalarProduct(new_direction);
	if(norm==0.0)
		return(last_dist);
	double normalization= -sqrt(min_precission/norm);
	new_direction*=normalization;
	last_dist=LineMinimizer(param,last_dist,new_direction,true);

	// The following implements the Fletcher Reeves Conjugate Gradient algorithm
	unsigned count=0;
	CNumVec last_direction;
	double new_dist;
	do
	{
		// Adjust Parameters can change param values without changing dist(param)
		AdjustParameters(param);
		
		if(count%restart_repeats==0) // Every restart_repeats iterations, the normal gradient is used
		{
			last_dist=MinimizerGradient(param,new_direction);
			new_direction*=-1;
		}
		else
		{
			CNumVec gradient;
			last_dist=MinimizerGradient(param,gradient);
			double beta = gradient.ScalarProduct(gradient) / last_direction.ScalarProduct(last_direction);

			new_direction=last_direction;
			new_direction*=beta;
			new_direction-=gradient;
		}
		last_direction=new_direction;

		norm=new_direction.ScalarProduct(new_direction);
		if(norm==0.0)
			return(last_dist);
		normalization= sqrt(min_precission/norm);

		new_direction*=normalization;

		new_dist=LineMinimizer(param,last_dist,new_direction,false);

		count++;
	}
	while(last_dist-new_dist>min_precission || count<1);
//	exit(0);
	return(new_dist);

}

double	CMinimizerBase::MinimizeSteepestDescent(CNumVec &param, const double min_precission)
{
	unsigned count=0;
//	_log.open("log minimizer.txt");
	CNumVec gradient;
	double last_dist, new_dist;
	do
	{
		// Adjust Parameters can change param values without changing dist(param)
		AdjustParameters(param);
		last_dist=MinimizerGradient(param,gradient);
		double gradient_norm=gradient.ScalarProduct(gradient);
		if(gradient_norm==0.0)
			return(last_dist);
		double normalization= -sqrt(min_precission/gradient_norm);
		gradient*=normalization;

//		_log << "gradient  normalized\t\t" << gradient << endl;
		new_dist=LineMinimizer(param,last_dist,gradient,true);
//		_log << endl;
		count++;
	}
	while(last_dist-new_dist>min_precission || count<2); //  || (count<2 && param.size()>1));
//	exit(0);
	return(new_dist);
}


void CMinimizerBase::TestGradient(const CNumVec & param, double delta)
{
	ex_ofstream out("Gradient Test.txt");
	CNumVec gradient;
	double dist0=MinimizerGradient(param,gradient);
	out << "param:\t" << param << endl;
	out << "analytical gradient:\t" << gradient<< endl;
	out << "numerical gradient:\tdelta: " << delta <<"\t";

	for (unsigned p=0; p<param.size(); p++)
	{
		CNumVec pos=param;
		pos[p]+=delta;
		double d=MinimizerDistance(pos);
		out << (d-dist0)/delta << "\t";
		out.flush(); 
	}
}


