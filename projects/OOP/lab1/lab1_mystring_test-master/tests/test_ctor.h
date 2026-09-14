#ifndef TEST_CTOR_H
#define TEST_CTOR_H

#include IMYSTRING
#include "test_logger.h"
#include <string>

using namespace std;

void test_constructors() {
    logger(info, "CONSTRUCTORS TESTING");
    string test_name;
    //-----------------------------|
    // TEST 1: DEFAULT CONSTRUCTOR |
    //-----------------------------|
    MyString str1;
    test_name = "TEST 1 - DEFAULT";
    str1.size() == 0 ? logger(success, test_name) : logger(error, test_name);
    //--------------------------|
    // TEST 2: CHAR CONSTRUCTOR |
    //--------------------------|
    test_name = "TEST 2 - CHAR";
    MyString str2(10, 'a');
    str2.size() == 10 && str2.capacity() >= 11 && \
        !strcmp(str2.c_str(), "aaaaaaaaaa") ? logger(success, test_name) : logger(error, test_name);
    //--------------------------|
    // TEST 3: COPY CONSTRUCTOR |
    //--------------------------|
    test_name = "TEST 3 - COPY";
    MyString str3(str2);
    str2.size() == 10 && str2.capacity() >= 11 && !strcmp(str2.c_str(), "aaaaaaaaaa") && \
        str3.size() == 10 && str3.capacity() >= 11 && !strcmp(str3.c_str(), "aaaaaaaaaa") \
        ? logger(success, test_name) : logger(error, test_name);
    //-------------------------------------------|
    // TEST 4: CHAR ARRAY WITH COUNT CONSTRUCTOR |
    //-------------------------------------------|
    test_name = "TEST 4 - CHAR ARRAY WITH COUNT";
    MyString str4("0123456789", 5);
    str4.size() == 5 && str4.capacity() >= 6 && !strcmp(str4.c_str(), "01234") \
        ? logger(success, test_name) : logger(error, test_name);
    //--------------------------------|
    // TEST 5: CHAR ARRAY CONSTRUCTOR |
    //--------------------------------|
    test_name = "TEST 5 - CHAR ARRAY";
    MyString str5("0123456789");
    str5.size() == 10 && str5.capacity() >= 11 && !strcmp(str5.c_str(), "0123456789") \
        ? logger(success, test_name) : logger(error, test_name);
    //---------------------------------|
    // TEST 6: STD::STRING CONSTRUCTOR |
    //---------------------------------|
    test_name = "TEST 5 - STD::STRING";
    std::string std_str1("0123456789");
    MyString str6(std_str1);
    str6.size() == 10 && str6.capacity() >= 11 && !strcmp(str6.c_str(), "0123456789") \
        ? logger(success, test_name) : logger(error, test_name);
}

#endif
