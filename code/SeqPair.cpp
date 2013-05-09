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
#include "SeqPair.h"
#include "math.h"

CSeqPair::CSeqPair(void)  
{
}

CSeqPair::~CSeqPair(void)
{
}

double CSeqPair::Score(CSequence::const_iterator begin, CSequence::const_iterator end)	const
{
	double score=CSeqMatrix::Score(begin,end);
	for(Vec<PairCoef>::const_iterator p=m_pair_coef.begin(); p<m_pair_coef.end(); p++)
		if(*(begin+p->pos1)==p->let1)
			if(*(begin+p->pos2)==p->let2)
				score+=p->val;
	return(score);
}




void CSeqPair::Load	(const string & name)
{
	ex_ifstream in(name);
	string x;
	unsigned length;
	in >> x >> length;
	CSeqMatrix::Load(in,length,false);

	m_pair_coef.clear();
	unsigned pair_count;
	in >> x >> pair_count;
	if(!in.fail())
	{
		m_pair_coef.resize(pair_count);
		in >> x >> x >> x >> x >> x;
		for(unsigned i=0; i<pair_count; i++)
		{
			PairCoef p;
			in >> p.val >> p.let1 >> p.pos1 >> p.let2 >> p.pos2;
			p.pos1--;
			p.pos2--;
			m_pair_coef[i]=p;
			if(in.fail())
			{
				BPFileException e("Unable to load pair coefficients",name);
				e.m_message << endl << "Coef Number: " << i+1 <<" / " << pair_count;
				throw e;
			}
		}
	}
}


void CSeqPair::Save	(const string & name)		const
{
	CSeqMatrix::Save(name);
	ofstream out;
	out.open(name.c_str(),ios_base::app);
	if(out.fail())
		throw BPFileException("Failed to append pair coefficients to file",name);
	out << endl << "NumPairs:\t"<< m_pair_coef.size() << endl;
	out << "Val\tLet1\tPos1\tLet2\tPos2";
	for(unsigned i=0; i<m_pair_coef.size(); i++)
	{
		const PairCoef &p=m_pair_coef[i];
		out  <<  endl << p.val << "\t" <<  p.let1 << "\t" <<  p.pos1 +1 << "\t" <<  p.let2 << "\t" <<  p.pos2+1;
		if(out.fail())
			throw BPFileException("Failed to save pair coefficients",name);
	}
}

void CSeqPair::ToXml(TiXmlNode *node) const
{
	CSeqMatrix::ToXml(node);

	TiXmlElement elet1("Letter1");
	TiXmlElement elet2("Letter2");
	TiXmlElement epos1("Position1");
	TiXmlElement epos2("Position2");
	TiXmlElement eval ("Value");
	
	TiXmlText a("");
	epos1.InsertEndChild(a);
	epos2.InsertEndChild(a);
	elet1.InsertEndChild(a);
	elet2.InsertEndChild(a);
	eval.InsertEndChild(a);

	TiXmlElement pair("PairCoef");
	pair.InsertEndChild(elet1);
	pair.InsertEndChild(epos1);
	pair.InsertEndChild(elet2);
	pair.InsertEndChild(epos2);
	pair.InsertEndChild(eval);
	


	TiXmlText *let1=pair.FirstChild("Letter1")->FirstChild()->ToText();
	TiXmlText *pos1=pair.FirstChild("Position1")->FirstChild()->ToText();
	TiXmlText *let2=pair.FirstChild("Letter2")->FirstChild()->ToText();
	TiXmlText *pos2=pair.FirstChild("Position2")->FirstChild()->ToText();
	TiXmlText *val =pair.FirstChild("Value")->FirstChild()->ToText();

	TiXmlElement pairs("SeqPair");
	for(unsigned p=0; p<m_pair_coef.size(); p++)
	{
		const PairCoef &pc=m_pair_coef[p];

		string l;
		l=pc.let1.Letter();
		let1->SetValue(l);
		l=pc.let2.Letter();
		let2->SetValue(l);
		
		stringstream v, p1, p2;
		v << pc.val;
		p1<< pc.pos1 +1;
		p2<< pc.pos2 +1;

		val->SetValue (v.str());
		pos1->SetValue(p1.str());
		pos2->SetValue(p2.str());
		pairs.InsertEndChild(pair);
	}
	node->InsertEndChild(pairs);
}


void CSeqPair::FromXml(TiXmlNode *node, unsigned seq_length)
{
	m_pair_coef.clear();
	CSeqMatrix::FromXml(node,seq_length);
	node=node->FirstChild("SeqPair");
	if(node)
	{
		node=node->FirstChild("PairCoef");
		while(node)
		{
			PairCoef pc;
			pc.pos1=ToUInt(node->FirstChild("Position1"))-1;
			pc.pos2=ToUInt(node->FirstChild("Position2"))-1;
			CLetter let1(ToText(node->FirstChild("Letter1"))[0]);
			CLetter let2(ToText(node->FirstChild("Letter2"))[0]);
			pc.let1=let1;
			pc.let2=let2;
			pc.val=ToDouble(node->FirstChild("Value"));
			m_pair_coef.push_back(pc);
			node=node->NextSibling();
		}
	}
}



unsigned CSeqPair::IdentifyPairCoef(const CSeqSet &set, unsigned min_count)
{
	m_pair_coef.clear();
	Mat<CSeqMatrix> cmat(CLetter::AlphabetSize(),NumCols());
	for(unsigned r=0; r<cmat.NumRows(); r++)
		for(unsigned c=0; c<cmat.NumCols(); c++)
			cmat(r,c).resize(NumCols());

	const Vec<CSequence> &seqs=set.GetSequences();
	for(Vec<CSequence>::const_iterator s=seqs.begin(); s<seqs.end(); s++)
		for(unsigned p1=0; p1<NumCols()-1; p1++)
		{
			CSeqMatrix &mat=cmat((*s)[p1].Index(),p1);
			for(unsigned p2=p1+1; p2<NumCols(); p2++)
			{
				double val=++mat.Matrix((*s)[p2],p2);
				if(val==min_count)
				{
					PairCoef pair;
					pair.val=0;
					pair.let1=(*s)[p1];
					pair.let2=(*s)[p2];
					pair.pos1=p1;
					pair.pos2=p2;
					m_pair_coef.push_back(pair);
				}
			}
		}
	return(m_pair_coef.size());
}


void CSeqPair::DropPairCoef(const CSeqSet &set,double percent)
{
	if(percent<0.0 || percent>0.5)
		throw BPException("Invalid argument for Pair Coefficient selection: MaxDisagreemnent has to be between 0.0 and 0.5");
	const Vec<CSequence> &seqs=set.GetSequences();
	Vec<PairCoef> pc;
	for(unsigned p=0; p<m_pair_coef.size(); p++)
	{
		DoubleVec vals;
		PairCoef &pair=m_pair_coef[p];
		for(unsigned s=0; s<seqs.size(); s++)
		{
			const CSequence &seq=seqs[s];
			if(seq[pair.pos1]==pair.let1)
				if(seq[pair.pos2]==pair.let2)
					if(set.GetInequalities()[s]==EQUAL)
						vals.push_back(set.GetMeasurements()[s]-set.GetPredictions()[s]);
		}
		double positives=0;
		for(unsigned v=0; v<vals.size(); v++)
			if(vals[v]>0)
				positives++;
		positives/=vals.size();
		if(positives<percent || positives>1-percent)
			pc.push_back(pair);
	}
	m_pair_coef=pc;
}


