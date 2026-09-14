#ifndef TEST_INSERT_H
#define TEST_INSERT_H

#include IMYSTRING
#include "test_logger.h"
#include <string>

using namespace std;

void test_insert_methods() {
    logger(info, "INSERT METHODS TESTING");
    string test_name;
    //--------------------------------|
    // TEST 1: INSERT IN EMPTY STRING |
    //--------------------------------|
    test_name = "TEST 1 - INSERT IN EMPTY STRING";
    MyString str1("");
    str1.insert(0, "Hello", 5);
    !strcmp(str1.c_str(), "Hello") ? logger(success, test_name) : logger(error, test_name);
    //-----------------------------------|
    // TEST 2: INSERT IN BEGIN OF STRING |
    //-----------------------------------|
    test_name = "TEST 2 - INSERT IN BEGIN OF STRING";
    MyString str2(" World");
    str2.insert(0, "Hello", 5);
    !strcmp(str2.c_str(), "Hello World") ? logger(success, test_name) : logger(error, test_name);
    //------------------------------------|
    // TEST 3: INSERT IN MIDDLE OF STRING |
    //------------------------------------|
    test_name = "TEST 3 - INSERT IN MIDDLE OF STRING";
    MyString str3("Hello World");
    str3.insert(6, "Beautiful ", 10);
    !strcmp(str3.c_str(), "Hello Beautiful World") ? logger(success, test_name) : logger(error, test_name);
    //---------------------------------|
    // TEST 4  INSERT IN END OF STRING |
    //---------------------------------|
    test_name = "TEST 4 - INSERT IN END OF STRING";
    MyString str4("Hello");
    str4.insert(5, " World", 6);
    !strcmp(str4.c_str(), "Hello World") ? logger(success, test_name) : logger(error, test_name);
    //-----------------------------|
    // TEST 5: INSERT EMPTY STRING |
    //-----------------------------|
    test_name = "TEST 5 - INSERT EMPTY STRING";
    MyString str5("Hello World");
    str5.insert(6, "", 0);
    !strcmp(str5.c_str(), "Hello World") ? logger(success, test_name) : logger(error, test_name);
    //---------------------------------------------|
    // TEST 6: INSERT EMPTY STRING IN EMPTY STRING |
    //---------------------------------------------|
    test_name = "TEST 6 - INSERT EMPTY STRING IN EMPTY STRING";
    MyString str6;
    str6.insert(0, "", 0);
    !strcmp(str6.c_str(), "") && str6.size() == 0 ? logger(success, test_name) : logger(error, test_name);
    //----------------------------|
    // TEST 7: INSERT LONG STRING |
    //----------------------------|
    test_name = "TEST 7 - INSERT LONG TAIL REWRITE ON COPY";
    MyString str7("0123456789");
    str7.insert(3, "**");
    !strcmp(str7.c_str(), "012**3456789") ? logger(success, test_name) : logger(error, test_name);
    //---------------------------|
    // TEST 8: INSERT ONE SYMBOL |
    //---------------------------|
    test_name = "TEST 8 - INSERT ONE SYMBOL";
    MyString str8("Hello World");
    str8.insert(5, "X", 1);
    !strcmp(str8.c_str(), "HelloX World") ? logger(success, test_name) : logger(error, test_name);
    //-----------------------------|
    // TEST 9: INSERT A LOT OF 'A' |
    //-----------------------------|
    test_name = "TEST 9 - INSERT A LOT OF \'A\'";
    MyString str9;
    str9.insert(0, 5000, 'a');
    str9.size() == 5000 && str9.capacity() >= 5001 ? logger(success, test_name) : logger(error, test_name);
    //------------------------------------|
    // TEST 10: INSERT STRING INTO ITSELF |
    //------------------------------------|
    test_name = "TEST 10 - INSERT STRING INTO ITSELF";
    MyString str10("Hello");
    str10.insert(5, str10.c_str());
    !strcmp(str10.c_str(), "HelloHello") ? logger(success, test_name) : logger(error, test_name);
    //---------------------------------------|
    // TEST 11: INSERT BY OUT OF RANGE INDEX |
    //---------------------------------------|
    test_name = "TEST 11 - INSERT BY OUT OF RANGE INDEX";
    MyString str11("Hello");
    try {
        str11.insert(1, " World", 6);
    }
    catch (...) {}
    logger(success, test_name); // If this line is executed, test has been successfully passed. Otherwise, the program should crash
}
#endif
