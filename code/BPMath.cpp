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
#include "BPMath.h"


#include <gsl/gsl_statistics.h>

void	LinearRegression(const CNumVec &x, const CNumVec &y, double &m, double &b)
{
	double sum_x=0; 
	double sum_y=0; 
	double sum_xy=0; 
	double sum_xx=0; 
	
	unsigned n;
	for(n=0; n<x.size(); n++)
	{
		sum_x +=x[n];
		sum_y +=y[n];
		sum_xy+=x[n] * y[n];
		sum_xx+=x[n] * x[n];
	}
	m=(n*sum_xy-sum_x*sum_y)/(n*sum_xx-sum_x*sum_x);
	b=(sum_y*sum_xx-sum_x*sum_xy)/(n*sum_xx-sum_x*sum_x);
}

// Berechnet den Korrelationskoeffizienten R 

double CorrelationCoefficient(const CNumVec &x, const CNumVec &y)
{
	assert(x.size()==y.size());
//	return(gsl_stats_covariance(x.m_vec->data,1,y.m_vec->data,1,x.size()));

	double sumx=0, sumx2=0, sumy=0, sumy2=0, sumxy=0;
	unsigned n; // E.K. gcc changed the scope
	// E.K. for the variables in the "for" loop define them before if you want to return them later
	for(n=0; n<x.size(); n++)
	{
		sumx+=x[n];
		sumy+=y[n];
		sumx2+=x[n]*x[n];
		sumy2+=y[n]*y[n];
		sumxy+=x[n]*y[n];
	}
	return((n*sumxy-sumx*sumy)/sqrt((n*sumx2-sumx*sumx)*(n*sumy2-sumy*sumy)));

}

double Stdev(const DoubleVec &val)
{
	double mean=0; 
	double sq=0;
	for(DoubleVec::const_iterator it=val.begin(); it< val.end(); it++)
	{
		mean+=*it;
		sq+=*it*(*it);
	}
	double std= sqrt((val.size()*sq-mean*mean)/((val.size()-1)*val.size()));
	mean/=val.size();
	sq/=val.size();
	return(std);
}


double ROC(const DoubleVec & _score_pos, const DoubleVec & _score_neg, DoubleVec &sens_vec, DoubleVec &spec_vec, DoubleVec &cutoffs)
{
	if(_score_pos.size()==0 || _score_neg.size()==0)
	{
		cout << "Error: No positive or negative scores";
		return(-1);
	}
	sens_vec.clear();
	spec_vec.clear();
	cutoffs.clear();

	DoubleVec score_pos=_score_pos;
	DoubleVec score_neg=_score_neg;
	std::sort(score_pos.begin(),score_pos.end());
	std::sort(score_neg.begin(),score_neg.end());

	double integral=0;
	sens_vec.push_back(0);
	spec_vec.push_back(1);
	cutoffs.push_back(min(score_neg.front(),score_pos.front()));


	DoubleVec::const_iterator cut_neg=score_neg.begin();
	DoubleVec::const_iterator cut_pos=score_pos.begin();

	while(cut_pos<score_pos.end())
	{

		while(cut_neg<score_neg.end()) // Move cut_neg to first value >= cut_pos
		{
			if(*cut_neg<*cut_pos)
				cut_neg++;
			else
				break;
		}

		unsigned TP=unsigned(cut_pos-score_pos.begin());
		unsigned FP=unsigned(cut_neg-score_neg.begin());
		unsigned TN=score_neg.size()-FP;
		unsigned FN=score_pos.size()-TP;
		double sens=double(TP)/score_pos.size();
		double spec=double(TN)/score_neg.size();
		integral+=(sens-sens_vec.back())*(spec+spec_vec.back())/2.0;
		sens_vec.push_back(sens);
		spec_vec.push_back(spec);
		cutoffs.push_back(*cut_pos);

		double last_cutoff=*cut_pos;
		while(cut_pos<score_pos.end()) // Advance cut_pos while = last cutoff !!Change that to move cut_pos to first value >= cut_neg, perfect symmetry
		{
			if(*cut_pos==last_cutoff)
				cut_pos++;
			else
				break;
		}
		while(cut_neg<score_neg.end()) // Advance cut_pos while = last cutoff
		{
			if(*cut_neg==last_cutoff)
				cut_neg++;
			else
				break;
		}

		TP=unsigned(cut_pos-score_pos.begin());
		FP=unsigned(cut_neg-score_neg.begin());
		TN=score_neg.size()-FP;
		FN=score_pos.size()-TP;

		sens=double(TP)/score_pos.size();
		spec=double(TN)/score_neg.size();
		integral+=(sens-sens_vec.back())*(spec+spec_vec.back())/2.0;
		sens_vec.push_back(sens);
		spec_vec.push_back(spec);

		double cp=score_pos.back();
		double cn=score_neg.back();
		if(cut_pos<score_pos.end())
			cp=*cut_pos;
		if(cut_neg<score_neg.end())
			cn=*cut_neg;
		cutoffs.push_back(min(cp,cn));
	}
	integral+=(1-sens_vec.back())*(spec_vec.back()+0)/2.0;
	sens_vec.push_back(1);
	spec_vec.push_back(0);
	cutoffs.push_back(max(score_neg.back(),score_pos.back()));
	return(integral);
}

double AUC(const DoubleVec & score_pos,const DoubleVec &  score_neg)
{
	DoubleVec sens,spec, cutoffs;
	return(ROC(score_pos,score_neg,sens,spec,cutoffs));
}

void ROCFile(const DoubleVec & score_pos,const DoubleVec &  score_neg, ofstream &out)
{
	DoubleVec sens,spec, cutoffs;
	double auc=ROC(score_pos,score_neg,sens,spec,cutoffs);
	out << "AUC: \t" << auc << "\n";
	out << "CutOff\t1-spec\tSens\n";
	for(unsigned i=0; i<sens.size(); i++)
		out << cutoffs[i] << "\t" << 1- spec[i] << "\t" << sens[i] << "\n";
}



/*

double mean(const DoubleVec &val)
{
	assert(val.size()>0);
	double sum=0; 
	for(DoubleVec::const_iterator it=val.begin(); it< val.end(); it++)
		sum+=*it;
	return(sum/val.size());
}

double correlation(const DoubleVec &x, const DoubleVec &y)
{
}




double DevAUCByNoise(const DoubleVec & score_pos,const DoubleVec &  score_neg, int repeats, double dev_noise)
{
/*
	DoubleVec auc;
	StochasticLib lib(0);
	cout << "Calc Dev AUC:\n";
	double mean=0;
	for(int sec=0; sec<repeats; sec++)
	{
		DoubleVec p;
		for(unsigned int i=0; i<score_pos.size(); i++)
			p.push_back(score_pos[i]+lib.Normal(0,dev_noise));

		DoubleVec n;
		for(i=0; i<score_neg.size(); i++)
			n.push_back(score_neg[i]+lib.Normal(0,dev_noise));
		auc.push_back(AUC(p,n));
		mean+=auc.back();
	}
	mean/=repeats;
	double dev=stdev(auc);
	cout << "Dev: \t" << dev << "Mean\t" << mean << "\n";
*//*
	double dev;
	return(dev);
}


double DevAUC(const DoubleVec & score_pos,const DoubleVec &  score_neg, int cv_num)
{
	DoubleVec auc;
	cout << "Calc Dev AUC:\n";
	for(int sec=0; sec<cv_num; sec++)
	{
		DoubleVec p;
		for(unsigned int i=0; i<score_pos.size(); i++)
			p.push_back(score_pos[rand()%score_pos.size()]);

		DoubleVec n;
		for(i=0; i<score_neg.size(); i++)
			n.push_back(score_neg[rand()%score_neg.size()]);
		auc.push_back(AUC(p,n));
	}
	double dev=stdev(auc);
	cout << "Dev: \t" << dev << "\n";
	return(dev);
}



*/
