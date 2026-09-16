#ifndef _MY_STRING_H_
#define _MY_STRING_H_

#include <exception>
#include <string_view>
#include <cstring>
#include <cstdint>
#include <fstream>
#include <stdexcept>

#define DEBUG 1

class MyStringIterator
{
  public:
	MyStringIterator();
	MyStringIterator(char* ptr);

	char& operator*() const;
	char* operator->() const;

	MyStringIterator& operator++();
	MyStringIterator operator++(int);
	MyStringIterator& operator--();
	MyStringIterator operator--(int);

	MyStringIterator operator+(int n) const;
	MyStringIterator operator-(int n) const;
	int operator-(const MyStringIterator& other) const;

	bool operator==(const MyStringIterator& other) const;
	bool operator!=(const MyStringIterator& other) const;

	char* base() const;

  private:
	char* ptr_;
};

class MyString
{
  public:
	class WrongTransformException : public std::exception
	{
	  public:
		WrongTransformException(char* mesg) : msg(mesg) {}
		const char* what() { return msg; }

	  private:
		char* msg = "Wrong transformation";
	};

	using iterator = char*;
	using const_iterator = const char*;

	class reverse_iterator
	{
	  public:
		explicit reverse_iterator(char* ptr) : ptr_it(ptr) {};

		char& operator*() const { return *ptr_it; }
		char* operator->() const { return ptr_it; }

		reverse_iterator& operator++()
		{
			--ptr_it;
			return *this;
		}
		reverse_iterator operator++(int)
		{
			reverse_iterator tmp = *this;
			--ptr_it;
			return tmp;
		}

		bool operator==(const reverse_iterator& other) const
		{
			return ptr_it == other.ptr_it;
		}
		bool operator!=(const reverse_iterator& other) const
		{
			return ptr_it != other.ptr_it;
		}

	  protected:
		char* ptr_it;
	};

	class const_reverse_iterator : public reverse_iterator
	{
		explicit const_reverse_iterator(const char* ptr)
			: reverse_iterator(const_cast<char*>(ptr))
		{
		}

		const char& operator*() const { return *ptr_it; }
		const char* operator->() const { return ptr_it; }
	};

	// Constructors
	MyString();
	MyString(std::string_view source_str);
	MyString(std::string_view source_str, int count);
	MyString(int count, char ch);

	MyString(MyString&& other); // Move constructor
	MyString(int32_t number);
	MyString(float number);

	MyString(const char* source_str);
	MyString(std::string source_str);
	MyString(const MyString& other);

	~MyString();

	// clear str content
	void clear();

	// Clear not used ram
	void shrink_to_fit();

	// Equal operators
	void operator=(std::string_view source_str);
	void operator=(const char* source_str);
	void operator=(const std::string& source_str);
	void operator=(char ch);

	MyString& operator=(MyString&& other);

	MyString& operator=(const MyString& other);

	// Gettes
	const char* c_str() const;
	int size() const;
	int capacity() const;
	bool empty() const;

	// Insert by index
	void insert(int index, int count, char ch);
	void insert(int index, std::string_view source_str);
	void insert(int index, std::string_view source_str, int count);
	void insert(int index, std::string_view source_str, int s_index, int count);

	void insert(iterator it, int count, char ch);
	void insert(iterator it, std::string_view source_str);
	void insert(iterator it, std::string_view source_str, int count);
	void insert(iterator it, std::string_view source_str, int s_index,
				int count);

	// Append to end of string
	void append(int count, char ch);
	void append(std::string_view source_str);
	void append(std::string_view source_str, int count);
	void append(std::string_view source_str, int s_index, int count);

	// Delete sub string
	void erase(int index, int count);

	// Replace sub string
	void replace(int index, int count, std::string_view source_str);
	void replace(int index, int count, std::string_view source_str,
				 int s_count);
	void replace(int index, int count, std::string_view source_str, int s_index,
				 int s_count);

	// Get sub string
	MyString substr(int index) const;
	MyString substr(int index, int count) const;

	// Sum operators
	MyString operator+(std::string_view source_str) const;
	MyString& operator+=(std::string_view source_str);

	// Index operator
	char& operator[](int index);
	const char& operator[](int index) const;

	// Operator and methods to compare strings
	short compare(const MyString& other) const;
	bool operator>(const MyString& other) const;
	bool operator<(const MyString& other) const;
	bool operator>=(const MyString& other) const;
	bool operator<=(const MyString& other) const;
	bool operator!=(const MyString& other) const;
	bool operator==(const MyString& other) const;

	friend std::basic_ofstream<char>& operator<<(std::basic_ofstream<char>& os,
												 const MyString& str);

	friend std::basic_ifstream<char>& operator>>(std::basic_ifstream<char>& is,
												 MyString& str);

	// Finders
	int find(std::string_view source_str) const;
	int find(std::string_view source_str, int index) const;

	char at(int index);

	int to_int();
	float to_float();

	operator std::string_view() const;

#ifdef DEBUG
	void pstr();
#endif

  private:
	char* pstr_ = nullptr;
	int len_;
	int capacity_;

	void init(std::string_view sv);
	char* create_copy_of_pstr(int new_size) const;
	void delete_pstr_change_params(char* new_pstr, int new_len,
								   int new_capacity);
	void my_insert(int index, int count, const char* data);
	int check_index(int index, int capacity, int count,
					std::string error_msg) const;
};

#endif // _MY_STRING_H_
