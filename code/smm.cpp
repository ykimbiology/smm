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

//Used by CommandLineInterfaceBoost
#include <iostream>
#include <string>
#include <boost/program_options.hpp>


#include "stdafx.h"
#include "SMMSeqMatrix.h"
#include "SMMSeqPair.h"
#include "tinyxml.h"

//Used by CommandLineInterfaceBoost
using namespace boost;
namespace po = boost::program_options;


//void SMMTrain(TiXmlNode *input, TiXmlNode *output)
//{
//	// Init Sequence Alphabet and length (has to be valid for all training data)
//	TiXmlNode *train_node=input->FirstChild("TrainingData");
//	CLetter::Init(ToText(train_node->FirstChild("Alphabet")));
//	unsigned seq_length =ToUInt(train_node->FirstChild("SequenceLength"));
//
//	// Load training data
//	CSeqSet train_set;
//	CSeqMatrix mat_ext;
//	train_set.FromXml(train_node,seq_length);
//	if(train_set.ElementNumber()==0)
//		throw BPException("No training data found");
//	if(train_node->FirstChild("SeqMatrix"))
//		mat_ext.FromXml(train_node,seq_length);
//
//	if(!input->FirstChild("MatrixCalculation"))
// 	{
//		// No information on how to train --> enter auto mode
//		// Decide training mode based on degrees of freedom and available data.
//		// Insert training mode into "input"
//
//		unsigned freedom=CLetter::AlphabetSize() * seq_length;
//		TiXmlElement matpar("MatrixCalculation");
//
//		if(mat_ext.NumCols()>0)
//		{
//			TiXmlElement t("UseExternalMatrix");
//			matpar.InsertEndChild(t);
//		}
//		if(train_set.ElementNumber()>freedom || mat_ext.NumCols()==0)
//		{
//			TiXmlElement t("SMMMatrix");
//			matpar.InsertEndChild(t);
//		}
//		input->InsertEndChild(matpar);
//
//		if(train_set.ElementNumber()>1.5*freedom)
//		{
//			TiXmlElement pairpar("PairCalculation");
//			input->InsertEndChild(pairpar);
//		}
//	}
//
//	// Start calculation
//	// First determine the Matrix:
//	CSMMSeqMatrix mat;
//
//	TiXmlNode *mat_node=input->FirstChild("MatrixCalculation");
//	if(mat_node->FirstChild("UseExternalMatrix"))
//	{
//		if(mat_ext.NumCols()!=seq_length)
//			throw BPException("No external matrix supplied or incorrect length");
//
//		// Adjust External Matrix as specified
//		if(mat_node->FirstChild("UseExternalMatrix")->FirstChild("AdjustToSequenceData"))
//			mat_ext.AdjustMatrix(train_set);
//		else if(mat_node->FirstChild("UseExternalMatrix")->FirstChild("AdjustOffsetToSequenceData"))
//			mat_ext.AdjustOffset(train_set);
//
//		if(mat_node->FirstChild("SMMMatrix"))
//		{
//			// Train SMM Matrix on difference to external matrix prediction
//			CSeqSet train_diff=train_set;
//			mat_ext.Predict(train_diff);
//			train_diff.SubtractPrediction();
//
//			mat.XMLParameters(mat_node->FirstChild("SMMMatrix"));
//			mat.TrainMatrix(train_diff);
//			mat+=mat_ext;
//		}
//		else
//			mat.CSeqMatrix::operator=(mat_ext);
//	}
//	else
//	{
//		if(!mat_node->FirstChild("SMMMatrix"))
//			throw BPException("Invalid Training Parameters: Either UseExternalMatrix or SMMMatrix have to be specified");
//
//		mat.XMLParameters(mat_node->FirstChild("SMMMatrix"));
//		mat.TrainMatrix(train_set);
//	}
//
//	// Now Determine Pair Coefficients if applicable:
//	CSMMSeqPair pair;
//	if(input->FirstChild("PairCalculation"))
//	{
//		pair.CSeqMatrix::operator=(mat);
//		pair.XMLParameters(input->FirstChild("PairCalculation"));
//		pair.TrainPair(train_set);
//	}
//
//	// Calculation completed succesfully
//
//	// Save results in output
//	// Copy Alphabet, Sequence Length from Input File
//	TiXmlElement npred("SMMPredictor");
//	npred.InsertEndChild(*train_node->FirstChild("Alphabet"));
//	npred.InsertEndChild(*train_node->FirstChild("SequenceLength"));
//
//	if(pair.GetPairs().size()>0)
//		pair.ToXml(&npred);
//	else
//		mat.ToXml(&npred);
//	output->InsertEndChild(npred);
//}

//void SMMPredict(TiXmlNode *input, TiXmlNode *output)
//{
//	// Read Predictor Information from input
//	TiXmlNode *predictor=input->FirstChild("SMMPredictor");
//	CLetter::Init(ToText(predictor->FirstChild("Alphabet")));
//	unsigned seq_length =ToUInt(predictor->FirstChild("SequenceLength"));
//	CSeqPair pair;
//	pair.FromXml(predictor,seq_length);
//
//
//	// Prepare "Prediction" output element
//	TiXmlElement pred("Predictions");
//	TiXmlText pred_value(""), *pv;
//	pred.InsertEndChild(pred_value);
//	pv=pred.FirstChild()->ToText();
//
//	// Loop Predict Elements in Input
//	TiXmlNode *eseq=input->FirstChild("Predict");
//	while(eseq)
//	{
//		string s = ToText(eseq->FirstChild("Sequence"));
//
//		CSequence seq;
//		for(unsigned i=0; i<s.size(); i++)
//			seq.push_back(s[i]);
//		stringstream ptext;
//		if(s.size()>=seq_length)
//		{
//			for(unsigned   j=0; j<=s.size()-seq_length; j++)
//			  ptext << pair.Score(CSequence::const_iterator(&seq[j]),CSequence::const_iterator(&seq[j+seq_length-1]+1)) << " ";
//		}
//		pv->ToText()->SetValue(ptext.str());
//
//		TiXmlElement pout=*eseq->ToElement();
//		pout.SetAttribute("ScanLength",seq_length);
//		pout.InsertEndChild(pred);
//		output->InsertEndChild(pout);
//		eseq=eseq->NextSibling("Predict");
//	}
//}

string AutoFileName(const string & original, const string &prefix)
{
	unsigned pos1 = original.find_last_of('\\',original.size());
	unsigned pos2 = original.find_last_of('/',original.size());
	
	unsigned pos = min(pos1,pos2);
	if(pos==original.size())
		return(prefix + "-" + original);

	if(max(pos1,pos2)<original.size())
		pos=max(pos1,pos2);
	string result;
	result = original.substr(0,pos+1);
	result +=prefix;
	result +="-";
	result +=original.substr(pos+1,original.size()-pos);
	return(string(result));
}




//void CommandLineInterface(int argc, char* argv[])
//{
//	CLetter::Init("ACDEFGHIKLMNPQRSTVWY");	// Assume peptide sequences as input
//	const string command=argv[1];
//	if(command=="-tm" || command=="-tp" || command=="-t")
//	{
//		if(argc==2)
//			throw BPException("No Input file name specified");
//
//		const string in_file_name=argv[2];
//		CSeqSet set;
//		set.Load(in_file_name);
//
//		if(command=="-tm" || (command=="-t" && (set.ElementNumber()<1.5 * 20.0 * set.SeqLength())))
//		{
//			CSMMSeqMatrix mat;
//			mat.TrainMatrix(set);
//			if(argc==3)
//				mat.Save(AutoFileName(in_file_name,"mat"));
//			else
//				mat.Save(argv[3]);
//		}
//		else
//		{
//			std:cout << "Calculating paircoefs.\n" << endl;
//			CSMMSeqMatrix mat;
//			mat.TrainMatrix(set);
//
//			CSMMSeqPair pair;
//			pair.CSeqMatrix::operator=(mat);
//			pair.TrainPair(set);
//			if(argc==3)
//				pair.Save(AutoFileName(in_file_name,"pair"));
//			else
//				pair.Save(argv[3]);
//		}
//
//	}
//	else if(command=="-p")
//	{
//		if(argc<4)
//			throw BPException("Prediction requires at least two file names as input parameters");
//
//		CSeqPair pair;
//		pair.Load(argv[2]);
//		const unsigned seq_length=pair.NumCols();
//
//		ex_ifstream seq_input(argv[3]);
//
//		ex_ofstream out;
//		if(argc>4)
//			out.open(argv[4]);
//		else
//			out.open(AutoFileName(argv[3],"pred"));
//
//		while(!seq_input.fail())
//		{
//			CSequence seq;
//			seq_input >> seq;
//			if(!seq_input.fail())
//			{
//				out << seq;
//				if(seq.size()>=seq_length)
//				{
//					out << "\t";
//					for(unsigned j=0; j<=seq.size()-seq_length; j++)
//						out << pair.Score(CSequence::const_iterator(&seq[j]),CSequence::const_iterator(&seq[j+seq_length-1]+1)) << "\t";
//				}
//				out << endl;
//			}
//		}
//	}
//	else
//		throw BPException(string("Unknown command line parameter '"+command+"'"));
//}

void CommandLineInterfaceBoost(int ac, char* av[])
{
	// This version uses the Boost c++ library to implement a commandline interface.

	CLetter::Init("ACDEFGHIKLMNPQRSTVWY");	// Assume peptide sequences as input
    try {
        std::string fname_input;
        std::string fname_output;
        std::string fname_matrix;
        int min_count_data;

        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "produce help message")
            ("input,i",  po::value<std::string>(&fname_input),
                "Input file name")
            ("output,o", po::value<std::string>(&fname_output),
                "Output file name")
            ("paircoef,n", po::value<int>(&min_count_data),
                "Train pair-coefficients (Specify minimum number of data points to require.)")
            ("matrix,m", po::value<std::string>(&fname_matrix),
                "File containing the trained scoring matrix.")
        ;


        po::variables_map vm;
        po::store(po::parse_command_line(ac, av, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << "Usage: ./smm [options]\n";
            std::cout << desc;
            //return 0;
        }

        if (vm.count("input"))
        {
            std::cout << "Input: " << vm["input"].as<std::string>() << "\n";
        }

        if (vm.count("output"))
        {
            std::cout << "Output: " << vm["output"].as<std::string>() << "\n";
        }

        if (vm.count("paircoef"))
        {
            std::cout << "Paircoef: " << vm["paircoef"].as<int>() << "\n";
        }

        if (vm.count("matrix"))
        {
            std::cout << "Matrix: " << vm["matrix"].as<std::string>() << "\n";
        }



        // Training: scoring matrix
        if (vm.count("input") && (!vm.count("matrix")) && (!vm.count("paircoef")))
        {
        	std::cout << "Train a scoring matrix." << std::endl;
    		//const string in_file_name=argv[2];
    		CSeqSet set;
    		set.Load(fname_input);

			CSMMSeqMatrix mat;
			mat.TrainMatrix(set);
			if (vm.count("output")) {
				mat.Save(fname_output);
			} else {
				mat.Save(AutoFileName(fname_input,"mat"));
			}
        }

        // Training: scoring matrix + paircoefs
        if (vm.count("input") && (!vm.count("matrix")) && (vm.count("paircoef")))
        {
        	std::cout << "Train a scoring matrix + paircoefs." << "-n " << min_count_data << std::endl;

    		CSeqSet set;
    		set.Load(fname_input);

			CSMMSeqMatrix mat;
			mat.TrainMatrix(set);

			CSMMSeqPair pair(min_count_data);
			pair.CSeqMatrix::operator=(mat);
			pair.TrainPair(set);
			if (vm.count("output")) {
				pair.Save(fname_output);
			} else {
				pair.Save(AutoFileName(fname_input,"pair"));
			}
        }

        // Prediction:
        if (vm.count("input") && (vm.count("matrix")))
        {
            std::cout << "Predict binding affinities, using the provided scoring matrix." << std::endl;
        }

    }
    catch(std::exception& e)
    {
        std::cout << e.what() << "\n";
        //return 1;
    }
}

int main(int argc, char* argv[])
{
//	if(argc < 2) // Missing parameters. Display usage information
//	{
//		cout << "smm - first public release (1.0)"<< endl;
//		cout << "Usage1: smm.exe [xml_filename]" << endl;
//		cout << "Usage2: smm.exe [parameter] [file1] [file2] [file3]" << endl;
//		cout << "The readme.htm file describes both calling formats."<< endl;
//		cout << "Check http://www.mhc-pathway.net/smm for additional support" << endl;
//		return(0);
//	}

	stringstream output_clog, output_cerr;
	try
	{
		// Redirect clog, cerr to strstreams, which can later be inserted into the output file
		streambuf* clog_old = clog.rdbuf(output_clog.rdbuf());
		streambuf* cerr_old = cerr.rdbuf(output_cerr.rdbuf());

		clog_detail.push_back(MEDIUM);	// Default output detail
		
		bool use_xml=true;
		
		TiXmlBase::SetCondenseWhiteSpace(false);
		TiXmlDocument input, output;
		string output_name="error_out.xml";	// Use if no output filename is specified in input
		
		try
		{
			string in_file_name=argv[1];
			use_xml=false;
			//cout << endl << "Starting...";
			cout.flush();
			CommandLineInterfaceBoost(argc,argv);
			//cout << endl << "Executed successfully" << endl;


//			string in_file_name=argv[1];
//			if(in_file_name[0]=='-') // Use simplified command line interface
//			{
//				use_xml=false;
//				cout << endl << "Starting...";
//				cout.flush();
//				CommandLineInterface(argc,argv);
//				cout << endl << "Executed succesfully" << endl;
//			}
//			else
//			{
//				// Prepare XML output
//
//				const char* output_base=
//				"<?xml version=\"1.0\"  encoding=\"utf-8\" ?>\n"
//				"<SMMOutput xmlns=\"http://www.mhc-pathway.net/smm\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.mhc-pathway.net/smm smm.xsd\" >\n"
//				"<Log success=\"?\" /> \n"
//				"</SMMOutput> ";
//				output.Parse(output_base);
//
//				// Open the input document
//				if(!input.LoadFile(in_file_name))
//				throw BPFileException("Unable to open XML Input File",in_file_name);
//
//				output_name=ToText(input.RootElement()->FirstChild("OutputFile"));
//
//				if(input.FirstChild("SMMTrainInput"))
//				{
//					output.RootElement()->SetValue("SMMTrainOutput");
//					SMMTrain(input.FirstChild("SMMTrainInput"),output.FirstChild("SMMTrainOutput"));
//				}
//				else if(input.FirstChild("SMMPredictInput"))
//				{
//					output.RootElement()->SetValue("SMMPredictOutput");
//					SMMPredict(input.FirstChild("SMMPredictInput"),output.FirstChild("SMMPredictOutput"));
//				}
//				else
//				throw BPFileException("Invalid Input file",in_file_name);
//				// End Succesfully
//				TiXmlNode *nlog=output.RootElement()->FirstChild("Log");
//				nlog->ToElement()->SetAttribute("success","true");
//
//				if(output.SaveFile(output_name)==false)
//				throw BPFileException("Could not save output to",output_name);
//			}

			clog.rdbuf(clog_old);
			cerr.rdbuf(cerr_old);
			return(0);
			
		}
		catch(BPFileException e)
		{
		    cerr << endl<< endl<< "File related exception, program terminated";
		    cerr << endl<< endl<< "Message: " <<  e.m_message.str();
		    cerr << endl<< endl<< "File: " << e.m_filename;
		    cerr << endl<< endl<< "Current Directory: " << ex_getcwd();
		}
		catch(BPException e)
		{
		    e.m_message << endl << '\0';
		    cerr << endl<< endl<< "Exception, program terminated";
		    cerr << endl<< endl<< "Message:" <<  e.m_message.str();
		}
		catch(...)
		{
		    cerr << endl<< "Some other exception.";
		}
		if(use_xml)
		{
			// End with error
			TiXmlNode *nlog=output.RootElement()->FirstChild("Log");
			nlog->ToElement()->SetAttribute("success","false");
			
			// Insert error message into output
			cerr << '\0';
			TiXmlText cerr_text(output_cerr.str());
			TiXmlElement cerr_element("ErrorMessage");
			cerr_element.InsertEndChild(cerr_text);
			nlog->InsertEndChild(cerr_element);
			
			// Insert log into output
			clog << '\0';
			TiXmlText clog_text(output_clog.str());
			TiXmlElement clog_element("LogText");
			clog_element.InsertEndChild(clog_text);
			nlog->InsertEndChild(clog_element);
			
			if(output.SaveFile(output_name)==false)
				output.SaveFile("default_out.xml");
		}
		else
		{
			cout << endl;
			cout << "An error occured:" << endl;
			cout << output_cerr.str() << endl;
		}
		clog.rdbuf(clog_old);
		cerr.rdbuf(cerr_old);
		return(-1);
	}
	catch(...)
	{
		cerr << endl<< "Exception. I am really not supposed to occur. cerr";
		cout << endl<< "Exception. I am really not supposed to occur. cout";
		return (-2);
	}
	cerr << endl << "Now this should never happen.";
	cout << endl << "Now this should never happen.";
	return(-3);
}



