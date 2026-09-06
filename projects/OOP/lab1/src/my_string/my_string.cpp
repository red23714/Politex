#include "my_string.h"
#include <cstring>
#include <string_view>

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

char* MyString::create_copy_of_pstr(int new_size)
{
	char* tmp = new char[new_size];
	std::memcpy(tmp, pstr_, capacity_);

	return tmp;
}

void MyString::delete_pstr_change_params(char* new_pstr, int new_len,
										 int new_capacity)
{
	delete[] pstr_;
	len_ = new_len;
	capacity_ = new_capacity;
	pstr_ = new_pstr;
}

void MyString::my_insert(int index, int count, const char* data)
{
	if (capacity_ == 0)
	{
		pstr_ = new char[1];
		pstr_[0] = '\0';
		len_ = 0;
		capacity_ = 1;
	}

	int new_capacity = capacity_ + count;
	char* tmp = create_copy_of_pstr(new_capacity);
	std::memmove(tmp + index + count, tmp + index, capacity_ - index);
	std::memcpy(tmp + index, data, count);

	delete_pstr_change_params(tmp, len_ + count, new_capacity);
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

	std::memset(pstr_, ch, count);
}

MyString::MyString(const MyString& other) { init(std::string_view(other)); }

MyString::~MyString() { delete[] pstr_; }

void MyString::clear()
{
	std::memset(pstr_, 0, capacity_);
	len_ = 0;
}

void MyString::shrink_to_fit() {}

void MyString::operator=(std::string_view source_str)
{
	delete_pstr_change_params(nullptr, 0, 0);
	init(source_str);
}

void MyString::operator=(char ch)
{
	delete_pstr_change_params(new char[2], 1, 2);

	pstr_[0] = ch;
	pstr_[1] = '\0';
}

MyString& MyString::operator=(const MyString& other)
{
	if (this != &other)
	{
		delete_pstr_change_params(nullptr, 0, 0);
		init(std::string_view(other));
	}
	return *this;
}

char* MyString::c_str() { return pstr_; }

int MyString::size() { return len_; }

int MyString::capacity() { return capacity_; }

bool MyString::empty() { return len_ == 0; }

void MyString::insert(int index, int count, char ch)
{
	char* data = new char[count];

	std::memset(data, ch, count);
	my_insert(index, count, data);

	delete[] data;
}

void MyString::insert(int index, std::string_view source_str)
{
	my_insert(index, source_str.size(), source_str.data());
}

void MyString::insert(int index, std::string_view source_str, int count)
{
	my_insert(index, count, source_str.data());
}

void MyString::insert(int index, std::string_view source_str, int s_index,
					  int count)
{
	std::string_view new_source_str = source_str.substr(s_index, count);
	my_insert(index, count, new_source_str.data());
}

void MyString::append(int count, char ch) { insert(len_, count, ch); }

void MyString::append(std::string_view source_str) { insert(len_, source_str); }

void MyString::append(std::string_view source_str, int count)
{
	insert(len_, source_str, count);
}

void MyString::append(std::string_view source_str, int s_index, int count)
{
	insert(len_, source_str, s_index, count);
}

void MyString::erase(int index, int count)
{
	std::memset(pstr_ + index, 0, count);
	std::memmove(
		pstr_ + index, pstr_ + index + count,
		capacity_ - index -
			count); // Move the other part of string that didnt erase to start.
					// dest is on index where start to ease, src is the place
					// where erase ends, count is whole len - part that erased
					// and where index starts
	len_ = len_ - count;
}

MyString::operator std::string_view() const
{
	return std::string_view(pstr_, len_);
}
