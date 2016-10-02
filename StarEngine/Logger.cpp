#include "Logger.h"

void Logger::Start(Priority minPriority, const string& logFile)
{
	instance.active = true;
	instance.minPriority = minPriority;
	if (logFile != "")
	{
		instance.fileStream.open(logFile.c_str());
	}
}

void Logger::Stop()
{
	instance.active = false;
	if (instance.fileStream.is_open())
	{
		instance.fileStream.close();
	}
}

void Logger::Write(Priority priority, const string& message)
{
	if (instance.active && priority >= instance.minPriority)
	{
		// identify current output stream
		ostream& stream
			= instance.fileStream.is_open() ? instance.fileStream : std::cout;

		stream  << PRIORITY_NAMES[priority]
		<< ": "
			<< message
			<< endl;
	}
}

const string Logger::PRIORITY_NAMES[] =
{
	"DEBUG",
	"CONFIG",
	"INFO",
	"WARNING",
	"ERROR"
};

