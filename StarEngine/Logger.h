#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

#include <string>
#include <fstream>
#include <iostream>
#include "TextUtils.h"

using namespace std;

#define LOGGER_WRITE(PRIORITY, MESSAGE) Logger::Write(PRIORITY, MESSAGE);
//#define LOGGER_LOG(PRIORITY, MESSAGE) Logger::Log(PRIORITY, MESSAGE);

class Logger
{
public:
	// log priorities
	enum Priority
	{
		DEBUG,
		CONFIG,
		INFO,
		WARNING,
		LERROR
	};

	// start/stop logging
	// - messages with priority >= minPriority will be written in log
	// - set logFile = "" to write to standard output
	static void Start(Priority minPriority, const string& logFile);
	static void Stop();

	// write message
	static void Write(Priority priority, const string& message);
	//Log to console
	//static void Log(Priority priority, const string& message);

private:
	// Logger adheres to the singleton design pattern, hence the private
	// constructor, copy constructor and assignment operator.
	Logger();
	Logger(const Logger& logger) {}
	Logger& operator = (const Logger& logger);

	// private instance data
	bool        active;
	ofstream    fileStream;
	Priority    minPriority;

	// names describing the items in enum Priority
	static const string PRIORITY_NAMES[];
	// the sole Logger instance (singleton)
	static Logger instance;
};

#endif //LOGGER_H_INCLUDED