#ifndef TEXTUTILS_H_INCLUDED
#define TEXTUTILS_H_INCLUDED

#include <vector>
#include <string>
#include <iostream>
#include <sstream>

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
};

#endif // EXCEPTIONS_H_INCLUDED