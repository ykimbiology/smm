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

#include <cassert>
#include <string>
#include <iostream>

#include "TVec.h"
#include "BPException.h"

// This class maps char elements (letters) to their index in an alphabet. 
// It is meant to translate protein or DNA sequences into a more machine appropriate format. 


class CLetter
{
public:
	CLetter();					// Should only be used to construct an empty CLetter that will be filled with >> 
	CLetter(char letter);
	CLetter(unsigned index);
	~CLetter(void);
public:
	unsigned Index()	const	{assert(m_index>=0 && m_index<m_alphabet.size()); return(m_index);};
	char	Letter()	const	{return(m_letter_map[m_index]);};
public:
	bool operator==(const CLetter &let) const	{return(m_index==let.Index());};
protected:
	unsigned m_index;
public:						
	static void Init(const string &alphabet);		// There should only be one initial call to init. 
	static const Vec<CLetter>&	Alphabet()	{return(m_alphabet);};  
	static unsigned	AlphabetSize()			{return(m_alphabet_size);};  
protected:
	static Vec<CLetter>			m_alphabet;
	static unsigned				m_alphabet_size;
	static UIntVec				m_index_map;		// maps letters to indices
	static Vec<char>			m_letter_map;		// maps indices to letters
};

typedef struct
{
	double val;
	CLetter let1;
	unsigned pos1;
	CLetter let2;
	unsigned pos2;
} PairCoef;




typedef Vec<CLetter>		CSequence;

ostream& operator<< ( ostream& os, const CLetter & let );
istream& operator>> ( istream& is, CLetter &let);

ostream& operator<< ( ostream& os, const CSequence & seq);
istream& operator>> ( istream& is, CSequence &seq);

class BPUnknownLetter : public BPException 
{
public:
	BPUnknownLetter(string message, char letter) : BPException(message), m_letter(letter){};
	BPUnknownLetter(string message) : BPException(message), m_letter(0)	{};
	char m_letter;
};
