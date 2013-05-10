// File name: HelloWorld.cpp
// Purpose:   A simple C++ program which prints "Hello World!" on the screen

#include <iostream>  // need this header file to support the C++ I/O system
using namespace std; // telling the compiler to use namespace "std",
		     // where the entire C++ library is declared.
	
int main()
{
        // Print out a sentence on the screen.
        // "<<" causes the expression on its right to 
        // be directed to the device on its left.
        // "cout" is the standard output device -- the screen.
	cout << "Hello World!" <<  endl;
	return 0;
		     
}
