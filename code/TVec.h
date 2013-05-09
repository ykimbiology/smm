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

#include <vector>
#include <fstream>
#include <cassert>
#include <iostream>
using namespace std;

#include "BPException.h"

// Encapsulates the std::Vec class for debugging.
// If TEST_VECTOR_ELEMENT_ACCESS is not defined, 
// the Vec<T> class is identical with the STL vector<t> class.

// #define TEST_VECTOR_ELEMENT_ACCESS

template<class T> class Vec : public std::vector<T> 
{
public: 
	Vec()							: std::vector <T>() {};
	explicit Vec(int s)				: std::vector <T>(s){};
	explicit Vec(int s, const T &t)	: std::vector <T>(s,t){};
public:
	// [] accesses Vec elements with checked boundaries
	// exception is caught here to make debugging easier
	// For the relese version this can be commented out.

#ifdef TEST_VECTOR_ELEMENT_ACCESS
	typename Vec<T>::reference	operator [](const unsigned i) 
	{
		try
		{
			return(this->at(i));
		}
		catch(...)
		{
			cerr << "\nIndex out of range, data is corrupted!";
			cerr << "\nIndex: " << i << " Size: " << unsigned(vector <T>::size());
			assert(false);
			throw BPException("Index out of range, data is corrupted");
		}
	};
	typename Vec<T>::const_reference operator [](const unsigned i) const 
	{
		try
		{
			return(this->at(i));
		}
		catch(...)
		{
			cerr << "\nIndex out of range, data is corrupted!";
			cerr << "\nIndex: " << i << " Size: " << unsigned(vector <T>::size());
			assert(false);
			throw BPException("Index out of range, data is corrupted");
		}
	};
#endif
};


typedef Vec<double>		DoubleVec;
typedef Vec<int>		IntVec;
typedef Vec<unsigned>	UIntVec;


// Input / Output of Vector as tabbed elements typically into a text file

template <class T> ostream& operator<< ( ostream& os, const Vec <T> & t )
{

  
  os << unsigned(t.size()) << "\t";
  // E.K. for the following for loop "class" is added before Vec<T> gcc error gone...
  for(class Vec<T>::const_iterator it=t.begin(); it<t.end(); it++)
    os << *it << "\t";
  if(os.fail())
    throw BPException("Unable to save Vec");
  return os;
};

template <class T> istream& operator>> ( istream& is, Vec <T> & t )
{
	t.clear();
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
};

