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

#include "SMMSolve.h"

// Calculates the cross validated distance of predictions to measurements 
// for a given lambda 


class CSMMCrossValidate 
{
public:
	CSMMCrossValidate(void);
	~CSMMCrossValidate(void);
public:
	void	SetPrecission(double precission=0.001)		{m_precission=precission;};
	void	SetMaxNormIterations(unsigned iter=10000)	{m_max_norm_iterations=iter;};
	CSMMSet &TrainingSet()	{return(m_train_set);};

public:	// Initializing
	void	CreateCrossValidator(unsigned subset_num);	
	void	CreateMaskedCopy(const CSMMCrossValidate & cv, const UIntVec & mask);
public:		
	unsigned CVMatrixCols()	const {return(m_train_set.SubsetTrain(0).MatrixCols());};
public:
	double	 CVDistance(const CNumVec &lambda);
	double	 CVGradient(CNumVec &gradient, const CNumVec &lambda);

public:	// returns average x in m_solver
	CNumVec	GetMeanCVX();
protected:
	double	m_precission;
	unsigned m_max_norm_iterations;
private:
	Vec<CSMMSolve> m_solver;
	CVSMMSet m_train_set;	
};
