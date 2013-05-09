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
#include "VSet.h"

class CSMMSet : public CDataset
{
	friend class CSeqSet;
public:
	CSMMSet(void);
	~CSMMSet(void);
public:		// Join and Split datasets
	void CreateSubset	(const CSMMSet &source, const UIntVec &members);
	void CreateMaskedSet(const CSMMSet &source, const UIntVec &mask);	// Masked copies have a subset of the matrix columns defined in mask
	void AddSet(const CSMMSet &source);

public:		// Access Elements
	unsigned MatrixCols()		const {return(m_A.NumCols());};
	const CNumMat &	GetMatrix() const {return(m_A);};

public:		// SMM algorithm specific: Assumes that m_A * x = m_measured
	void	Predict(const CNumVec &x);
	double	XDistance(const CNumVec &x);
	double	XGradient(const CNumVec &x, CNumVec &ddistdx);
protected:
	CNumMat m_A;
private:
	CNumVec m_difference;
};

class CVSMMSet : public CVSet<CSMMSet>
{
public:
	void CreateMaskedCopy(const CVSMMSet &source, const UIntVec &mask);
};
