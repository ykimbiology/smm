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

#include "SeqMatrix.h"

// This extends the standard scoring matrix defined in SeqMatrix.h
// to also include pair interactions between two positions captured 
// in a list of "PairCoef"s. PairCoef is defined in Letter.h


class CSeqPair :	public CSeqMatrix
{
public:
	CSeqPair(void);
	~CSeqPair(void);

public:		
	void Load	(const string & name);	
	void Save	(const string & name)		const;
	void FromXml(TiXmlNode *node, unsigned seq_length);	
	void ToXml	(TiXmlNode *node)			const;	
public:		// add PairCoef score to Matrix Score
	virtual double	Score(CSequence::const_iterator begin, CSequence::const_iterator end)	const;

public:		// Access to PairCoef values
		const		Vec<PairCoef> &GetPairs()	const	{return(m_pair_coef);};

public:		// Used to determine which PairCoefs to accept 
	unsigned	IdentifyPairCoef(const CSeqSet &set, unsigned min_count);	// Identifies all PairCoef that have at least min_count occurences in set
	void		DropPairCoef(const CSeqSet &set, double percent);			// Set predictions have to be made by matrix. This drops PairCoefs that have more than percent(<0.5) occurences above AND below the matrix score
protected:
	Vec<PairCoef> m_pair_coef;
};
