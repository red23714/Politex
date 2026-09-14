#ifndef TEST_OP_ASSIGN_H
#define TEST_OP_ASSIGN_H

#include IMYSTRING
#include "test_logger.h"

void test_op_assign()
{
    logger(info, "OPERATOR= TESTS");
	std::string test_name;
	// TEST 1
	test_name = "TEST 1 - ASSIGN CONST CHAR*";
	MyString s1("Hello");
	s1 = "1234567890";
	log_assertion(s1.size() == 10 && strcmp(s1.c_str(), "1234567890") == 0, test_name);
    // TEST 2
	test_name = "TEST 2 - ASSIGN CHAR";
	MyString s2("Hello");
	s2 = 'a';
	log_assertion(s2.size() == 1 && strcmp(s2.c_str(), "a") == 0, test_name);
    // TEST 3
	test_name = "TEST 3 - ASSIGN STD::STRING";
	MyString s3("Hello");
    std::string std_s3("1234567890");
	s3 = std_s3;
	log_assertion(s3.size() == 10 && strcmp(s3.c_str(), std_s3.c_str()) == 0, test_name);
}

#endif // !TEST_OP_ASSIGN_H
