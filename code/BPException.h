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

// This is somewhat of a junk file, containing code for
// - Exceptions
// - File and directory routines to mask differences between windows / linux, which will throw exceptions if an error occurs
// - data conversion functions for TinyXML Elements


#pragma once

#include <string>
#include <fstream>
#include <cassert>
#include <sstream>
#include <algorithm>
#include "tinyxml.h"
#include <iostream>

using namespace std;

class BPException
{
public:
  BPException(const string &message="") {m_message << message.c_str();};
  BPException(const BPException& bpe)	{m_message << bpe.m_message.rdbuf();};
  ~BPException()	{};

public:
	stringstream m_message;
};

class BPFileException : public BPException
{
public:
	BPFileException(BPException bpex, string filename) : BPException(bpex), m_filename(filename)	{};
	BPFileException(string message, string filename) : BPException(message), m_filename(filename)	{};
public:
	string m_filename;
};


class ex_ofstream : public ofstream
{
public:
	ex_ofstream()	{};

	ex_ofstream(const string &name) : ofstream(name.c_str())	
	{
		if(fail())
			throw(BPFileException("Failed to open", name));
	}
	void open(const string &name)
	{
		if(fail())
			cout << "ahlas";
		ofstream::open(name.c_str());
		if(fail())
			throw(BPFileException("Failed to open", name));
	}
};

class ex_ifstream : public ifstream
{
public:
	ex_ifstream()	{};
	ex_ifstream(const string &name) : ifstream(name.c_str())	
	{
		if(fail())
			throw(BPFileException("Failed to open", name));
	}
	void open(const string &name)
	{
		ifstream::open(name.c_str());
		if(fail())
			throw(BPFileException("Failed to open", name));
	}
};


void	ex_chdir(const string &name);
void	ex_mkdir(const string &name);
string	ex_getcwd();

bool            ToBool(const TiXmlNode *node);
unsigned int	ToUInt(const TiXmlNode *node);
double			ToDouble(const TiXmlNode *node);
int				ToInt(const TiXmlNode *node);
const char*		ToText(const TiXmlNode *node);

