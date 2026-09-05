#include "my_string.h"
#include <cstring>

#ifdef DEBUG
#include <iostream>
#endif

void MyString::init(std::string_view sv)
{
	len_ = sv.size();
	capacity_ = len_ + 1;
	pstr_ = new char[capacity_];
	std::memcpy(pstr_, sv.data(), len_);
	pstr_[len_] = '\0';
}

#ifdef DEBUG
void MyString::pstr()
{
	std::cout << "\"";
	for (int i = 0; i < len_; ++i)
	{
		std::cout << pstr_[i];
	}
	std::cout << "\"";
	std::cout << " " << "(" << len_ << ", " << capacity_ << ")" << std::endl;
}
#endif

MyString::MyString() : len_(0), capacity_(0) {}

MyString::MyString(std::string_view source_str) { init(source_str); }

MyString::MyString(std::string_view source_str, int count)
{
	init(source_str.substr(0, count)); // Create substring of original string
									   // that starts at 0 and ends in count
}

MyString::MyString(int count, char ch)
{
	len_ = count;
	capacity_ = len_ + 1;
	pstr_ = new char[capacity_];

	for (int i = 0; i < count; ++i)
	{
		pstr_[i] = ch;
	}
}

MyString::~MyString() { delete[] pstr_; }

void MyString::clear()
{
	std::memset(pstr_, 0, capacity_);
	len_ = 0;
}

void MyString::shrink_to_fit() {}

void MyString::operator=(std::string_view source_str)
{
	delete[] pstr_;
	len_ = 0;
	capacity_ = 0;
	init(source_str);
}

void MyString::operator=(char ch)
{
	delete[] pstr_;
	len_ = 1;
	capacity_ = len_ + 1;
	pstr_ = new char[capacity_];
	pstr_[0] = ch;
	pstr_[1] = '\0';
}

char* MyString::c_str()
{
	char* tmp = new char[capacity_];
	std::memcpy(tmp, pstr_, capacity_);

	return tmp;
}

int MyString::size() { return len_; }

int MyString::capacity() { return capacity_; }

bool MyString::empty() { return len_ == 0; }

MyString::operator std::string_view() const
{
	return std::string_view(pstr_, len_);
}
