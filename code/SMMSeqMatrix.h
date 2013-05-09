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

#include "SMMRepeater.h"
#include "SeqMatrix.h"

enum	MATRIX_LAMBDA_GROUPING	{ONE_GROUP, POSITION_GROUPS, INDIVIDUAL_LAMBDAS};

class CSMMSeqMatrix :	public CSeqMatrix
{
public:
	CSMMSeqMatrix(void);
	~CSMMSeqMatrix(void);
public:
	void XMLParameters(TiXmlNode *node);
	void SetLambdaGrouping(MATRIX_LAMBDA_GROUPING mlg=POSITION_GROUPS)	{m_mlg=mlg;};
	void TrainMatrix(const CSeqSet &set);
protected:
	CSMMRepeater m_smm;
	void GenerateLambdaGroups();
	MATRIX_LAMBDA_GROUPING m_mlg;
};
