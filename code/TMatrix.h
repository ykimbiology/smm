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

// Mat implements a rather basic matrix class 
// of which only the operator() functions add value
// based on the Vec class some operators

#include "TVec.h"

template<class T> class Mat: public Vec<T>
{
public:
	// Construction: 
	Mat()													: Vec<T>(),			m_cols(0),	  m_rows(0)			{};
	Mat(unsigned int rows, unsigned int cols)				: Vec<T>(rows*cols),	m_cols(cols), m_rows(rows)		{};
	Mat(unsigned int rows, unsigned int cols, const T& val)	: Vec<T>(rows*cols,val),m_cols(cols), m_rows(rows)		{};
public:
	void resize(unsigned int rows, unsigned int cols);
	void resize(unsigned int rows, unsigned int cols, const T & value);
public: 
	unsigned int NumCols()const	{return(m_cols);};
	unsigned int NumRows()const	{return(m_rows);};
public:
	// Access to Elements: 

	typename Vec<T>::reference	operator ()(const unsigned int row, const unsigned int col) 
	{
		assert(row<m_rows);
		assert(col<m_cols);
		assert(row>=0);
		assert(col>=0);
		return(Vec<T>::operator [](row*m_cols+col));
	};
	typename Vec<T>::const_reference operator ()(const unsigned int row, const unsigned int col) const 
	{
		assert(row<m_rows);
		assert(col<m_cols);
		assert(row>=0);
		assert(col>=0);
		return(Vec<T>::operator [](row*m_cols+col));
	};
protected:
	unsigned m_rows;
	unsigned m_cols;
};


// Typedefs: 

typedef Mat<int>		IntMat;
typedef Mat<double>		DoubleMat;

// Function Definitions: 

// Input / Output of Mat as tabbed elements typically into a text file

template <class T> ostream& operator<< ( ostream& os, const Mat <T> & t )
{
	os << t.NumRows() << "\t" << t.NumCols() << "\n";
	for(unsigned r=0; r<t.NumRows(); r++)
	{
		for(unsigned  c=0; c<t.NumCols(); c++)
			os << t(r,c) << "\t";
		os << "\n";
	}
	if(os.fail())
		BPException("Could not load Mat");
   return os;
};

template <class T> istream& operator>> ( istream& is, Mat <T> & t )
{
	int rows, cols;
	is >>rows >> cols;
	assert(!is.fail());
	t.resize(rows,cols);
	for(unsigned  r=0; r<t.NumRows(); r++)
		for(unsigned  c=0; c<t.NumCols(); c++)
			is >> t(r,c);
   return is;
};
// Mat member functions


template<class T> void Mat<T>::resize(unsigned int rows, unsigned int cols)
{
	assert(rows>0);
	assert(cols>0);
	m_cols=cols;
	m_rows=rows;
	Vec<T>::resize(cols*rows);
};

template<class T> void Mat<T>::resize(unsigned int rows, unsigned int cols, const T &value)
{
	assert(rows>0);
	assert(cols>0);
	m_cols=cols;
	m_rows=rows;
	Vec<T>::resize(cols*rows,value);
};
