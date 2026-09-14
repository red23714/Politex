#ifndef TEST_ERASE_H
#define TEST_ERASE_H

#include IMYSTRING
#include "test_logger.h"

void test_erase()
{
	logger(info, "ERASE TESTS");
	std::string test_name;
	// TEST 1
	test_name = "TEST 1 - ERASE FROM START";
	MyString s1("LongString");
	s1.erase(0, 4);
	log_assertion(s1.size() == 6 && strcmp(s1.c_str(), "String") == 0, test_name);
	// TEST 2
	test_name = "TEST 2 - ERASE FROM END";
	MyString s2("LongString");
	s2.erase(4, 6);
	log_assertion(s2.size() == 4 && strcmp(s2.c_str(), "Long") == 0, test_name);
    // TEST 3
	test_name = "TEST 3 - ERASE BY OUT OF BOUND INDEX";
	MyString s3("LongString");
    try {
        s2.erase(12, 6);
    }
	catch (...) {}
	logger(success, test_name);
}

#endif // !TEST_ERASE_H
