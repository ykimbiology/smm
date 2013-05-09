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

// CDataset is a base class for any set of elements that can be associated with
// a measured and a predicted value, including measurements that only give 
// a "inequality" information (<100, >10, ...).
// The main functionality is to evaluate prediction quality

enum	INEQUALITY	{EQUAL, SMALLER, GREATER};


class CDataset
{
public:
	CDataset(void)	: m_eval_method(L2_DISTANCE) {};
	~CDataset(void)	{};

public:		// Load and Save. The filename used in Load is stored in m_file_name, and can be changed by SetName
	virtual void	Load(const string & name);
	virtual void	Save(const string & name) const	{};
	void			SetName(const string &name)			{m_file_name=name;};
	const string	& GetName()	const					{return(m_file_name);};

public:		// Access Elements
	unsigned				ElementNumber() const		{return(m_measurements.size());};
	const CNumVec			& GetMeasurements()	const	{return(m_measurements);};
	const CNumVec			& GetPredictions() const	{return(m_predictions);};
	const Vec<INEQUALITY>	& GetInequalities() const	{return(m_inequalities);};
	bool					InequalitiesPresent() const {return(m_inequality_present);};

public:		// Predict and Evaluate Prediction
	void		Predict	(const CNumVec &pred)			{m_predictions=pred;};
	// Evaluate Predictions
	enum		EVAL_METHOD {DEFAULT, L2_DISTANCE, AUC,CORRELATION, L1_DISTANCE};
	void		SetEvaluationMethod (EVAL_METHOD method, double classification_cutoff=0, bool classify_positives_below_cutoff=true)	{m_eval_method=method; m_classification_cutoff=classification_cutoff; m_classification_positive_below_cutoff=classify_positives_below_cutoff;};
	const char *EvaluationName		()					const;	// Returns name of distance type specified with SetEvaluationMethod
	double		EvaluatePrediction	(EVAL_METHOD method=DEFAULT, double classification_cutoff=0, bool classify_positives_below_cutoff=true)const;	// Returns distance type specified in m_eval_method
	double		PredROC				(double classification_cutoff, bool classify_positives_below_cutoff,DoubleVec &tp,DoubleVec &fp,DoubleVec &cutoff) const;

public:		// Join or split Datasets
	void CreateSubset(const CDataset &source, const UIntVec &members);
	void AddSet(const CDataset &source);
public:
	void	SubtractPrediction()	{m_measurements-=m_predictions;};
protected:
	string			m_file_name;
protected:
	CNumVec			m_measurements;
	CNumVec			m_predictions;
	Vec<INEQUALITY>	m_inequalities;
	bool			m_inequality_present;
protected:
	EVAL_METHOD	m_eval_method;
	double		m_classification_cutoff;
	bool		m_classification_positive_below_cutoff;
};

