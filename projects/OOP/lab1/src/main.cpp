#include "my_string/my_string.h"
#include <string>
#include <iostream>

int main()
{
	MyString str;
	str.pstr();

	MyString str2("Hello world!");
	str2.pstr();

	std::string s_str = "hello";
	MyString str3(s_str);
	str3.pstr();

	MyString s_str1("hello");
	MyString str4(s_str1);
	str4.pstr();

	MyString str5("hello", 4);
	str5.pstr();

	MyString str6(std::string("hello"), 4);
	str6.pstr();

	MyString str7(MyString("hello"), 4);
	str7.pstr();

	MyString str8(5, '!');
	str8.pstr();

	MyString str_clear("Hello world!");
	str_clear.pstr();
	str_clear.clear();
	str_clear.pstr();

	MyString str_eq;
	str_eq = "hello";
	str_eq.pstr();
	str_eq = std::string("hello std::string");
	str_eq.pstr();
	MyString str_eq_tmp("hello MyString");
	str_eq = str_eq_tmp;
	str_eq.pstr();

	MyString str_eq_char;
	str_eq_char = '!';
	str_eq_char.pstr();

	MyString str_cstr("str");
	std::cout << str_cstr.c_str() << std::endl;

	std::cout << str2.size() << std::endl;

	std::cout << str2.capacity() << std::endl;

	std::cout << str2.empty() << std::endl;
	std::cout << str.empty() << std::endl;

	MyString str_insert("aaaaa");
	str_insert.insert(0, 1, '!');
	str_insert.pstr();
	str_insert.insert(3, 2, '@');
	str_insert.pstr();

	str_insert = "aaaaa";
	str_insert.insert(1, "@@@@@");
	str_insert.pstr();

	str_insert = "aaaaa";
	str_insert.insert(1, std::string("@@@@@"));
	str_insert.pstr();

	str_insert = "aaaaa";
	MyString tmp("@@@@@");
	str_insert.insert(1, tmp);
	str_insert.pstr();

	str_insert = "aaaaa";
	str_insert.insert(1, "@@@@@", 2);
	str_insert.pstr();

	str_insert = "aaaaa";
	str_insert.insert(1, "abcde", 1, 2);
	str_insert.pstr();

	MyString str_append;
	str_append.append(3, '!');
	str_append.pstr();
	std::cout << str_append.c_str() << std::endl;
	str_append.append(3, '@');
	str_append.pstr();

	str2 = "Hello world!";
	str2.erase(5, 6);
	str2.pstr();

	return 0;
}
