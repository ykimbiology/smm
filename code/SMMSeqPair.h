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
#include "SeqPair.h"
#include "SMMRepeater.h"


class CSMMSeqPair :	public CSeqPair
{
public:
	CSMMSeqPair(int min_count);
	~CSMMSeqPair(void);
public:
	void TrainPair(CSeqSet &set);
	void XMLParameters(TiXmlNode *node);
	//void SetPairCoefCriteria(unsigned min_count=5, double max_percent_disagree=0.4)	{m_min_count=min_count; m_max_percent_disagree=max_percent_disagree;};
	void SetPairCoefCriteria(int min_count, double max_percent_disagree=0.4)
	{
		m_min_count=min_count;
		m_max_percent_disagree=max_percent_disagree;
		// DEBUG:
		cout << "CSMMSeqPair: min_count " << m_min_count << endl;
	};

	void SetIndividualLambdas(bool individual_lambdas=false)	{m_individual_lambdas=individual_lambdas;};
protected:
	void ConvertSolution();
	void GenerateLambdaGroups(bool individual_lambda);
	CSMMRepeater m_smm;
	double m_matrix_offset;
protected:
	bool m_individual_lambdas;
	int m_min_count;
	double m_max_percent_disagree;
};
