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
#include "SMMRepeater.h"

CSMMRepeater::CSMMRepeater(void)
{
	SetRepetitions();
	SetInitRand();
}

CSMMRepeater::~CSMMRepeater(void)
{
}

void CSMMRepeater::XMLParameters(TiXmlNode *node)
{
	if(node->FirstChild("Repeats"))
		SetRepetitions(ToUInt(node->FirstChild("Repeats")->FirstChild("Bagging")),ToUInt(node->FirstChild("Repeats")->FirstChild("CrossValidation")));
	if(node->FirstChild("LambdaRange"))
		SetLambdaRange(ToDouble(node->FirstChild("LambdaRange")->FirstChild("Min")),ToDouble(node->FirstChild("LambdaRange")->FirstChild("Start")),ToDouble(node->FirstChild("LambdaRange")->FirstChild("Max")));
	if(node->FirstChild("Precission"))
		SetPrecission(ToDouble(node->FirstChild("Precission")));
	if(node->FirstChild("LogDetail"))
	{
		string detail=node->FirstChild("LogDetail")->Value();
		if(detail=="None")
			clog_detail.push_back(NONE); 
		else if(detail=="Minimal")
			clog_detail.push_back(MINIMAL); 
		else if(detail=="Medium")
			clog_detail.push_back(MEDIUM); 
		else if(detail=="Detailed")
			clog_detail.push_back(DETAILED); 
		else if(detail=="All")
			clog_detail.push_back(ALL); 
	}
	if(node->FirstChild("SeedRandomizer"))
		m_srand=ToUInt(node->FirstChild("SeedRandomizer"));
	if(node->FirstChild("MaxNormIterations"))
		SetMaxNormIterations(ToUInt(node->FirstChild("MaxNormIterations")));

}


double CSMMRepeater::Train()
{
	ResetLambda();
	FindStartLambda(true);
	if(OneDimensionalOptimization())
		return(CVDistance(GetLambda()));

	return(OptimizeLambda());
}


void	CSMMRepeater::TrainRepeater(bool solution_possible)
{
	srand(m_srand);

	m_solutions.clear();
	m_lambdas.clear();
	m_dists.clear();

	if(!solution_possible)
		return;

	if(clog_detail.back()>NONE)
	{
		clog << endl << "LogDetail\t";
		switch(clog_detail.back())
		{
		case NONE		: clog << "NONE";	break;
		case MINIMAL	: clog << "MINIMAL";break;
		case MEDIUM		: clog << "MEDIUM"; break;
		case DETAILED	: clog << "DETAILED";break;
		case ALL		: clog << "ALL";	break;
		}
		clog << endl << "RepeatsBagging\t"			<< m_train_repeats;
		clog << endl << "RepeatsCrossValidation\t"	<< m_cv_num;
		clog << endl << "LambdaRangeMin\t"			<< m_lambda_min;
		clog << endl << "LambdaRangeMax\t"			<< m_lambda_max;
		clog << endl << "LambdaRangeStart\t"		<< m_lambda_start;
		clog << endl << "Precission\t"				<< m_precission;
		clog << endl << "SeedRandomizer\t"			<< m_srand;
		clog << endl << "MaxNormIterations\t"		<< m_max_norm_iterations;
	}

	m_solutions.clear();
	for(unsigned r=0; r<m_train_repeats; r++)
	{
	  //cout << endl << "Repeat :" << r << endl;
		if(clog_detail.back()>=DETAILED)
			clog << endl << endl << "Repeat:\t" << r;
		CreateCrossValidator(m_cv_num);
		double dist=Train();

		m_dists.push_back(dist);
		m_lambdas.push_back(GetLambda());
		m_solutions.push_back(GetMeanCVX());
	}

	// Final solution is stored at back of m_solutions
	CNumVec mean;
	mean.resize(m_solutions.front().size());
	mean=0;
	for(unsigned c=0; c<m_solutions.size(); c++)
		mean+=m_solutions[c];
	mean*=1.0/m_solutions.size();
	m_solutions.push_back(mean);

	if(clog_detail.back()>NONE)
	{
		if(clog_detail.back()>MINIMAL)
		{
			clog << endl <<endl << "Individual X:";
			for(unsigned r=0; r<m_solutions.size()-1; r++)
				clog << endl << "Rep.: " << r << "\t\t\t" << m_solutions[r];
			clog << endl << endl << "Individual Lambdas:";
			for(unsigned r=0; r<m_lambdas.size(); r++)
				clog << endl << "Rep.: " << r << "\tDist:\t" << m_dists[r] << "\t" << m_lambdas[r];
			clog << endl;
		}
		clog << endl << "Mean X:\t\t\t" << GetMeanSolution();
	}
}
