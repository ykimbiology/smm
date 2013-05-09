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

#include "NumVec.h"

class CMinimizerBase
{
public:
	CMinimizerBase(void);
	~CMinimizerBase(void);
public:
	double	MinimizeConjugateGradient(CNumVec &startparam, const double precission, unsigned restart_repeats=5); 
	double	MinimizeSteepestDescent(CNumVec &startparam, const double precission); 
	void	TestGradient(const CNumVec &param, double delta);
protected:
	virtual double MinimizerDistance(const CNumVec & param)=0;
	virtual double MinimizerGradient(const CNumVec & param, CNumVec &p_gradient)=0;
	virtual void   AdjustParameters(CNumVec &param)	{};
private:
	double LineMinimizer(CNumVec &position, double dist0, const CNumVec &direction, bool roughly);
	double LineDistance(double x);
	const CNumVec *LM_start_pos;
	const CNumVec *LM_direction;
};
