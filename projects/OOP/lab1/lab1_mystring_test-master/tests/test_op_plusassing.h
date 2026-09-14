#ifndef TEST_OP_PLUSASSIGN_H
#define TEST_OP_PLUSASSIGN_H

#include IMYSTRING
#include "test_logger.h"

void test_op_plusassign()
{
    logger(info, "OPERATOR+= TESTS");
	std::string test_name;

    MyString str1("Hello");
    std::string std_str1 = "World";
    const char* cstr = "!!!";
    char exclamation = '!';

	// TEST 1
	test_name = "TEST 1 - MYSTRING+=STD::STRING";
    str1 += " ";
    str1 += std_str1;
    log_assertion(strcmp(str1.c_str(), "Hello World") == 0, test_name);
    // TEST 2
	test_name = "TEST 2 - MYSTRING+=CONST CHAR*";
    str1 += cstr;
    log_assertion(strcmp(str1.c_str(), "Hello World!!!") == 0, test_name);
}

#endif // !TEST_OP_PLUSASSIGN_H
