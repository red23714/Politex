#ifndef TEST_REPLACE_H
#define TEST_REPLACE_H

#include IMYSTRING
#include "test_logger.h"

void test_replace()
{
	logger(info, "REPLACE TESTS");
	std::string test_name;
	// TEST 1
	test_name = "TEST 1 - REPLACE FROM START";
	MyString s1("LongString");
	s1.replace(0, 4, "Short");
	log_assertion(s1.size() == 11 && strcmp(s1.c_str(), "ShortString") == 0, test_name);
	// TEST 2
	test_name = "TEST 2 - REPLACE FROM END";
	MyString s2("LongString");
	s2.replace(4, 6, "Int");
	log_assertion(s2.size() == 7 
		&& s2.capacity() >= 11
		&& strcmp(s2.c_str(), "LongInt") == 0, test_name);
}

#endif // !TEST_REPLACE_H
