#ifndef TEST_SUBSTR_H
#define TEST_SUBSTR_H

#include IMYSTRING
#include "test_logger.h"

void test_substr()
{
	logger(info, "SUBSTRING TESTS");
	std::string test_name;
	// TEST 1
	test_name = "TEST 1 - SUBSTRING FULL SIZE";
	MyString s1("Str1 Str2 Str3");
	auto s1sub = s1.substr(5);
	log_assertion(s1sub.size() == 9 && strcmp(s1sub.c_str(), "Str2 Str3") == 0, test_name);
	// TEST 2
	test_name = "TEST 2 - SUBSTRING OF LENGTH";
	MyString s2("Str1 Str2 Str3");
	auto s2sub = s2.substr(5, 4);
	log_assertion(s2sub.size() == 4 && strcmp(s2sub.c_str(), "Str2") == 0, test_name);
}

#endif // !TEST_SUBSTR_H
