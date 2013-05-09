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
#include "SMMOptimizeLambda.h"

#include "math.h"

CSMMOptimizeLambda::CSMMOptimizeLambda(void)
{
	SetLambdaRange();
	SetOnlyLambdaDescent();
}

CSMMOptimizeLambda::~CSMMOptimizeLambda(void)
{
}

void CSMMOptimizeLambda::SetLambdaRange(double min, double start, double max)
{
	m_lambda_min=min; 
	m_lambda_start=start; 
	m_lambda_max=max;
};

void CSMMOptimizeLambda::SetLambdaGroups(const Vec<UIntVec> & groups)
{
	m_lambda_groups=groups;
}

void CSMMOptimizeLambda::ResetLambda()
{
	m_param.resize(m_lambda_groups.size());
	m_param=log(m_lambda_start);
	m_lambda.resize(CVMatrixCols());
	CalcLambdaFromParameters(m_param);
}


void CSMMOptimizeLambda::FindStartLambda(bool offset)
{
	clog_detail.push_back(NONE);

	CSMMOptimizeLambda smm;
	smm.SetOnlyLambdaDescent(false);
	smm.SetLambdaRange(m_lambda_min, m_lambda_start, m_lambda_max);
	smm.SetPrecission(m_precission);

	// Find start parameters for each lambda_group
	for(unsigned g=0; g<m_lambda_groups.size(); g++)
	{
		UIntVec mask=m_lambda_groups[g];
		if(offset)
            mask.push_back(0);
		smm.CreateMaskedCopy(*this,mask);

		UIntVec group;
		for(unsigned n=0; n<m_lambda_groups[g].size(); n++)
			group.push_back(n);
		Vec<UIntVec> groups;
		groups.push_back(group);
		smm.SetLambdaGroups(groups);

		smm.ResetLambda();
		double dist=smm.MinimizeSteepestDescent(smm.m_param, m_precission);
		m_param[g]=smm.m_param[0];
	}
	clog_detail.pop_back();
	if(clog_detail.back()>MEDIUM)
		clog << endl << "Start Parameter\t\t\t" << m_param;
	CalcLambdaFromParameters(m_param);
}


double	CSMMOptimizeLambda::OptimizeLambda()
{
	m_lambda.resize(CVMatrixCols());

	MinimizeSteepestDescent(m_param,m_precission);

	CalcLambdaFromParameters(m_param);
	return(CVDistance(m_lambda));
}

void	CSMMOptimizeLambda::CalcLambdaFromParameters(const CNumVec &param)
{
	m_lambda=0;

	const double param_max=log(m_lambda_max);
	for(unsigned p=0; p<param.size(); p++)
	{
		double val;
		if(param[p]>param_max)
			val=m_lambda_min+m_lambda_max;
		else
			val=m_lambda_min+exp(param[p]);

		UIntVec lambda_group=m_lambda_groups[p];
		for(unsigned i=0; i<lambda_group.size(); i++)
			m_lambda[lambda_group[i]]=val;
	}
}

void   CSMMOptimizeLambda::AdjustParameters(CNumVec &param)
{
	const double param_max=log(m_lambda_max);
	for(unsigned p=0; p<param.size(); p++)
	{
		if(param[p]>param_max)
			param[p]=param_max;
	}
}


double CSMMOptimizeLambda::MinimizerDistance(const CNumVec & param)
{
  //cout <<"d";
	CalcLambdaFromParameters(param);
	double dist=CVDistance(m_lambda);

	if(clog_detail.back()>MEDIUM)
		clog << endl << "Dist\t" << dist << "\tparam\t" << param;
	return(dist);
}

double CSMMOptimizeLambda::MinimizerGradient(const CNumVec & param, CNumVec &p_gradient)
{
  //cout << endl << "G";
	CalcLambdaFromParameters(param);
	CNumVec l_gradient(m_lambda.size());
	double dist=CVGradient(l_gradient,m_lambda);

	p_gradient.resize(param.size());
	for(unsigned p=0; p<param.size(); p++)
	{
		const UIntVec & lambda_group=m_lambda_groups[p];
		p_gradient[p]=0;
		for(unsigned i=0; i<lambda_group.size(); i++)
			p_gradient[p]+=m_lambda[lambda_group[i]]*l_gradient[lambda_group[i]];
	}

	if(m_only_lambda_descent && p_gradient.size()>1)	// Disallowing anything but descent for the lambda values actually makes the search for a minimum more robust - for a starting point found with FindStartLambda
	{
		for(unsigned n=0; n<p_gradient.size(); n++)
		{
			if(p_gradient[n]<0)
				p_gradient[n]=0;
		}
	}

	if(clog_detail.back()>MEDIUM)
	{
		clog << endl << "Dist\t"  << dist << "\tParam\t"  << param;
		clog << endl << "Dist\t"  << dist << "\tGrad\t"  << p_gradient;
	}
	return(dist);
}
