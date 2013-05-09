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
#pragma once
#include "SMMCrossValidate.h"
#include "MinimizerBase.h"


class CSMMOptimizeLambda : public CSMMCrossValidate, private CMinimizerBase
{
public:
	CSMMOptimizeLambda(void);
	~CSMMOptimizeLambda(void);

public:		// Set groups and allowed values for the lambda vector
	void	SetLambdaRange(double min=1e-1, double start=1e4, double max=1e6);
	void	SetLambdaGroups(const Vec<UIntVec> & groups);
	void	SetOnlyLambdaDescent(bool set=true)	{m_only_lambda_descent=set;};

public:		// Functions to find the optimal lambda values
	void	ResetLambda();
	void	FindStartLambda(bool offset);
	double	OptimizeLambda();
public:
    const CNumVec	&GetLambda()	const	{return(m_lambda);};
	bool	OneDimensionalOptimization()	{return(m_lambda_groups.size()==1);};
protected:
	double		m_lambda_max;
	double		m_lambda_min;
	double		m_lambda_start;
private:
	CNumVec		m_lambda;
private:	// Param is mapped to lambda by {lambda(group)}=e^param(group)+lambda_min. 
			// By minimizing param instead of lambda, there are no positivity constraints
			// and the "landscape" of lambdas is better mapped 
	void	CalcLambdaFromParameters(const CNumVec &param);
	CNumVec			m_param;
	Vec<UIntVec>	m_lambda_groups;
private:
	virtual double MinimizerDistance(const CNumVec & param);
	virtual double MinimizerGradient(const CNumVec & param, CNumVec &p_gradient);
	virtual void   AdjustParameters(CNumVec &param);
private:
	bool m_only_lambda_descent;
};
