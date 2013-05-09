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

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>

#include "TMatrix.h"

class CNumMat;


class CNumVec 
{
public:
	CNumVec(void);
	~CNumVec(void);
	CNumVec(const CNumVec &v);
	explicit CNumVec(unsigned rows);
public:
	double	&operator[](unsigned row);
	double	operator[] (unsigned row) const;
	unsigned size()	const; 
	void	resize(unsigned rows);
	void clear();
public:
	void	operator= (const CNumVec &v);
	void	operator= (double x);
	void	operator+=(const CNumVec &v);
	void	operator-=(const CNumVec &v);
	void	operator+=(double x);
	void	operator*=(double x);
public:
	double ScalarProduct(const CNumVec &vec) const;
	void SetToProduct(const CNumMat &a, const CNumVec &b, bool transpose_a=false);
public:
	gsl_vector *m_vec;
public:
	void SetTo(const DoubleVec &v);
};

class CNumMat
{
	friend class CNumVec;
public:
	CNumMat(void);
	~CNumMat(void);
	CNumMat(const CNumMat &m);
	explicit CNumMat(unsigned rows, unsigned cols);
public:
	double &operator()(unsigned row, unsigned col);
	const double &operator()(unsigned row, unsigned col) const;
	unsigned NumRows() const;
	unsigned NumCols() const;
	void resize(unsigned rows, unsigned cols);
	void clear();
public:
	void	operator= (const CNumMat &m);
	void	operator= (double x);
	void	operator*=(double x);
	void	operator+=(double x);
	void	operator+=(const CNumMat &m);
	void	operator-=(const CNumMat &m);
public:
	void SetToInverse(const CNumMat &mat);
	void SetToProduct(const CNumMat &a, const CNumMat &b, bool transpose_a=true, bool transpose_b=false);
public:
	gsl_matrix *m_mat;
};


ostream& operator<< (ostream& os, const CNumMat & t);
ostream& operator<< (ostream& os, const CNumVec & t);
istream& operator>> (istream& is, CNumVec & t);

