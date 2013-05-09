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
#include "BPException.h"

#ifdef _WIN32

#include <direct.h>

#else

const unsigned _MAX_PATH=5000;
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#endif



using namespace std;

string ex_getcwd()
{
	char buffer[_MAX_PATH];
#ifdef _WIN32
	if(_getcwd(buffer,_MAX_PATH)==NULL)
		throw(BPException("Could not determine directory"));
#else
	if(getcwd( buffer, _MAX_PATH )==NULL)
		throw(BPException("Could not determine directory"));
#endif
	return(string(buffer));
}

void ex_chdir(const string &name)
{
#ifdef _WIN32
	if(_chdir(name.c_str()))
		throw(BPFileException("Could not change into directory", name));
#else
	if(chdir(name.c_str()))
	   throw(BPFileException("Could not change into directory", name));
#endif
}


void ex_mkdir(const string &name)
{
#ifdef _WIN32
	if(_mkdir(name.c_str()))
       throw(BPFileException("Could not create directory", name));
#else
	if(mkdir(name.c_str(),S_IRWXU))
       throw(BPFileException("Could not create directory", name));
#endif
}


bool ToBool(const TiXmlNode *node)
{
	const TiXmlNode *child=node->FirstChild();
	if(!child)
		throw BPException("No child element");
	string text=child->Value();
	if((text=="true") || (text=="True") || (text=="TRUE"))
		return(true);
	else if (text=="false" || text=="False" || text=="FALSE")
		return(false);
	throw BPException(string("Invalid XML content, Bool expected, but this turned up: ") + text);
}



unsigned int ToUInt(const TiXmlNode *node)
{
	const TiXmlNode *child=node->FirstChild();
	if(!child)
		throw BPException("No child element");
	const char *text=child->Value();
	char *stop;
	unsigned int value=strtoul(text,&stop,10);
	if(*stop!=0)
		throw BPException(string("Invalid XML content, UInt expected, but this turned up: ") + text);
	return(value);
}

double ToDouble(const TiXmlNode *node)
{
	const TiXmlNode *child=node->FirstChild();
	if(!child)
		throw BPException("No child element");
	const char *text=child->Value();
	char *stop;
	double value=strtod(text,&stop);
	if(*stop!=0)
		throw BPException(string("Invalid XML content, double expected, but this turned up: ") + text);
	return(value);
}

int	ToInt(const TiXmlNode *node)
{
	const TiXmlNode *child=node->FirstChild();
	if(!child)
		throw BPException("No child element");
	const char *text=child->Value();
	char *stop;
	int value=strtol(text,&stop,10);
	if(*stop!=0)
		throw BPException(string("Invalid XML content, int expected, but this turned up: ") + text);
	return(value);
}

const char *ToText(const TiXmlNode *node)
{
	const TiXmlNode *child=node->FirstChild();
	if(!child)
		throw BPException("No child element");
	const char *text=child->Value();
	return(text);
}

