#ifndef TEST_CAPACITY_H
#define TEST_CAPACITY_H

#include IMYSTRING
#include "test_logger.h"
#include <string>

void test_capacity()
{
    logger(info, "CHANGE CAPACITY TESTS");
    std::string test_name;

    // TEST 1
    test_name = "TEST 1 - 1001 CHAR";
    MyString str1(1000, 'a');
    MyString str2("1234567890");
    str1 = str2;
    str1.insert(3, "****", 3);
    log_assertion(strcmp(str1.c_str(), "123***4567890") == 0 && str1.size() == 13 && str1.capacity() == 1001, test_name);
    // TEST 2
    test_name = "TEST 2 - SHRINK TO FIT";
    str1.shrink_to_fit();
    log_assertion(strcmp(str1.c_str(), "123***4567890") == 0 && str1.size() == 13 && str1.capacity() == 14, test_name);
}

#endif
