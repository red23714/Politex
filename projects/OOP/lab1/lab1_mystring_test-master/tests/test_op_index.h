#ifndef TEST_OP_INDEX_H
#define TEST_OP_INDEX_H

#include IMYSTRING
#include "test_logger.h"

void test_op_index()
{
    logger(info, "OPERATOR[] TESTS");
	std::string test_name;
	// TEST 1
	test_name = "TEST 1 - GET FIRST ELEMENT BY INDEX";
	MyString str("Hello, world!");
	log_assertion(str[0] == 'H', test_name);
    // TEST 2
    test_name = "TEST 2 - GET MIDDLE ELEMENT BY INDEX";
    log_assertion(str[7] == 'w', test_name);
    // TEST 3
    test_name = "TEST 3 - GET LAST ELEMENT BY INDEX";
    log_assertion(str[str.size() - 1] == '!', test_name);
    // TEST 4
    test_name = "TEST 4 - GET ELEMENT BY OUT BOUND INDEX";
    try {
        char ch = str[str.size()];
    } catch (...) {}
    logger(success, test_name);
    // TEST 5
    test_name = "TEST 5 - WRITE ELEMENT BY INDEX";
    str[0] = 'h';
    log_assertion(str[0] == 'h', test_name);
    // TEST 6
    test_name = "TEST 6 - WRITE ELEMENT BY OUT BOUND INDEX";
    try {
        str[str.size()] = 'X';
    } catch (...) {}
    logger(success, test_name);
}

#endif // !TEST_OP_INDEX_H
