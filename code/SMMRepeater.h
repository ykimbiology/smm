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

#include "SMMOptimizeLambda.h"


class CSMMRepeater : public CSMMOptimizeLambda
{
public:
	CSMMRepeater(void);
	~CSMMRepeater(void);
public:
	void XMLParameters(TiXmlNode *node);
	void SetRepetitions(unsigned train_repeats=10, unsigned cv_num=10)	{m_train_repeats=train_repeats; m_cv_num=cv_num;};
	void SetInitRand(unsigned rand=1)									{m_srand=rand;};
public:
	void TrainRepeater(bool solution_possible=true);
	const CNumVec	GetMeanSolution()	{return(m_solutions.back());};
protected:
	virtual double Train();
private:
	unsigned m_srand;
	unsigned m_train_repeats;
	unsigned m_cv_num;
	Vec<CNumVec> m_solutions;
	Vec<CNumVec> m_lambdas;
	Vec<double>	 m_dists;
};
