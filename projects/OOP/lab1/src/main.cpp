#include "my_string/my_string.h"
#include <string>
#include <iostream>
#include <cassert>

void print_result(const std::string& test_name, bool passed)
{
	std::cout << (passed ? "[OK]   " : "[FAIL] ") << test_name << std::endl;
}

int main()
{
	MyString str_t1(1000, 'a');
	MyString str_t2("1234567890");
	str_t1.pstr();
	str_t1 = str_t2;
	str_t1.insert(3, "****", 3);
	str_t1.pstr();

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

	MyString str_shrink("Hello world!");
	str_shrink.pstr();
	str_shrink.erase(5, 6);
	str_shrink.pstr();
	str_shrink.shrink_to_fit();
	str_shrink.pstr();

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

	MyString str_repl("hello amazing world");
	str_repl.replace(6, 7, "wonderful");
	str_repl.pstr();

	str_repl = "hello amazing world";
	str_repl.replace(6, 7, "wonderful", 6);
	str_repl.pstr();

	str_repl = "hello amazing world";
	str_repl.replace(6, 7, "wonderful", 1, 2);
	str_repl.pstr();

	MyString str_sub("hello amazing world"), substr;
	substr = str_sub.substr(6);
	substr.pstr();

	str_sub = "hello amazing world";
	substr = str_sub.substr(6, 7);
	substr.pstr();

	MyString left("hel"), right("lo"), r;
	r = left + right;
	left.pstr();
	right.pstr();
	r.pstr();

	left += right;
	left.pstr();
	right.pstr();

	MyString str_brac("hello");
	std::cout << str_brac[2] << std::endl;
	str_brac[2] = 'L';
	str_brac.pstr();

	MyString a("abcd"), b("abce");
	std::cout << a.compare(b) << b.compare(a) << std::endl;
	std::cout << (a == b) << (a != b) << (a > b) << (a >= b) << (a < b)
			  << (a <= b) << std::endl;

	MyString str_find = std::string("hello");
	str_find = "hello amazing world";
	std::cout << str_find.find("amazing") << std::endl;

	MyString str_num(0x12345678);
	str_num.pstr();

	MyString str_float(0.05f);
	str_float.pstr();

	str_num = std::move(str_float);
	str_num.pstr();

	std::cout << str_find.at(0) << std::endl;

	MyString str_tofloat(0.05f);
	MyString str_toint(12345678);
	std::cout << str_tofloat.to_float() << std::endl;
	std::cout << str_toint.to_int() << std::endl;

	try
	{
		std::cout << str_find.to_float() << std::endl;
	}
	catch (const MyString::WrongTransformException)
	{
		std::cout << "work exception to wrong transform" << std::endl;
	}

	std::cout << "=== ТЕСТЫ ИТЕРАТОРОВ ===" << std::endl;

	// ---------- 1-2. begin() / end() ----------
	{
		MyString s("Hello");
		std::string result;
		for (MyString::iterator it = s.begin(); it != s.end(); ++it)
			result += *it;

		std::cout << "begin()/end() обход: \"" << result
				  << "\" (ожидали \"Hello\")" << std::endl;
		print_result("begin()/end()", result == "Hello");

		// Проверка модификации через iterator
		for (MyString::iterator it = s.begin(); it != s.end(); ++it)
			*it = std::toupper(*it);
		std::cout << "После модификации: \"" << s.c_str()
				  << "\" (ожидали \"HELLO\")" << std::endl;
		print_result("iterator modification",
					 std::string(s.c_str()) == "HELLO");
	}

	// ---------- 3-4. cbegin() / cend() ----------
	{
		const MyString s("World");
		std::string result;
		for (MyString::const_iterator it = s.cbegin(); it != s.cend(); ++it)
			result += *it;

		std::cout << "cbegin()/cend() обход: \"" << result
				  << "\" (ожидали \"World\")" << std::endl;
		print_result("cbegin()/cend()", result == "World");
		// *it = 'X'; // <-- должно НЕ компилироваться, если раскомментировать
	}

	// ---------- 5-6. rbegin() / rend() ----------
	{
		MyString s("Hello");
		std::string result;
		for (MyString::reverse_iterator it = s.rbegin(); it != s.rend(); ++it)
			result += *it;

		std::cout << "rbegin()/rend() обход: \"" << result
				  << "\" (ожидали \"olleH\")" << std::endl;
		print_result("rbegin()/rend()", result == "olleH");

		// Проверка модификации через reverse_iterator
		MyString s2("abc");
		for (MyString::reverse_iterator it = s2.rbegin(); it != s2.rend(); ++it)
			*it = std::toupper(*it);
		std::cout << "После модификации reverse: \"" << s2.c_str()
				  << "\" (ожидали \"ABC\")" << std::endl;
		print_result("reverse_iterator modification",
					 std::string(s2.c_str()) == "ABC");
	}

	// ---------- 7-8. rcbegin() / rcend() ----------
	{
		const MyString s("Hello");
		std::string result;
		for (MyString::const_reverse_iterator it = s.rcbegin(); it != s.rcend();
			 ++it)
			result += *it;

		std::cout << "rcbegin()/rcend() обход: \"" << result
				  << "\" (ожидали \"olleH\")" << std::endl;
		print_result("rcbegin()/rcend()", result == "olleH");
		// *it = 'X'; // <-- должно НЕ компилироваться, если раскомментировать
	}

	// ---------- Проверка на пустой строке (граничный случай) ----------
	{
		MyString s;
		int count = 0;
		for (MyString::iterator it = s.begin(); it != s.end(); ++it)
			++count;
		std::cout << "begin()/end() на пустой строке, итераций: " << count
				  << " (ожидали 0)" << std::endl;
		print_result("empty string forward", count == 0);
	}

	// ---------- insert/erase/replace/at через iterator ----------
	{
		MyString s("Hello");
		MyString::iterator it = s.begin();
		++it;
		++it; // указывает на второй 'l' (индекс 2)
		s.insert(it, 3, 'X');
		std::cout << "insert(iterator, 3, 'X') в \"Hello\" на индекс 2: \""
				  << s.c_str() << "\" (ожидали \"HeXXXllo\")" << std::endl;
		print_result("insert(iterator)", std::string(s.c_str()) == "HeXXXllo");
	}
	{
		MyString s("Hello");
		MyString::iterator it = s.begin();
		++it; // индекс 1 ('e')
		s.erase(it, 2);
		std::cout << "erase(iterator, 2) из \"Hello\" на индекс 1: \""
				  << s.c_str() << "\" (ожидали \"Hlo\")" << std::endl;
		print_result("erase(iterator)", std::string(s.c_str()) == "Hlo");
	}
	{
		MyString s("Hello");
		MyString::iterator it = s.begin();
		char c = s.at(it);
		std::cout << "at(begin()) = '" << c << "' (ожидали 'H')" << std::endl;
		print_result("at(iterator)", c == 'H');
	}
	{
		MyString s("Hello World");
		MyString::const_iterator it = s.cbegin();
		for (int i = 0; i < 6; ++i)
			++it; // индекс 6 -> "World"
		MyString sub = s.substr(it);
		std::cout << "substr(iterator) от индекса 6: \"" << sub.c_str()
				  << "\" (ожидали \"World\")" << std::endl;
		print_result("substr(const_iterator)",
					 std::string(sub.c_str()) == "World");
	}

	std::cout << std::endl << "=== ОСТАЛЬНЫЕ ФУНКЦИИ ===" << std::endl;

	// ---------- MyString(MyString&&) — move-конструктор ----------
	{
		MyString original("MoveMe");
		const char* original_ptr = original.c_str();

		MyString moved(std::move(original));

		std::cout << "Move-конструктор: moved = \"" << moved.c_str()
				  << "\" (ожидали \"MoveMe\")" << std::endl;
		print_result("move constructor value",
					 std::string(moved.c_str()) == "MoveMe");

		std::cout << "После move, original.size() = " << original.size()
				  << " (ожидали 0)" << std::endl;
		print_result("move constructor leaves source empty",
					 original.size() == 0);
	}

	// ---------- MyString(int32_t) — число в строку ----------
	{
		MyString s(12345);
		std::cout << "MyString(12345) = \"" << s.c_str()
				  << "\" (ожидали \"12345\")" << std::endl;
		print_result("int constructor positive",
					 std::string(s.c_str()) == "12345");

		MyString s_neg(-42);
		std::cout << "MyString(-42) = \"" << s_neg.c_str()
				  << "\" (ожидали \"-42\")" << std::endl;
		print_result("int constructor negative",
					 std::string(s_neg.c_str()) == "-42");
	}

	// ---------- MyString(float) — float в строку ----------
	{
		MyString s(3.14f);
		std::cout << "MyString(3.14f) = \"" << s.c_str()
				  << "\" (ожидали что-то вроде \"3.14...\")" << std::endl;
		// Точное сравнение зависит от реализации округления, поэтому просто
		// выводим
	}
	{
		MyString s(0.05f);
		std::cout << "MyString(0.05f) = \"" << s.c_str()
				  << "\" (ожидали что-то вроде \"0.05...\")" << std::endl;
	}

	// ---------- operator=(MyString&&) — move-присваивание ----------
	{
		MyString a("First");
		MyString b("Second");
		b = std::move(a);

		std::cout << "После move-присваивания b = \"" << b.c_str()
				  << "\" (ожидали \"First\")" << std::endl;
		print_result("move assignment value",
					 std::string(b.c_str()) == "First");

		std::cout << "После move-присваивания a.size() = " << a.size()
				  << " (ожидали 0)" << std::endl;
		print_result("move assignment leaves source empty", a.size() == 0);
	}

	// ---------- at(index) ----------
	{
		MyString s("Test");
		char c = s.at(1);
		std::cout << "at(1) для \"Test\" = '" << c << "' (ожидали 'e')"
				  << std::endl;
		print_result("at(index) valid", c == 'e');

		bool threw = false;
		try
		{
			s.at(100); // индекс за пределами строки
		}
		catch (const std::exception&)
		{
			threw = true;
		}
		std::cout << "at(100) выбросил исключение: " << (threw ? "да" : "нет")
				  << " (ожидали да)" << std::endl;
		print_result("at(index) throws on invalid index", threw);
	}

	// ---------- to_int() ----------
	{
		MyString s("123");
		int result = s.to_int();
		std::cout << "to_int() для \"123\" = " << result << " (ожидали 123)"
				  << std::endl;
		print_result("to_int() positive", result == 123);
	}

	// ---------- to_float() ----------
	{
		MyString s("3.14");
		float result = s.to_float();
		std::cout << "to_float() для \"3.14\" = " << result
				  << " (ожидали ~3.14)" << std::endl;
		print_result("to_float()", std::abs(result - 3.14f) < 0.01f);
	}

	std::cout << std::endl << "=== ТЕСТЫ ЗАВЕРШЕНЫ ===" << std::endl;
	return 0;
}
