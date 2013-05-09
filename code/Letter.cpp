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
#include "Letter.h"

#include <sstream>
#include <climits> //https://bugs.launchpad.net/ubuntu/+bug/290907

using namespace std;

Vec<CLetter>		CLetter::m_alphabet;
Vec<char>		CLetter::m_letter_map;
Vec<unsigned>	CLetter::m_index_map;
unsigned			CLetter::m_alphabet_size=0;

CLetter::CLetter() : m_index(UINT_MAX)
{
}

CLetter::CLetter(unsigned index) : m_index(index) 
{
	if(m_index<0 || m_index>m_alphabet.size())
	{
		BPUnknownLetter l("Index ");
		l.m_message << index << " out of range of alphabet";
		throw(BPUnknownLetter(l));
	}
}

CLetter::CLetter(char letter) : m_index(m_index_map[letter])
{
	if(m_index==UINT_MAX)
	{
		BPUnknownLetter l("Letter ",letter);
		l.m_message << letter << " not contained in alphabet";
		throw(BPUnknownLetter(l));
	}
}

CLetter::~CLetter(void)
{
}

void CLetter::Init(const string &alphabet)
{
	if(alphabet.size()==0)
		throw(BPException("Empty alphabet"));

	m_letter_map.clear();

	m_index_map.clear();
	m_index_map.insert(m_index_map.begin(),UCHAR_MAX,UINT_MAX);
	
	for(unsigned pos=0; pos<alphabet.size(); pos++)
	{
		if(isspace(alphabet[pos]))
			throw(BPException("Whitespace character in alphabet"));
		m_index_map[alphabet[pos]]=pos;
		m_letter_map.push_back(alphabet[pos]);
		m_alphabet.push_back(CLetter(alphabet[pos]));
	}
	m_alphabet_size=m_alphabet.size();
}

ostream& operator<< ( ostream& os, const CLetter & let )
{
	os << let.Letter();
	return(os);
}

istream& operator>> ( istream& is, CLetter &let)
{
	char c=0;
	is >> c;
	if(!is.fail())
		let=CLetter(c);
	return(is);
}

ostream& operator<<  (ostream& os, const CSequence & seq)
{
	for(CSequence::const_iterator s=seq.begin(); s<seq.end(); s++)
		os << s->Letter();
	return(os);
}

istream& operator>> (istream& is, CSequence &seq)
{
	string  str;
	is >> str;
	for(unsigned int s=0; s<str.size(); s++)
	{
		try
		{
			CLetter let(str[s]);
			seq.push_back(let);
		}
		catch(BPUnknownLetter &l)
		{
			l.m_message << endl << "In sequence: "<< str << endl << "At position: "<< s +1 << endl;
			throw(BPUnknownLetter(l));
		}
	}
	return(is);
}
