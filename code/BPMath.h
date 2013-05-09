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

#include "math.h"
#include "TVec.h"
#include <algorithm>
#include <cassert>

#include "NumVec.h"

double CorrelationCoefficient(const CNumVec &x, const CNumVec &y);
double Stdev(const DoubleVec &val);
void	LinearRegression(const CNumVec &x, const CNumVec &y, double &m, double &b);

double AUC		(const DoubleVec & score_pos, const DoubleVec & score_neg);
double ROC		(const DoubleVec & score_pos, const DoubleVec & score_neg, DoubleVec &sens_vec, DoubleVec &spec_vec, DoubleVec &cutoffs);
void   ROCFile	(const DoubleVec & score_pos, const DoubleVec & score_neg, ofstream &out);

