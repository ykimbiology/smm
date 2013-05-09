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
#include "NumVec.h"

// #include <gsl/gsl_min.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_blas.h>

#include "BPException.h"


CNumVec::CNumVec(void) : m_vec(NULL)
{
}

CNumVec::~CNumVec(void)
{
	if(m_vec!=NULL)
		gsl_vector_free(m_vec);
}

CNumVec::CNumVec(const CNumVec &v)
{
	if(v.m_vec!=NULL)
	{
		m_vec=gsl_vector_alloc(v.size());
		gsl_vector_memcpy(m_vec,v.m_vec);
	}
	else
		m_vec=NULL;
}


unsigned CNumVec::size() const
{
	if(m_vec==NULL)
		return(0);
	return(m_vec->size);
}

CNumVec::CNumVec(unsigned n)
{
	if(n==0)
		m_vec=NULL;
	else
		m_vec=gsl_vector_alloc(n);
}

void CNumVec::resize(unsigned rows)
{
	if(m_vec!=NULL)
	{
		if(size()==rows)
			return;
		gsl_vector_free(m_vec);
	}
	if(rows==0)
		m_vec=NULL;
	else
		m_vec=gsl_vector_alloc(rows);
}

void CNumVec::clear()
{
	resize(0);
}

void CNumVec::SetTo(const DoubleVec &v)
{
	resize(v.size());
	for(unsigned i=0; i<v.size(); i++)
		gsl_vector_set(m_vec,i,v[i]);
}



double &CNumVec::operator[](unsigned row)
{
	assert(m_vec!=NULL);
	assert(row<size());
	return(*gsl_vector_ptr(m_vec,row));
}

double CNumVec::operator[](unsigned row) const
{
	assert(m_vec!=NULL);
	assert(row<size());
	return(*gsl_vector_ptr(m_vec,row));
}

void	CNumVec::operator= (double x)
{
	assert(m_vec!=NULL);
	gsl_vector_set_all(m_vec,x);
}

void	CNumVec::operator= (const CNumVec &v)
{
	if(v.m_vec==NULL)
		m_vec=NULL;
	else
	{
		resize(v.size());
		gsl_vector_memcpy(m_vec,v.m_vec);
	}
}

void	CNumVec::operator+=(const CNumVec &v)
{
	assert(v.size()==size());
	gsl_vector_add(m_vec,v.m_vec);	
}

void	CNumVec::operator-=(const CNumVec &v)
{
	assert(v.size()==size());
	gsl_vector_sub(m_vec,v.m_vec);	
}


void	CNumVec::operator*=(double x)
{
	assert(m_vec!=NULL);
	gsl_vector_scale(m_vec,x);
}

void	CNumVec::operator+=(double x)
{
	assert(m_vec!=NULL);
	if(gsl_vector_add_constant(m_vec,x))
		throw BPException("gsl_vector_add_constant");
}

double CNumVec::ScalarProduct(const CNumVec &vec) const
{
	assert(size()==vec.size());

	double result;
	if(gsl_blas_ddot(m_vec,vec.m_vec,&result))
		throw BPException("gsl_blas_ddot");
	return(result);
}


void CNumVec::SetToProduct(const CNumMat &a, const CNumVec &b, bool transpose_a)
{
	unsigned R,N;
	if(!transpose_a)
	{
		R =a.NumRows();
		N=a.NumCols();
	}
	else
	{
		R =a.NumCols();
		N=a.NumRows();
	}
	assert(N==b.size());
	assert(b.m_vec!=m_vec);

	resize(R);
	CBLAS_TRANSPOSE_t ta;
	if(transpose_a)
		ta=CblasTrans;
	else
		ta=CblasNoTrans;
	gsl_blas_dgemv(ta,1.0,a.m_mat,b.m_vec,0.0,m_vec);
}



CNumMat::CNumMat(const CNumMat &mat)
{
	if(mat.m_mat!=NULL)
	{
		m_mat=gsl_matrix_alloc(mat.NumRows(),mat.NumCols());
		gsl_matrix_memcpy(m_mat,mat.m_mat);
	}
	else
		m_mat=NULL;
}


void	CNumMat::operator= (const CNumMat &m)
{
	if(m.m_mat==NULL)
		m_mat=NULL;
	else
	{
		resize(m.NumRows(),m.NumCols());
		gsl_matrix_memcpy(m_mat,m.m_mat);
	}
}

 
CNumMat::CNumMat(unsigned n, unsigned m)
{
	assert(n>0 && m>0);
	m_mat=gsl_matrix_alloc(n,m);
}

CNumMat::CNumMat(void) : m_mat(NULL)
{
}

CNumMat::~CNumMat(void)
{
	if(m_mat!=NULL)
		gsl_matrix_free(m_mat);
}

void CNumMat::resize(unsigned rows, unsigned cols)
{
	assert(rows>0 && cols>0);
	if(m_mat!=NULL)
	{
		if(NumRows()==rows && NumCols()==cols)
			return;
		gsl_matrix_free(m_mat);
	}
	m_mat=gsl_matrix_alloc(rows,cols);
}

void CNumMat::clear()
{
	if(m_mat!=NULL)
		gsl_matrix_free(m_mat);
}


double &CNumMat::operator()(unsigned row, unsigned col)
{
	assert(m_mat!=NULL);
	assert(row<NumRows());
	assert(col<NumCols());
	return(*gsl_matrix_ptr(m_mat,row, col));
}

const double &CNumMat::operator()(unsigned row, unsigned col) const
{
	assert(m_mat!=NULL);
	assert(row<NumRows());
	assert(col<NumCols());
	return(*gsl_matrix_ptr(m_mat,row, col));
}



unsigned CNumMat::NumRows() const
{
	if(m_mat==NULL)
		return(0);
	return(m_mat->size1);
}


unsigned CNumMat::NumCols() const
{
	if(m_mat==NULL)
		return(0);
	return(m_mat->size2);
}

void CNumMat::operator= (double x)
{
	assert(m_mat!=NULL);
	gsl_matrix_set_all(m_mat,x);
}

void CNumMat::operator*=(double x)
{
	assert(m_mat!=NULL);
	gsl_matrix_scale(m_mat,x);
}

void CNumMat::operator+=(double x)
{
	assert(m_mat!=NULL);
	if(gsl_matrix_add_constant(m_mat,x))
		throw BPException("gsl_matrix_add_constant");
}

void CNumMat::operator+=(const CNumMat &m)
{
	assert(NumCols()==m.NumCols());
	assert(NumRows()==m.NumRows());
	if(gsl_matrix_add(m_mat,m.m_mat))
		throw BPException("gsl_matrix_add");
}

void CNumMat::operator-=(const CNumMat &m)
{
	assert(NumCols()==m.NumCols());
	assert(NumRows()==m.NumRows());
	if(gsl_matrix_sub(m_mat,m.m_mat))
		throw BPException("gsl_matrix_sub");
}




void CNumMat::SetToInverse(const CNumMat & mat)
{
	assert(mat.NumRows()==mat.NumCols());
	const unsigned N=mat.NumRows();

	CNumMat LU(mat);
	
	gsl_permutation *p=gsl_permutation_alloc(N);
	int signum;

	if(gsl_linalg_LU_decomp(LU.m_mat, p, &signum))
		throw BPException("gsl_linalg_LU_decomp");

	resize(N,N);

	if(gsl_linalg_LU_invert(LU.m_mat,p,m_mat))
		throw BPException("gsl_linalg_LU_invert");
	
	gsl_permutation_free(p);
}

void CNumMat::SetToProduct(const CNumMat &a, const CNumMat &b, bool  transpose_a, bool transpose_b)
{
	unsigned R,N1,N2,C;
	if(!transpose_a)
	{
		R =a.NumRows();
		N1=a.NumCols();
	}
	else
	{
		R =a.NumCols();
		N1=a.NumRows();
	}
	if(!transpose_b)
	{
		N2=b.NumRows();
		C =b.NumCols();
	}
	else
	{
		N2=b.NumCols();
		C =b.NumRows();
	}
	assert(N1==N2);
	assert(a.m_mat!=m_mat && b.m_mat!=m_mat);

	resize(R,C);
	CBLAS_TRANSPOSE_t ta, tb;
	
	if(transpose_a)
		ta=CblasTrans;
	else
		ta=CblasNoTrans;

	if(transpose_b)
		tb=CblasTrans;
	else
		tb=CblasNoTrans;

	gsl_blas_dgemm(ta,tb,1.0,a.m_mat,b.m_mat,0.0,m_mat);
}



ostream& operator<< ( ostream& os, const CNumVec & t )
{
	os << unsigned(t.size()) << "\t";
	for(unsigned i=0; i<t.size(); i++)
		os << t[i] << "\t";
	if(os.fail())
		throw BPException("Unable to save CNumVec");
	return os;
}

istream& operator>> ( istream& is, CNumVec & t )
{
	unsigned n;
	is >> n;
	if(!is.fail())
	{
		t.resize(n);
		for(unsigned i=0; i<n; i++)
			is >> t[i];
		if(is.fail())
			throw BPException("Incomplete Vec");
	}
	return is;
}


ostream& operator<< ( ostream& os, const CNumMat & t )
{
	os << t.NumRows() << "\t" << t.NumCols() << "\n";
	for(unsigned r=0; r<t.NumRows(); r++)
	{
		for(unsigned  c=0; c<t.NumCols(); c++)
			os << t(r,c) << "\t";
		os << "\n";
	}
	if(os.fail())
		BPException("Could not save Mat");
   return os;
};

