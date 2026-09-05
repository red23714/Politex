#ifndef _MY_STRING_H_
#define _MY_STRING_H_

#include <string_view>
#include <cstring>

#define DEBUG 1

class MyString
{
  public:
	// Constructors
	MyString();
	MyString(std::string_view source_str);
	MyString(std::string_view source_str, int count);
	MyString(int count, char ch);

	~MyString();

	// clear str content
	void clear();

	// Clear not used ram
	void shrink_to_fit();

	// Equal operators
	void operator=(std::string_view source_str);
	void operator=(char ch);

	// Gettes
	char* c_str();
	int size();
	int capacity();
	bool empty();

	// Insert by index
	void insert(int index, int count, char ch);
	void insert(int index, std::string_view source_str);
	void insert(int index, std::string_view source_str, int count);
	void insert(int index, std::string_view source_str, int s_index, int count);

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
	MyString* substr(int index);
	MyString* substr(int index, int count);

	// Sum operators
	MyString* operator+(std::string_view source_str);
	MyString* operator+=(std::string_view source_str);

	// Index operator
	char* operator[](int index);

	// Operator and methods to compare strings
	short compare(MyString&);
	bool operator>(MyString& other);
	bool operator<(MyString& other);
	bool operator>=(MyString& other);
	bool operator<=(MyString& other);
	bool operator!=(MyString& other);
	bool operator==(MyString& other);

	// Finders
	int find(std::string_view source_str);
	int find(std::string_view source_str, int index);

	operator std::string_view() const;

#ifdef DEBUG
	void pstr();
#endif

  private:
	char* pstr_ = nullptr;
	int len_;
	int capacity_;

	void init(std::string_view sv);
};

#endif // _MY_STRING_H_
