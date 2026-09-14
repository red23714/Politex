#include <string>
#include <ctime>

#include <iostream>

#include "test_logger.h"

using namespace std;

void logger(LogType type, string msg) {
    auto current_time = time(0);
    char current_time_str[size("yyyy-mm-ddThh:mm:ssZ")];
    tm calendar_datetime;
    #if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
        gmtime_r(&current_time, &calendar_datetime);
    #elif _WIN32
        gmtime_s(&calendar_datetime, &current_time);
    #endif
    strftime(current_time_str, size(current_time_str), "%FT%TZ", &calendar_datetime);
    current_time_str[strlen(current_time_str) - 1] = '\0';

    switch (type)
    {
    case info:
        cout << "\033[34m" << "[" << current_time_str << "]" << " INFO: " << msg << "\033[0m\n";
        break;
    case success:
        cout << "\033[32m" << "[" << current_time_str << "]" << " Success: " << msg << "\033[0m\n";
        break;
    case error:
        cout << "\033[31m" << "[" << current_time_str << "]" << " Error: " << msg << "\033[0m\n";
        break;
    }
}

void log_assertion(bool condition, std::string test_name)
{
    condition
        ? logger(success, test_name)
        : logger(error, test_name);
}
