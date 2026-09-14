#ifndef TEST_APPEND_H
#define TEST_APPEND_H

#include IMYSTRING
#include "test_logger.h"
#include <string>

void test_append()
{
	logger(info, "APPEND TESTS");
	std::string test_name;
	// TEST 1
	test_name = "TEST 1 - APPEND COUNT CHARS FROM C STR";
	MyString s1("str1");
	s1.append(3, 'a');
	log_assertion(s1.size() == 7 && strcmp(s1.c_str(), "str1aaa") == 0, test_name);
	// TEST 2
	test_name = "TEST 2 - APPEND FROM C STR";
	MyString s2("str2");
	s2.append("str1");
	log_assertion(s2.size() == 8 && strcmp(s2.c_str(), "str2str1") == 0, test_name);
	// TEST 3
	test_name = "TEST 3 - APPEND COUNT OF CHAR";
	MyString s3("str3");
	s3.append(5, 'a');
	log_assertion(s3.size() == 4 + 5 && strcmp(s3.c_str(), "str3aaaaa") == 0, test_name);
	// TEST 4
	test_name = "TEST 4 - APPEND STD STRING";
	std::string stdstr = "stdstr";
	MyString s4("str4");
	s4.append(stdstr);
	log_assertion(s4.size() == 10 && strcmp(s4.c_str(), "str4stdstr") == 0, test_name);
}

#endif // !TEST_APPEND_H
