#ifndef TEST_LOGGER_H
#define TEST_LOGGER_H

#include <string>
#include <string.h>

enum LogType
{
	info = 0,
	success,
	error
};

void logger(LogType type, std::string msg);
void log_assertion(bool condition, std::string test_name);

#endif
