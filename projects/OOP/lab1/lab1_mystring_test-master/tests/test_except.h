#ifndef TEST_EXCEPT_H
#define TEST_EXCEPT_H

#include IMYSTRING
#include "test_logger.h"

void test_except() 
{
    logger(info, "EXCEPTIONS TESTS");
    //-----------------------------|
    // TEST 1: INSERT EXCEPTION 1  |
    //-----------------------------|
    std::string test_name = "TEST 1 - EXCEPT INSERT 1";
    MyString str1("abc");
    bool caught = false;
    try
    {
        str1.insert(-1, "some");
    }
    catch (const std::out_of_range& ex)
    {
        caught = true;
    }
    caught ? logger(success, test_name) : logger(error, test_name);

    //-----------------------------|
    // TEST 2: INSERT EXCEPTION 2  |
    //-----------------------------|
    test_name = "TEST 2 - EXCEPT INSERT 2";
    caught = false;
    try
    {
        str1.insert(10000, "some");
    }
    catch (const std::out_of_range& ex)
    {
        caught = true;
    }
    caught ? logger(success, test_name) : logger(error, test_name);

    //-----------------------------|
    // TEST 3: INDEX OPERATOR      |
    //-----------------------------|
    test_name = "TEST 3 - EXCEPT INDEX OPERATOR";
    MyString str2("abc");
    caught = false;
    try
    {
        str2[24];
    }
    catch (const std::out_of_range& ex)
    {
        caught = true;
    }
    caught ? logger(success, test_name) : logger(error, test_name);
}

#endif
