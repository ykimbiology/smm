

// GOAL: Implement command line interface for SMM program.

// Copyright Vladimir Prus 2002-2004.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <string>
#include <boost/program_options.hpp>

using namespace boost;
namespace po = boost::program_options;




// A helper function to simplify the main part.
//template<class T>
//ostream& operator<<(ostream& os, const vector<T>& v)
//{
//    copy(v.begin(), v.end(), ostream_iterator<T>(os, " "));
//    return os;
//}


void CommandLineInterfaceBoost(int ac, char* av[])
{
	// This version uses the Boost c++ library to implement a commandline interface.
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

        // Training:
        if (vm.count("input") && (!vm.count("matrix")))
        {
        	std::cout << "Train a scoring matrix." << std::endl;
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

int main(int ac, char* av[])
{
	CommandLineInterfaceBoost(ac,av);
    return 0;
}
