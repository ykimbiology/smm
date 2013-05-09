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
#include "SMMSet.h"

// Calculates the optimal x given a CSMMSet and  lambda 


class CSMMSolve
{
public:
	CSMMSolve(void);
	~CSMMSolve(void);
public:
	void InitSolver(const CSMMSet &set);
	void SolveX(const CSMMSet &set, const CNumVec & lambda, bool calc_dx_dlambda, double precission, unsigned max_iterations);
public:
	const CNumVec &GetX()			const {return(m_x);};
	const CNumMat &Getdxdlambda()	const {return(m_dx_dlambda);};
private: 
	CNumVec m_x;
	CNumMat m_dx_dlambda;
	CNumMat m_tAA;
	CNumVec	m_y2;
};