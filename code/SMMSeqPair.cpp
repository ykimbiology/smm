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
#include "BPException.h"
#include "SMMSeqPair.h"

CSMMSeqPair::CSMMSeqPair(int min_count)
{
	cout << "Begin: CSMMSeqPair" << endl;
	SetPairCoefCriteria(min_count);
	SetIndividualLambdas();
}

CSMMSeqPair::~CSMMSeqPair(void)
{
}

void CSMMSeqPair::XMLParameters(TiXmlNode *node)
{
	if(node->FirstChild("IndividualLambdas"))
		SetIndividualLambdas(ToBool(node->FirstChild("IndividualLambdas")));
	if(node->FirstChild("PairCoefCriteria"))
		SetPairCoefCriteria(ToUInt(node->FirstChild("PairCoefCriteria")->FirstChild("MinCount")),ToDouble(node->FirstChild("PairCoefCriteria")->FirstChild("MaxDisagrement")));
	if(node->FirstChild("SMMParameters"))
		m_smm.XMLParameters(node->FirstChild("SMMParameters"));

}



void CSMMSeqPair::GenerateLambdaGroups(bool individual_lambda)
{
	Vec<UIntVec> lambda_groups;
	if(individual_lambda)
	{
		for(unsigned c=0; c<m_pair_coef.size(); c++)
		{
			UIntVec group;
			group.push_back(c+1);
			lambda_groups.push_back(group);
		}
	}
	else
	{
		UIntVec group;
		for(unsigned c=0; c<m_pair_coef.size(); c++)
			group.push_back(c+1);
		lambda_groups.push_back(group);
	}
	m_smm.SetLambdaGroups(lambda_groups);
}


void CSMMSeqPair::ConvertSolution()
{
	const CNumVec &solution=m_smm.GetMeanSolution();

	m_matrix[0]=m_matrix_offset+solution[0];

	for(unsigned i=0; i<m_pair_coef.size(); i++)
		m_pair_coef[i].val=solution[1+i];
}



void CSMMSeqPair::TrainPair(CSeqSet &set)
{
	if(clog_detail.back()>NONE)
	{
		clog << endl << "CPredSMM::TrainPair(" << set.GetName()<<")";
		clog << endl << "Elements in Set: " << set.ElementNumber();
		clog << endl << "Lambda Grouping\t";
		if(m_individual_lambdas)
			clog << "INDIVIDUAL_LAMBDAS";
		else 
			clog << "ONE_GROUP";
		clog << endl << "Min Count\t" << m_min_count;
		clog << endl << "Max Disagreement %\t" << m_max_percent_disagree;
	}

	assert(m_matrix.size()==CLetter::AlphabetSize()*set.SeqLength()+1);
	m_matrix_offset=m_matrix[0];

	m_pair_coef.clear();
	IdentifyPairCoef(set,m_min_count);

	Predict(set);
	assert(m_max_percent_disagree<=0.5);
	DropPairCoef(set,m_max_percent_disagree);

	unsigned count=m_pair_coef.size();
	if(m_pair_coef.size()>0)
	{
		GenerateLambdaGroups(m_individual_lambdas);
		set.ConvertToSMMSet(m_smm.TrainingSet(),false,m_pair_coef);
		m_smm.TrainRepeater();
		ConvertSolution();
	}
}
