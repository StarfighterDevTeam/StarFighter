#ifndef TEXTUTILS_H_INCLUDED
#define TEXTUTILS_H_INCLUDED

#include "Logger.h"

#include <vector>
#include <string>
#include <cstdarg>
#include <iostream>
#include <sstream>
using namespace std;

class TextUtils
{
public:
	static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}

	static std::vector<std::string> split(const std::string &s, char delim) {
		std::vector<std::string> elems;
		split(s, delim, elems);
		return elems;
	}

	static inline string format(const char* fmt, ...){
		int size = 512;
		char* buffer = 0;
		buffer = new char[size];
		va_list vl;
		va_start(vl, fmt);
		int nsize = vsnprintf(buffer, size, fmt, vl);
		if(size<=nsize){ 
			delete[] buffer;
			buffer = 0;
			buffer = new char[nsize+1]; 
			nsize = vsnprintf(buffer, size, fmt, vl);
		}
		std::string ret(buffer);
		va_end(vl);
		delete[] buffer;
		return ret;
	}

	static string float2string(float f)
	{
		ostringstream os;
		os << f;
		return os.str();
	}

};

#endif // EXCEPTIONS_H_INCLUDED