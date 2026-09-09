#include "my_string.h"
#include <cstring>
#include <stdexcept>
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
	if (new_size < capacity_)
		std::memcpy(tmp, pstr_, new_size);
	else
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
	if (index < 0)
		index = capacity_ - index;
	if (index > capacity_ || index < 0)
		throw std::out_of_range("Index is bigger than length of string");
	if (capacity_ == 0)
	{
		pstr_ = new char[1];
		pstr_[0] = '\0';
		len_ = 0;
		capacity_ = 1;
	}

	int new_capacity = len_ + count + 1;
	if (len_ + count < capacity_)
		new_capacity = capacity_;

	char* tmp = create_copy_of_pstr(new_capacity);
	std::memmove(tmp + index + count, tmp + index, len_ + 1 - index);
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

MyString::MyString(const char* source_str)
	: MyString(std::string_view(source_str))
{
}

MyString::MyString(std::string source_str)
	: MyString(std::string_view(source_str))
{
}

MyString::MyString(MyString&& other)
	: pstr_(other.pstr_), len_(other.len_), capacity_(other.capacity_)
{
	other.pstr_ = nullptr;
	other.len_ = 0;
	other.capacity_ = 0;
}

MyString::MyString(int32_t number)
{
	if (number == 0)
	{
		this->insert(0, 1, '0');
	}

	bool is_negative = false;
	int32_t tmp = number;

	if (tmp < 0)
	{
		is_negative = true;
		tmp = -tmp;
	}

	while (tmp > 0)
	{
		char tmp2 = '0' + tmp % 10;
		this->insert(0, 1, tmp2);
		tmp /= 10;
	}

	if (is_negative)
		this->insert(0, 1, '-');
}

MyString::MyString(float number)
{
	if (number < 0)
	{
		this->insert(0, 1, '-');
	}

	int32_t int_part = static_cast<int32_t>(number);
	MyString int_str(int_part);

	for (int i = 0; i < int_str.size(); ++i)
	{
		this->insert(this->size(), 1, int_str[i]);
	}

	float fraction = number - int_part;

	if (fraction > 0.00001f)
	{
		this->insert(this->size(), 1, '.');

		int steps = 0;

		while (steps < 6)
		{
			fraction *= 10.0f;
			int32_t digit = static_cast<int32_t>(fraction);

			char ch = '0' + digit;
			this->insert(this->size(), 1, ch);

			fraction -= digit;
			++steps;

			if (fraction < 0.00001f)
			{
				break;
			}
		}
	}
}

MyString::~MyString() { delete[] pstr_; }

void MyString::clear()
{
	std::memset(pstr_, 0, capacity_);
	len_ = 0;
}

void MyString::shrink_to_fit()
{
	char* tmp = create_copy_of_pstr(len_ + 1);
	delete_pstr_change_params(tmp, len_, len_ + 1);
}

void MyString::operator=(std::string_view source_str)
{
	delete_pstr_change_params(nullptr, 0, 0);
	init(source_str);
}

void MyString::operator=(const char* source_str)
{
	*this = std::string_view(source_str);
}

void MyString::operator=(const std::string& source_str)
{
	*this = std::string_view(source_str);
}

void MyString::operator=(char ch)
{
	delete_pstr_change_params(new char[2], 1, 2);

	pstr_[0] = ch;
	pstr_[1] = '\0';
}

MyString& MyString::operator=(MyString&& other)
{
	if (this != &other)
	{
		delete_pstr_change_params(nullptr, 0, 0);

		pstr_ = other.pstr_;
		len_ = other.len_;
		capacity_ = other.capacity_;

		other.pstr_ = nullptr;
		other.len_ = 0;
		other.capacity_ = 0;
	}

	return *this;
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

const char* MyString::c_str() const { return pstr_; }

int MyString::size() const { return len_; }

int MyString::capacity() const { return capacity_; }

bool MyString::empty() const { return len_ == 0; }

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
	if (index < 0)
		index = capacity_ - index;
	if (index > capacity_ || index < 0)
		throw std::out_of_range("Index is bigger than length of string");

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

void MyString::replace(int index, int count, std::string_view source_str,
					   int s_index, int s_count)
{
	std::string_view replace_str = source_str.substr(s_index, s_count);
	erase(index, count);
	insert(index, replace_str);
}

void MyString::replace(int index, int count, std::string_view source_str)
{
	replace(index, count, source_str, 0, source_str.size());
}

void MyString::replace(int index, int count, std::string_view source_str,
					   int s_count)
{
	replace(index, count, source_str, 0, s_count);
}

MyString MyString::substr(int index, int count) const
{
	if (index < 0)
		index = capacity_ - index;
	if (index > capacity_ || index < 0)
		throw std::out_of_range("Index is bigger than length of string");
	if (count + index > capacity_)
		throw std::out_of_range(
			"Substring count is larger than length of string");

	MyString new_str = *this; // Unname pointer and make copy

	if (index != 0)
		new_str.erase(0, index); // delete start of the string previous to
								 // index were we need substr

	new_str.erase(count, len_ - index -
							 count); // delete other part after substr to end

	new_str.shrink_to_fit();

	return new_str;
}

MyString MyString::substr(int index) const
{
	return substr(index, len_ - index);
}

MyString& MyString::operator+=(std::string_view source_str)
{
	this->append(source_str);
	return *this;
}

MyString MyString::operator+(std::string_view source_str) const
{
	MyString tmp = *this;
	return tmp += source_str;
}

char& MyString::operator[](int index)
{
	if (index < 0)
		index = capacity_ - index;
	if (index > capacity_ || index < 0)
		throw std::out_of_range("Index is bigger than length of string");

	return pstr_[index];
}

const char& MyString::operator[](int index) const { return pstr_[index]; }

short MyString::compare(MyString& other) const
{
	int this_len = this->len_;
	if (this_len < other.len_)
		return -1;
	if (this_len > other.len_)
		return 1;

	for (int i = 0; i < this_len; ++i)
	{
		if (this->pstr_[i] != other.pstr_[i])
		{
			if (this->pstr_[i] - other.pstr_[i] > 0)
				return 1;
			else
				return -1;
		}
	}

	return 0;
}

bool MyString::operator>(MyString& other) const
{
	return this->compare(other) == 1;
}
bool MyString::operator<(MyString& other) const
{
	return this->compare(other) == -1;
}
bool MyString::operator>=(MyString& other) const
{
	return this->compare(other) != -1;
}
bool MyString::operator<=(MyString& other) const
{
	return this->compare(other) != 1;
}
bool MyString::operator!=(MyString& other) const
{
	return this->compare(other) != 0;
}
bool MyString::operator==(MyString& other) const
{
	return this->compare(other) == 0;
}

std::basic_ifstream<char>& operator>>(std::basic_ifstream<char>& is,
									  MyString& str)
{
}

std::basic_ofstream<char>& operator<<(std::basic_ofstream<char>& os,
									  const MyString& str)
{
	os.write(str.c_str(), str.size());
	return os;
}

int MyString::find(std::string_view source_str) const
{
	return find(source_str, 0);
}

int MyString::find(std::string_view source_str, int index) const
{
	if (index < 0)
		index = capacity_ - index;
	if (index > capacity_ || index < 0)
		throw std::out_of_range("Index is bigger than length of string");

	int m = static_cast<int>(source_str.size());

	if (m == 0)
		return -1;

	const char* pattern = source_str.data();

	for (int i = index; i <= len_ - m; ++i)
	{
		int j = 0;
		while (j < m && pstr_[i + j] == pattern[j])
			++j;

		if (j == m)
			return i;
	}

	return -1;
}

char MyString::at(int index)
{
	if (index < 0)
		index = capacity_ - index;
	if (index > capacity_ || index < 0)
		throw std::out_of_range("Index is bigger than length of string");

	return pstr_[index];
}

int MyString::to_int() { return static_cast<int>(this->to_float()); }

float MyString::to_float()
{
	float result = 0.0f;
	float sign = 1.0f;
	int i = 0;

	if (this->pstr_[i] == '-')
	{
		sign = -1.0f;
		i++;
	}
	else if (this->pstr_[i] == '+')
	{
		i++;
	}

	while (i < this->len_ && this->pstr_[i] >= '0' && this->pstr_[i] <= '9')
	{
		result = result * 10.0f + (this->pstr_[i] - '0');
		i++;
	}

	if (i < this->len_ && this->pstr_[i] == '.')
	{
		i++;
		float weight = 0.1f;

		while (i < this->len_ && this->pstr_[i] >= '0' && this->pstr_[i] <= '9')
		{
			result += (this->pstr_[i] - '0') * weight;
			weight /= 10.0f;
			i++;
		}
	}

	return result * sign;
}
MyString::operator std::string_view() const
{
	return std::string_view(pstr_, len_);
}
