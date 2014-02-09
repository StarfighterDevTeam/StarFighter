#ifndef EXCEPTIONS_H_INCLUDED
#define EXCEPTIONS_H_INCLUDED


#include <exception>
#include <string>
#include <cstdarg>
using namespace std;

enum ExceptionLevel {
	Warning,
	Error,
	FatalError,
	NBVAL_ExceptionLevel
};

class ExceptionUtils
{

public:
	static string getExceptionMessage (ExceptionLevel lvl, string message)
	{
		switch(lvl)
		{
		case Warning:
			return "[WRN]" + message;
		case Error:
			return "[ERR]" + message;
		case FatalError:
			return "[FATAL]" + message;
		default:
			return "[]" + message;
		}

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

};



#endif // EXCEPTIONS_H_INCLUDED