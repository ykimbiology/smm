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

#include "Letter.h"
#include "SeqSet.h"
#include "NumVec.h"

// CSeqMatrix assigns a score for a fixed length CSequence by summing over
// a scoring matrix which assigns a value to each possible CLetter at each position in CSequence
// and adding an offset.

class CSeqMatrix 
{
public:
	CSeqMatrix(void) : m_seq_length(0)	{};
	~CSeqMatrix(void)	{};

public:		// Load and Save matrices
	void FromXml(TiXmlNode *node, unsigned seq_length);	
	void ToXml(TiXmlNode *node) const;	
	void Load(const string & name);	
	void Save(const string & name)		const;
	void Load(const string & name, int length, bool no_offset);	// allowing to load matrices in slightly different format
	void Load(istream &in, int length, bool no_offset);			// allowing to load matrices in slightly different format

public:		// Score sequence(s)
	void			Predict(CSeqSet  &set)	const;
	double			Score(const CSequence & seq)	const			{return(Score(seq.begin(),seq.end()));};
	virtual double	Score(CSequence::const_iterator begin, CSequence::const_iterator end)	const;

public:		// Manipulate matrix elements
	double &		Matrix(const CLetter &let, unsigned col)		{return(m_matrix[1+col*CLetter::AlphabetSize()+let.Index()]);};
	const double	Matrix(const CLetter &let, unsigned col) const	{return(m_matrix[1+col*CLetter::AlphabetSize()+let.Index()]);};
	double &		Offset() 										{return(m_matrix[0]);};
	const double	Offset() const									{return(m_matrix[0]);};
	void			resize(unsigned cols)							{m_seq_length=cols; m_matrix.resize(1+cols*CLetter::AlphabetSize()); m_matrix=0;};
	unsigned		NumCols() const									{return(m_seq_length);};			

public:		// Bonus
	void AdjustMatrix(CSeqSet & set);					// Uses a linear transformation of the matrix scores to optimally reproduce values in set. Use this to adjust if matrix is generated from different type of experiment than set
	void AdjustOffset(CSeqSet & set);					// Adjust matrix offset to optimally reproduce values in set. Use this to adjust if matrix is generated from different type of experiment than set
	void AverageMatrix(const Vec<CSeqMatrix> matrices);	// Calculates the average out of several matrices
	void CountSet(const CSeqSet & set);					// Counts the Number of occurences per amino acid and position in set
	void operator+=(const CSeqMatrix &mat)	{m_matrix+=mat.m_matrix;};
protected:
	CNumVec m_matrix;
	unsigned m_seq_length;
};

