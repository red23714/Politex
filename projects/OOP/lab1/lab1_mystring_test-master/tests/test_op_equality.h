#ifndef TEST_OP_EQUALITY_H
#define TEST_OP_EQUALITY_H

#include IMYSTRING
#include "test_logger.h"

void test_op_equality()
{
    logger(info, "OPERATOR= TESTS");
	std::string test_name;

    MyString str1("apple");
    MyString str2("banana");
    MyString str3("apple");
    
    // TEST 1
	test_name = "TEST 1 - OPERATOR== EQUAL";
    log_assertion(str1 == str3, test_name);
    // TEST 2
	test_name = "TEST 2 - OPERATOR== NOT EQUAL";
    log_assertion(!(str1 == str2), test_name);
    // TEST 3
	test_name = "TEST 3 - OPERATOR!= NOT EQUAL";
    log_assertion(str1 != str2, test_name);
    // TEST 4
	test_name = "TEST 4 - OPERATOR!= EQUAL";
    log_assertion(!(str1 != str3), test_name);
    // TEST 5
	test_name = "TEST 5 - OPERATOR<";
    log_assertion(str1 < str2, test_name);
    // TEST 6
	test_name = "TEST 6 - OPERATOR<=";
    log_assertion(str1 <= str3, test_name);
    // TEST 7
	test_name = "TEST 7 - OPERATOR<=";
    log_assertion(str2 > str1, test_name);
    // TEST 8
	test_name = "TEST 8 - OPERATOR>=";
    log_assertion(str2 >= str1, test_name);
}

#endif // !TEST_OP_EQUALITY_H
