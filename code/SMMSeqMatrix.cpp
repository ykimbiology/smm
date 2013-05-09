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
#include "SMMSeqMatrix.h"


CSMMSeqMatrix::CSMMSeqMatrix(void)
{
	SetLambdaGrouping();
}

CSMMSeqMatrix::~CSMMSeqMatrix(void)
{
}


void CSMMSeqMatrix::XMLParameters(TiXmlNode *node)
{
	if(node->FirstChild("LambdaGrouping"))
	{
		string lg=node->FirstChild("LambdaGrouping")->FirstChild()->Value();
		if(lg=="One")
			SetLambdaGrouping(ONE_GROUP);
		else if(lg=="Position")
			SetLambdaGrouping(POSITION_GROUPS);
		else if (lg=="Individual")
			SetLambdaGrouping(INDIVIDUAL_LAMBDAS);
		else
			throw BPException("Invalid LambdaGrouping");
	}
	if(node->FirstChild("SMMParameters"))
		m_smm.XMLParameters(node->FirstChild("SMMParameters"));
}


void CSMMSeqMatrix::GenerateLambdaGroups()
{
	Vec<UIntVec>	lambda_groups;
	switch(m_mlg)
	{
	case POSITION_GROUPS:
		for(unsigned c=0; c<m_seq_length; c++)
		{
			UIntVec group;
			for(unsigned i=0; i<CLetter::AlphabetSize(); i++)
				group.push_back(1+i+c*CLetter::AlphabetSize());
			lambda_groups.push_back(group);
		}
		break;
	case ONE_GROUP:
		{
		UIntVec group;
		for(unsigned c=0; c<m_seq_length*CLetter::AlphabetSize(); c++)
			group.push_back(c+1);
		lambda_groups.push_back(group);
		}
		break;
	case INDIVIDUAL_LAMBDAS:
		for(unsigned c=0; c<m_seq_length*CLetter::AlphabetSize(); c++)
		{
			UIntVec group;
			group.push_back(c+1);
            lambda_groups.push_back(group);
		}
		break;
	default:	throw BPException("Invalid lambda grouping");
	}
	m_smm.SetLambdaGroups(lambda_groups);
}



void CSMMSeqMatrix::TrainMatrix(const CSeqSet &seqset)
{
	if(clog_detail.back()>NONE)
	{
		clog << endl << "CPredSMM::TrainMatrix(" << seqset.GetName()<<")";
		clog << endl << "Elements in Set: " << seqset.ElementNumber();
		clog << endl << "Lambda Grouping\t";
		if(m_mlg==ONE_GROUP)			clog << "ONE_GROUP";
		else if(m_mlg==POSITION_GROUPS)	clog << "POSITION_GROUPS";
		else							clog << "INDIVIDUAL_LAMBDAS";
	}
	resize(seqset.SeqLength());
	
	GenerateLambdaGroups();

	seqset.ConvertToSMMSet(m_smm.TrainingSet(),true);

	m_smm.TrainRepeater();

	m_matrix=m_smm.GetMeanSolution();
}
