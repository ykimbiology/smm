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
#include "SMMSolve.h"

#include "BPException.h"
#include "math.h"

CSMMSolve::CSMMSolve(void)
{
}

CSMMSolve::~CSMMSolve(void)
{
}


void CSMMSolve::InitSolver(const CSMMSet &set)
{
	m_tAA.SetToProduct(set.GetMatrix(),set.GetMatrix());
	m_y2=set.GetMeasurements();
}


void CSMMSolve::SolveX(const CSMMSet &set, const CNumVec & lambda, bool calc_dx_dlambda, double precission, unsigned max_iterations)
{
	const CNumMat			&A		=set.GetMatrix();
	const CNumVec			&y		=set.GetMeasurements();
	const Vec<INEQUALITY>	&ineq	=set.GetInequalities();

	CNumVec ypred;

	// InitSolver has to be called first
	assert(m_tAA.NumCols()==lambda.size());
	
	CNumMat tAA_lam=m_tAA;
	for(unsigned int n=0; n<lambda.size(); n++)
        tAA_lam(n,n)+=lambda[n];

	CNumMat tAA_lam_inv;
	tAA_lam_inv.SetToInverse(tAA_lam);

	CNumMat inverse;
	inverse.SetToProduct(tAA_lam_inv,A,false,true);

	if(!set.InequalitiesPresent())
	{
		m_x.SetToProduct(inverse,y);
		if(calc_dx_dlambda)
		{
			m_dx_dlambda.resize(lambda.size(),lambda.size());
			for(unsigned r=0; r<lambda.size(); r++)
				for(unsigned c=0; c<lambda.size(); c++)
					m_dx_dlambda(c,r)=-tAA_lam_inv(r,c)*m_x[c];
		}
		return;
	}
	
	// Inequalities are present

	const unsigned	MAX_ITERATIONS	=	10000;
	const double	MAX_DIFFERENCE	=	1e-6;
	const double    ABSOLUTE_PRECISSION = precission*1e-3;

	m_x.SetToProduct(inverse,m_y2);
	unsigned iteration=0;
	double relative_difference;
	
	CNumVec last_x;
	do
	{
		ypred.SetToProduct(A,m_x);
		m_y2=y;
		// Adjust m_y2 to ypred where allowed by inequality
		for(unsigned m=0; m<y.size(); m++)
		{
			if(ineq[m]!=EQUAL)
			{
				if(ineq[m]==GREATER)
				{
					if(ypred[m]>y[m])
						m_y2[m]=ypred[m];
				}
				else if(ypred[m]<y[m])
					m_y2[m]=ypred[m];
			}
		}

		last_x=m_x;
		m_x.SetToProduct(inverse,m_y2);

		// Evaluate difference between new m_x and last_x
		relative_difference=0;
		for(unsigned n=0; n<m_x.size(); n++)
		{
			double max_abs=max(fabs(m_x[n]),fabs(last_x[n]));
			if(max_abs<ABSOLUTE_PRECISSION)
				continue;
			double rdiff=fabs(m_x[n]-last_x[n])/max_abs;
			if(rdiff>relative_difference)
				relative_difference=rdiff;
		}

		if(++iteration==MAX_ITERATIONS && relative_difference>MAX_DIFFERENCE)
		{
			BPException e("No norm conversion for x=inverse(y_opt) with inequalities");
			e.m_message  << endl << "Iteration"		<< "\t" << MAX_ITERATIONS;
			e.m_message  << endl << "max difference"<< "\t" << relative_difference;
			e.m_message  << endl << "last x"		<< "\t"	<< last_x;
			e.m_message  << endl << "x"				<< "\t"	<< m_x;
			last_x-=m_x;
			e.m_message  << endl << "diff"			<< "\t"	<< last_x;
			e.m_message  << endl << "y"				<< "\t"	<< y;
			e.m_message  << endl << "ypred"			<< "\t"	<< ypred << endl;
			throw(e);
		}
	}
	while(relative_difference>MAX_DIFFERENCE);

	if(clog_detail.back()>DETAILED) 
		clog << endl << "Solve(lambda)\tIterations:\t" << iteration <<"\tm_x:\t" << m_x;
	
	if(calc_dx_dlambda)
	{
		ypred.SetToProduct(A,m_x);
		CNumMat FA(A.NumRows(),A.NumCols());
		FA=0;
		for(unsigned m=0; m<y.size(); m++)
			if((ineq[m]==GREATER && ypred[m]>y[m]) || (ineq[m]==SMALLER && ypred[m]<y[m]))
				for(unsigned n=0; n<A.NumCols(); n++)
					FA(m,n)=A(m,n);
		CNumMat tAFA;
		tAFA.SetToProduct(A,FA);
		tAA_lam-=tAFA;
		tAA_lam_inv.SetToInverse(tAA_lam);
		m_dx_dlambda.resize(lambda.size(),lambda.size());
		for(unsigned r=0; r<lambda.size(); r++)
			for(unsigned c=0; c<lambda.size(); c++)
				m_dx_dlambda(c,r)=-tAA_lam_inv(r,c)*m_x[c];
	}
}

