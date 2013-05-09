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
#include "SMMCrossValidate.h"
#include <algorithm>

CSMMCrossValidate::CSMMCrossValidate(void)
{
	SetPrecission();
	SetMaxNormIterations();
}

CSMMCrossValidate::~CSMMCrossValidate(void)
{
}



void CSMMCrossValidate::CreateCrossValidator(unsigned subset_num)
{
	m_train_set.GenerateSubsets(subset_num);
	m_solver.resize(m_train_set.SubsetNum());
	for(unsigned c=0; c<m_train_set.SubsetNum(); c++)
		m_solver[c].InitSolver(m_train_set.SubsetTrain(c));
}

void	CSMMCrossValidate::CreateMaskedCopy(const CSMMCrossValidate & cv, const UIntVec & mask)
{
	m_train_set.CreateMaskedCopy(cv.m_train_set,mask);
	m_solver.resize(m_train_set.SubsetNum());
	for(unsigned c=0; c<m_train_set.SubsetNum(); c++)
		m_solver[c].InitSolver(m_train_set.SubsetTrain(c));
}


double CSMMCrossValidate::CVDistance(const CNumVec &lambda)
{

	double dist=0;
	unsigned count=0;
	for(unsigned c=0; c<m_train_set.SubsetNum(); c++)
	{
		const CSMMSet &train=m_train_set.SubsetTrain(c);
		CSMMSet		  &blind=m_train_set.SubsetBlind(c);
		CSMMSolve	  &solver=m_solver[c];
		
		solver.SolveX(train,lambda,false,m_precission, m_max_norm_iterations);
		dist +=blind.XDistance(solver.GetX());
		count+=blind.ElementNumber();
	}
	dist/=count;

	if(clog_detail.back()>DETAILED) 
		clog << endl << "Dist\t"  << dist << "\tLambda\t"  << lambda;
	return dist;
}



double CSMMCrossValidate::CVGradient(CNumVec &gradient, const CNumVec &lambda)
{
	assert(gradient.size()==lambda.size());

	double dist=0;
	unsigned count=0;
	gradient=0;

	for(unsigned c=0; c<m_train_set.SubsetNum(); c++)
	{
		const CSMMSet &train=m_train_set.SubsetTrain(c);
		CSMMSet		  &blind=m_train_set.SubsetBlind(c);
		CSMMSolve	  &solver=m_solver[c];

		solver.SolveX(train,lambda,true,m_precission, m_max_norm_iterations);
		const CNumMat &dxdl=solver.Getdxdlambda();
		const CNumVec &x   =solver.GetX();

		CNumVec ddistdx;
		dist+=blind.XGradient(x,ddistdx);
		count+=blind.ElementNumber();

		CNumVec g;
		g.SetToProduct(dxdl,ddistdx);
		gradient+=g;
	}
	gradient*=1.0/count;
	dist/=count;
	if(clog_detail.back()>DETAILED) 
	{
		clog << endl << "Dist\t"  << dist << "\tLambda\t"  << lambda;
		clog << endl << "Dist\t"  << dist << "\tGrad\t"  << gradient;
	}
	return dist;
}


CNumVec	CSMMCrossValidate::GetMeanCVX()
{
	CNumVec cvx=m_solver[0].GetX();
	for(unsigned c=1; c<m_train_set.SubsetNum(); c++)
		cvx+=m_solver[c].GetX();
	cvx*=1.0/m_train_set.SubsetNum();
	return(CNumVec(cvx));
}

