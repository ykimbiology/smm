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

#include "Dataset.h"
#include "Letter.h"
#include "VSet.h"
#include "SMMSet.h"

// CSeqSet holds CSequence elements of a fixed length


class CSeqSet :	public CDataset
{
public:
	CSeqSet(void);
	~CSeqSet(void);
public:
	void FromXml(const TiXmlNode *node, unsigned length);
	void ToXml(TiXmlNode *node);

	virtual void Load(const string &name);
	virtual void Save(const string &name, bool predicted=false) const;

public:		// Access Sequences
	const Vec<CSequence> &GetSequences()	const {return(m_sequences);};
	unsigned SeqLength()					const {return(m_seq_length);};

public:		// Join and Split Sets
	void CreateSubset(const CSeqSet &source, UIntVec &members);
	void AddSet(const CSeqSet &source);
public:
	void ConvertToSMMSet(CSMMSet &set, bool matrix, const Vec<PairCoef> &pairs=Vec<PairCoef>()) const;
public:		// Bonus
	unsigned DiscardOutliers(const CSeqSet &source_set, double stdev_threshold);
protected:
	unsigned m_seq_length;
	Vec<CSequence> m_sequences;
};

typedef CVSet<CSeqSet>	CVSeqSet; 
