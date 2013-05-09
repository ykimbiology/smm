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
#include "Dataset.h"

#include "BPException.h"
#include "BPMath.h"

void CDataset::Load(const string & name)
{
	m_file_name=name; 

	m_inequality_present=false;
	for(unsigned i=0; i<m_inequalities.size(); i++)
		if(m_inequalities[i]!=EQUAL)
		{
			m_inequality_present=true;
			break;
		}
}


void CDataset::CreateSubset(const CDataset &source, const UIntVec &members)
{
	assert(source.m_predictions.size()==source.m_measurements.size());

	const unsigned M=members.size();
	m_measurements.resize(M);
	m_inequalities.resize(M);
	m_predictions.resize(M);

	for(unsigned m=0; m<M; m++)
	{
		m_measurements[m]	=source.m_measurements	[members[m]];
		m_inequalities[m]	=source.m_inequalities	[members[m]];
		m_predictions[m]	=source.m_predictions	[members[m]];
	}

	m_inequality_present=false;
	for(unsigned i=0; i<m_inequalities.size(); i++)
		if(m_inequalities[i]!=EQUAL)
		{
			m_inequality_present=true;
			break;
		}
};

void CDataset::AddSet(const CDataset &source)
{
	const unsigned M1=m_measurements.size();
	const unsigned M2=source.m_measurements.size();
	assert(m_predictions.size()==M1);
	assert(source.m_predictions.size()==M2);

	const CNumVec meas=m_measurements;
	const CNumVec pred=m_predictions;
	const Vec<INEQUALITY> ineq=m_inequalities;

	m_measurements.resize(M1+M2);
	m_inequalities.resize(M1+M2);
	m_predictions.resize(M1+M2);
		
	for(unsigned m=0; m<M1; m++)
	{
		m_measurements[m]	=meas[m];
		m_inequalities[m]	=ineq[m];
		m_predictions[m]	=pred[m];
	}

	for(unsigned m=0; m<M2; m++)
	{
		m_measurements[m+M1]=source.m_measurements[m];
		m_inequalities[m+M1]=source.m_inequalities[m];
		m_predictions[m+M1]	=source.m_predictions[m];
	}

	if(source.m_inequality_present)
		m_inequality_present=true;
};



double CDataset::EvaluatePrediction(EVAL_METHOD method, double classification_cutoff, bool classify_positives_below_cutoff) const
{
	assert(m_predictions.size()== m_measurements.size() && m_measurements.size()>2);
	double dist=0;
	if(method==DEFAULT)
	{
		method=m_eval_method;
		classification_cutoff =m_classification_cutoff;
		classify_positives_below_cutoff=m_classification_positive_below_cutoff;
	}
	switch(method)
	{
	case L1_DISTANCE : 
		for(unsigned i=0; i<m_predictions.size(); i++)
		{
			if(m_inequalities[i]!=EQUAL)
			{
				if(m_inequalities[i]==GREATER)
				{
					if(m_predictions[i]>m_measurements[i])
						continue;
				}
				else if(m_predictions[i]<m_measurements[i])
						continue;
			}
			double d=m_predictions[i]-m_measurements[i];
			dist+=fabs(d);
		}
		return(dist/m_predictions.size());

	case L2_DISTANCE : 
		for(unsigned i=0; i<m_predictions.size(); i++)
		{
			if(m_inequalities[i]!=EQUAL)
			{
				if(m_inequalities[i]==GREATER)
				{
					if(m_predictions[i]>m_measurements[i])
						continue;
				}
				else if(m_predictions[i]<m_measurements[i])
						continue;
			}
			double d=m_predictions[i]-m_measurements[i];
			dist+=(d*d);
		}
		return(dist/m_predictions.size());
	case CORRELATION	: 
		return(CorrelationCoefficient(m_measurements,m_predictions));
	case AUC			: 
		DoubleVec tp, fp, cutoff;
		return(PredROC(classification_cutoff, classify_positives_below_cutoff,tp,fp,cutoff));
	}
	throw(BPException("Invalid value for evaluation method"));
};


const char * CDataset::EvaluationName() const
{
	switch(m_eval_method)
	{
	case L2_DISTANCE	: return("L2Distance");
	case CORRELATION	: return("correl");
	case AUC			: return("AUC");
	}
	throw(BPException("Invalid value for m_eval_method"));
};


double CDataset::PredROC(double classification_cutoff, bool classify_positives_below_cutoff,DoubleVec &tp,DoubleVec &fp,DoubleVec &cutoff) const
{
	DoubleVec pos_scores, neg_scores;
	for(unsigned i=0; i<m_predictions.size(); i++)
	{
		if(m_measurements[i]<classification_cutoff && classify_positives_below_cutoff)
			pos_scores.push_back(m_predictions[i]);
		else
			neg_scores.push_back(m_predictions[i]);

	}
	return(ROC(pos_scores, neg_scores,tp,fp,cutoff));
}
