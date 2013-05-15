// Copyright Vladimir Prus 2002-2004.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

/* The simplest usage of the library.
 */

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <iostream>
#include <iterator>
using namespace std;

int main(int ac, char* av[])
{
    try {

        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce help message")
            ("train", "Train a model")
            ("predict", "Make predictions")
        ;

        po::variables_map vm;
        po::store(po::parse_command_line(ac, av, desc), vm);
        po::notify(vm);

//        if (vm.count("help")) {
//            cout << desc << "\n";
//            return 0;
//        }
//
//        if (vm.count("compression")) {
//            cout << "Compression level was set to "
//                 << vm["compression"].as<double>() << ".\n";
//        } else {
//            cout << "Compression level was not set.\n";
//        }

        if (vm.count("help")) {
        //if (vm.count("")) {
            cout << desc << "\n";
            return 0;
        }

        if (vm.count("train")) {
            cout << "Training: " << "\n";

        } else if (vm.count("predict")) {
        	cout << "Predicting: " << "\n";
        }

        else {
            cout << "use --help for details.\n";
        }


    }
    catch(exception& e) {
        cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...) {
        cerr << "Exception of unknown type!\n";
    }

    return 0;
}
