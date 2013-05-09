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
#include "SeqSet.h"
#include "math.h"

CSeqSet::CSeqSet(void) 
{
}


void CSeqSet::ToXml(TiXmlNode *node)
{
	TiXmlText a("");

	TiXmlElement meas("Measured");
	TiXmlElement seq("Sequence");
	TiXmlElement thres("Threshold");
	
	TiXmlText *tm=meas.InsertEndChild(a)->ToText();
	TiXmlText *ts=seq.InsertEndChild(a)->ToText();
	
	TiXmlElement *tt=thres.InsertEndChild(thres)->ToElement();

	TiXmlElement set("SequenceData");
	for(unsigned i=0; i<m_measurements.size(); i++)
	{
		stringstream s,m;
		s << m_sequences[i];
		m << m_measurements[i];

		ts->SetValue(s.str());
		tm->SetValue(m.str());


		TiXmlElement data("DataPoint");
		data.InsertEndChild(seq);
		if(m_inequalities[i]!=EQUAL)
		{
			if(m_inequalities[i]==GREATER)
				tt->SetValue("Greater");
			else
				tt->SetValue("Lesser");
			data.InsertEndChild(thres);
		}
		data.InsertEndChild(meas);
		set.InsertEndChild(data);
	}
	node->InsertEndChild(set);
}


void CSeqSet::FromXml(const TiXmlNode *node, unsigned length)
{
	node=node->FirstChild("SequenceData");
	m_seq_length=length;
	DoubleVec measurements;
	TiXmlNode *datapoint=node->FirstChild("DataPoint");
	try
	{
		while(datapoint)
		{
			measurements.push_back(ToDouble(datapoint->FirstChild("Measured")));
			if(datapoint->FirstChild("Threshold"))
			{
				if(datapoint->FirstChild("Threshold")->FirstChild("Greater"))
					m_inequalities.push_back(GREATER);
				else
					m_inequalities.push_back(SMALLER);
			}
			else
				m_inequalities.push_back(EQUAL);
			const string &s=ToText(datapoint->FirstChild("Sequence"));
			if(s.size()!=length)
				throw BPException("Invalid sequence length");
			
			CSequence seq(length);
			for(unsigned l=0; l<length; l++)
				seq[l]=CLetter(s[l]);
			m_sequences.push_back(seq);

			datapoint=datapoint->NextSibling();
		}
	}
	catch(BPException b)
	{
		b.m_message << " (datapoint " << measurements.size() << ") " ;
		throw b;
	}
	m_measurements.SetTo(measurements);	
	m_predictions.resize(measurements.size());
	CDataset::Load("xml set");
}



CSeqSet::~CSeqSet(void)
{
}


void CSeqSet::ConvertToSMMSet(CSMMSet &set, bool matrix, const Vec<PairCoef> &pairs) const
{
	set.CDataset::operator=(*this);
	unsigned N1;
	if(matrix)
		N1=SeqLength()*CLetter::AlphabetSize()+pairs.size();
	else
	{
		// Only pair coefficients will be determined. 
		N1=0;
		// Subtract predictions from measurements
		set.m_measurements-=set.m_predictions;
	}

	CNumMat &A=set.m_A;
	A.resize(m_sequences.size(),1+N1+pairs.size());
	A=0;
	for(unsigned e=0; e<m_sequences.size(); e++)
	{
		A(e,0)=1;
		const CSequence &seq=m_sequences[e];
		if(matrix)
		{
			for(unsigned pos=0; pos<m_seq_length; pos++)
				A(e,1+pos*CLetter::AlphabetSize()+seq[pos].Index())=1;
		}
		for(unsigned p=0; p<pairs.size(); p++)
		{
			if(seq[pairs[p].pos1]==pairs[p].let1)
				if(seq[pairs[p].pos2]==pairs[p].let2)
					A(e,1+N1+p)=1;
		}
	}
}


void CSeqSet::CreateSubset(const CSeqSet &source, UIntVec &members) 
{
	CDataset::CreateSubset(source,members);

	m_seq_length=source.m_seq_length;

	const unsigned M=members.size();
	m_sequences.resize(M);
	for(unsigned m=0; m<M; m++)
		m_sequences[m]	=source.m_sequences[members[m]];
};


unsigned CSeqSet::DiscardOutliers(const CSeqSet &source_set, double stdev_threshold)
{
	const CNumVec & meas=source_set.GetMeasurements();
	const CNumVec & pred=source_set.GetPredictions();
	const Vec<INEQUALITY> &ineq=source_set.GetInequalities();
	
	DoubleVec diff;
	for(unsigned m=0; m<source_set.GetPredictions().size(); m++)
	{
		diff.push_back(pred[m]-meas[m]);
		if(ineq[m]==GREATER && diff.back()>0)
			diff.back()=0;
		else if(ineq[m]==SMALLER && diff.back()<0)
			diff.back()=0;
	}

	DoubleVec sdiff=diff;
	sort(sdiff.begin(),sdiff.end());
	double low=sdiff[unsigned(0.1587 * sdiff.size())];
	double high=sdiff[unsigned(0.8413 * sdiff.size())];
	double stdev=(high-low)/2;

	UIntVec members;
	for(unsigned m=0; m<source_set.GetPredictions().size(); m++)
		if(fabs(diff[m]/stdev)<stdev_threshold)
			members.push_back(m);

	CreateSubset(source_set,members);
	return(meas.size()-members.size());
}




void CSeqSet::AddSet(const CSeqSet &source)
{
//	assert(m_seq_length==source.m_seq_length);
	CDataset::AddSet(source);
	m_sequences.insert(m_sequences.end(),source.m_sequences.begin(),source.m_sequences.end());
};


void CSeqSet::Load(const string & name )
{
	ex_ifstream in(name);
	string x;
	in >> x >> m_seq_length;
	if(in.fail())
		throw(BPException("Could not load Sequence Length for CSeqSet"));

	m_inequalities.clear();
	m_sequences.clear();
	DoubleVec measurements;
	try
	{
		while(!in.fail())
		{
			char s;
			in >> s;
			if(in.fail())
				break;
			switch(s)
			{
			case '>'	:	m_inequalities.push_back(GREATER);	break;
			case '='	:	m_inequalities.push_back(EQUAL);		break;
			case '<'	:	m_inequalities.push_back(SMALLER);	break;
			default		:
				BPException e("Unexcpected character "); 
				e.m_message << s << "Instead of <,=,> when loading Dataset Element";
				throw e;
			}
			double measured;
			in >> measured; 
			if(in.fail())
				throw BPException("Invalid measurement");

			CSequence seq;
			for(unsigned p=0; p<m_seq_length; p++)
			{
				CLetter l;
				in >> l;
				seq.push_back(l);
			}
			if(in.fail())
				throw BPException("Invalid sequence");
			if(seq.size()!=m_seq_length)
				throw(BPException("Tried to load sequence of invalid length"));
			measurements.push_back(measured);
			m_sequences.push_back(seq);
		}
	}
	catch(BPException e)
	{
		BPFileException e2(e,name);
		e2.m_message << endl << "In element number " << m_sequences.size();
		throw(e2);
	}
	m_measurements.SetTo(measurements);	
	m_predictions.resize(measurements.size());
	CDataset::Load(name);
}

void CSeqSet::Save(const string &name, bool predicted) const
{
	ex_ofstream out(name);

	out << "SeqLength:\t" << m_seq_length << endl;

	if(predicted)
	{
		out << EvaluationName() << "\t" << EvaluatePrediction();
		out << "\nPredicted\t\tMeasured"<<endl;
	}
	for(unsigned int i=0; i<m_sequences.size(); i++)
	{
		if(predicted)
			out << m_predictions[i] <<  "\t";
		switch(m_inequalities[i])
		{
		case GREATER	:	out << ">";	break;
		case EQUAL		:	out << "=";	break;
		case SMALLER	:	out << "<";	break;
		}
		out << "\t" << m_measurements[i] << "\t" << m_sequences[i] << endl;
	}
}

/*
void CSeqSet::LoadFixedLength(const string & name, unsigned  length, unsigned subset_num)
{
	m_seq_length=length;
	m_file_name=name;
	m_measurements.clear();
	m_predictions.clear();
	m_inequalities.clear();
	m_sequences.clear();
	DoubleVec measurements;
	ex_ifstream f(name);
	try
	{
		while(!f.fail())
		{
			char s;
			f >> s;
			if(f.fail())
				break;
			switch(s)
			{
			case '>'	:	m_inequalities.push_back(GREATER);	break;
			case '='	:	m_inequalities.push_back(EQUAL);		break;
			case '<'	:	m_inequalities.push_back(SMALLER);	break;
			default		:	
				BPFileException e("Unexpected character ", name); 
				e.m_message << s << "Instead of <,=,> when loading Dataset Element";
				throw BPFileException(e);
			}
			double measured;
			f >> measured;
			measurements.push_back(measured);
			CSequence seq;
			if(f.fail())
				throw BPException("Incomplete sequence");
			m_sequences.push_back(seq);
		}
	}
	catch(BPException e)
	{
		BPFileException e2(e, name);
		e2.m_message << endl << "In element number " << m_sequences.size();
		throw(e2);
	}
	m_measurements.SetTo(measurements);
	if(subset_num!=0)
	{
		m_sub_blind.clear();
		m_sub_train.clear();
		
		EnterSubDir();

		unsigned pos=m_file_name.find('.');
		if(pos==string::npos)
			pos=m_file_name.size();

		for(unsigned sec=0; sec<subset_num; sec++)
		{
			ostringstream bname, tname;

			bname << m_file_name.substr(0,pos) << "_b" << sec+1 <<"-" << subset_num << m_file_name.substr(pos) << '\0';
			tname << m_file_name.substr(0,pos) << "_t" << sec+1 <<"-" << subset_num << m_file_name.substr(pos) << '\0';

			CSeqSet t,b;
			t.LoadFixedLength(tname.str(),length);
			b.LoadFixedLength(bname.str(),length);
			assert(t.m_sequences.size()+ b.m_sequences.size()==m_sequences.size());
			m_sub_blind.push_back(b);
			m_sub_train.push_back(t);
		}
		ex_chdir("..");
	}
}
*/
