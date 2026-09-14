#ifndef TEST_FIND_H
#define TEST_FIND_H

#include IMYSTRING
#include "test_logger.h"


void test_find()
{
	logger(info, "FIND TESTS");
	std::string test_name;

    MyString str = "Hello, world! Hello, world!";

	// TEST 1
	test_name = "TEST 1 - FIND CONST CHAR* SUBSTR";
    std::size_t pos1 = str.find("Hello");
	log_assertion(pos1 == 0, test_name);
    // TEST 2
	test_name = "TEST 2 - FIND CONST CHAR* SUBSTR BY INDEX";
    std::size_t pos2 = str.find("world", 9);
	log_assertion(pos2 == 21, test_name);
    // TEST 3
	test_name = "TEST 3 - FIND STD::STRING SUBSTR";
    std::size_t pos3 = str.find(std::string("Hello"));
	log_assertion(pos3 == 0, test_name);
    // TEST 4
	test_name = "TEST 4 - FIND STD::STRING SUBSTR BY INDEX";
    std::size_t pos4 = str.find(std::string("world"), 9);
	log_assertion(pos4 == 21, test_name);
    // TEST 5
	test_name = "TEST 5 - FIND STD::STRING SUBSTR BY OUT OF BOUND INDEX";
    try {
        std::size_t pos5 = str.find(std::string("world"), 35);
    }
    catch(...){};
	logger(success, test_name);
}

#endif // !TEST_FIND_H
