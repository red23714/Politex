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

	return 0;
}
