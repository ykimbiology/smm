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
#include "SMMSet.h"



CSMMSet::CSMMSet(void)
{
}

CSMMSet::~CSMMSet(void)
{
}

void CSMMSet::CreateMaskedSet(const CSMMSet &source, const UIntVec &mask)
{
	CDataset::operator=(source);
	const unsigned M=source.ElementNumber();
	const unsigned N=mask.size();
	m_A.resize(M,N);
	for(unsigned m=0; m<M; m++)
		for(unsigned n=0; n<N; n++)
			m_A(m,n)=source.m_A(m,mask[n]);

}


void CSMMSet::CreateSubset(const CSMMSet &source, const UIntVec &members)
{
	CDataset::CreateSubset(source,members);
	
	const unsigned M=members.size();
	const unsigned N=source.MatrixCols();
	m_A.resize(M,N);
	for(unsigned m=0; m<M; m++)
		for(unsigned n=0; n<N; n++)
			m_A(m,n)=source.m_A(members[m],n);
}


void CSMMSet::AddSet(const CSMMSet &source)
{
	assert(m_A.NumCols()==source.m_A.NumCols());
	CDataset::AddSet(source);	

	const unsigned N=m_A.NumCols();
	const unsigned M1=m_A.NumRows();
	const unsigned M2=source.m_A.NumRows();

	const CNumMat A=m_A;
	
	m_A.resize(M1+M2,source.m_A.NumCols());

	for(unsigned m=0; m<M1; m++)
		for(unsigned n=0; n<N; n++)
			m_A(m,n)=A(m,n);

	for(unsigned m=0; m<M2; m++)
		for(unsigned n=0; n<N; n++)
			m_A(m+M1,n)=source.m_A(m,n);
}

void	CSMMSet::Predict(const CNumVec &x)
{
	m_predictions.SetToProduct(m_A,x);
}


double CSMMSet::XDistance(const CNumVec &x)
{
	assert(x.size()==MatrixCols());
	m_predictions.SetToProduct(m_A,x);

	m_difference=GetPredictions();
	m_difference-=GetMeasurements();
	if(InequalitiesPresent())
	{
		for(unsigned m=0; m<m_difference.size(); m++)
		{
			if(m_inequalities[m]!=EQUAL)
			{
				if(m_inequalities[m]==GREATER)
				{
					if(m_difference[m]>0)
						m_difference[m]=0;
				}
				else if(m_difference[m]<0)
					m_difference[m]=0;
			}
		}
	}
	return(m_difference.ScalarProduct(m_difference));
}

double	CSMMSet::XGradient(const CNumVec &x,CNumVec &ddistdx)
{
	double dist=XDistance(x);
	ddistdx.SetToProduct(m_A,m_difference,true);
	return(dist);
}


// Only member of CV SMMSet:
void CVSMMSet::CreateMaskedCopy(const CVSMMSet &source, const UIntVec &mask)
{
	unsigned subnum=source.SubsetNum();
	m_train.resize(subnum);
	m_blind.resize(subnum);
	for(unsigned s=0; s<subnum; s++)
	{
		m_train[s].CreateMaskedSet(source.m_train[s],mask);
		m_blind[s].CreateMaskedSet(source.m_blind[s],mask);
	}
}

