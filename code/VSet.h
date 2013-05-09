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

#include "TVec.h"
#include "BPException.h"

// Allows to split a set in Subsets for CrossValidation
// Assumes that <Set> has AddSet, CreateSubset and other functions supplied in CDataset

template <class Set> class CVSet : public Set
{
public:
	CVSet(void)		{};
	~CVSet(void)	{};

public:		// Access Subsets
	const Set & SubsetTrain(unsigned n)	const {return(m_train[n]);};
	const Set & SubsetBlind(unsigned n)	const {return(m_blind[n]);};
	Set	& SubsetTrain(unsigned n)	{return(m_train[n]);};
	Set & SubsetBlind(unsigned n)	{return(m_blind[n]);};
	unsigned SubsetNum()	const	{return(m_train.size());};

public:		// Load, Save and Generate Subsets 
	void	GenerateSubsets	(unsigned subnum);
	void	LoadSubsets		(unsigned subnum);
	void	SaveSubsets		();
	void	PoolBlindSets(Set &pool);
	string  SubDir()	const	{return(string("_"+Set::m_set_name));};
protected:
	Vec<Set>	m_train;
	Vec<Set>	m_blind;
	
};


template <class Set> void CVSet<Set>::PoolBlindSets(Set &pool)
{
	pool=m_blind.front();
	for(unsigned s=1; s<m_blind.size(); s++)
		pool.AddSet(m_blind[s]);
}

template <class Set> void CVSet<Set>::GenerateSubsets(unsigned subnum)
{
	if(subnum>Set::ElementNumber())
		throw BPException("Number of Set Elements < Number of Cross Validations");
	m_train.resize(subnum);
	m_blind.resize(subnum);

	unsigned count=0;
	UIntVec ord;
	for(unsigned i=0; i<Set::ElementNumber(); i++)
		ord.push_back(i);
	random_shuffle(ord.begin(),ord.end());
	UIntVec order(Set::ElementNumber());

	//  More efficient, but not backwards identical:
	//	order=ord;
	//	order*=subset_num;
	//	order/=ElementCount();


	for(unsigned section=0; section<subnum; section++)
	{
		double part=double(ord.size())/double(subnum);
		unsigned start=unsigned (part*section);
		unsigned end=unsigned (part*(section+1));
		
		for(unsigned i=0; i<ord.size(); i++)
		{
			if(i>=start && i<end)
				order[ord[i]]=section;
		}
	}

	for(unsigned c=0; c<subnum; c++)
	{
		UIntVec blind_members, train_members;
		for(unsigned i=0; i<order.size(); i++)
		{
			if(order[i]==c)
				blind_members.push_back(i);
			else
				train_members.push_back(i);
		}
		m_blind[c].CreateSubset(*this,blind_members);
		m_train[c].CreateSubset(*this,train_members);
	}
}

template <class Set> void CVSet<Set>::LoadSubsets(unsigned subnum)
{
	m_blind.resize(subnum);
	m_train.resize(subnum);
	
	ex_chdir(SubDir());

	unsigned pos=Set::m_file_name.find('.');
	if(pos==string::npos)
		pos=Set::m_file_name.size();

	for(unsigned sec=0; sec<subnum; sec++)
	{
		ostringstream bname, tname;

		bname << Set::m_file_name.substr(0,pos) << "_b" << sec+1 <<"-" << subnum << Set::m_file_name.substr(pos);
		tname << Set::m_file_name.substr(0,pos) << "_t" << sec+1 <<"-" << subnum << Set::m_file_name.substr(pos);
		Set t,b;
		m_train[sec].Load(tname.str());
		m_blind[sec].Load(bname.str());
		assert(m_train[sec].ElementNumber()+m_blind[sec].ElementNumber()==Set::ElementNumber());
	}
	ex_chdir("..");
}

template <class Set> void CVSet<Set>::SaveSubsets() 
{
	ex_mkdir(SubDir());
	ex_chdir(SubDir());

	unsigned pos=Set::m_file_name.find('.');
	if(pos==string::npos)
		pos=Set::m_file_name.size();

	for(unsigned c=0; c<SubsetNum(); c++)
	{
		ostringstream bname, tname;

		bname << Set::m_file_name.substr(0,pos) << "_b" << c+1 <<"-" << SubsetNum() << Set::m_file_name.substr(pos);
		tname << Set::m_file_name.substr(0,pos) << "_t" << c+1 <<"-" << SubsetNum() << Set::m_file_name.substr(pos);
		m_blind[c].SetName(bname.str());
		m_blind[c].Save	  (bname.str(),false);
		m_train[c].SetName(tname.str());
		m_train[c].Save	  (tname.str(),false);
	}
	ex_chdir("..");
}
