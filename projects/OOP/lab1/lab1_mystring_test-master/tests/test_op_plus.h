#ifndef TEST_OP_PLUS_H
#define TEST_OP_PLUS_H

#include IMYSTRING
#include "test_logger.h"

void test_op_plus()
{
    logger(info, "OPERATOR+ TESTS");
	std::string test_name;

    MyString str1("Hello");
    MyString str2("World");
    std::string std_str1 = "!!!";
    const char* cstr = "!!!";
    char exclamation = '!';

	// TEST 1
	test_name = "TEST 1 - MYSTRING + MYSTRING";
    MyString result1 = str1 + " " + str2;
    log_assertion(strcmp(str1.c_str(), "Hello World") != 0 && strcmp(result1.c_str(), "Hello World") == 0, test_name);
    str1 = "Hello";
    // TEST 2
	test_name = "TEST 2 - MYSTRING + CONST CHAR*";
    MyString result2 = str1 + cstr;
    log_assertion(strcmp(str1.c_str(), "Hello!!!") != 0 && strcmp(result2.c_str(), "Hello!!!") == 0, test_name);
    str1 = "Hello";
    // TEST 3
	test_name = "TEST 3 - MYSTRING + STD::STRING";
    MyString result3 = str1 + " " + std_str1;
    log_assertion(strcmp(str1.c_str(), "Hello !!!") != 0 && strcmp(result3.c_str(), "Hello !!!") == 0, test_name);
}

#endif // !TEST_OP_PLUS_H
