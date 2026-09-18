#include "my_string.h"
#include <cstring>
#include <stdexcept>
#include <string_view>

#ifdef DEBUG
#include <iostream>
#endif

class AhoCorasickNode
{
  public:
	static const int kAlphabetSize = 256;

	AhoCorasickNode()
	{
		for (int ch = 0; ch < kAlphabetSize; ++ch)
		{
			children_[ch] = -1;
			next_[ch] = -1;
		}
	}

	int child(int ch) const { return children_[ch]; }
	void set_child(int ch, int node) { children_[ch] = node; }

	int cached_transition(int ch) const { return next_[ch]; }
	void cache_transition(int ch, int node) { next_[ch] = node; }

	int fail() const { return fail_; }
	void set_fail(int node) { fail_ = node; }

	bool is_terminal() const { return is_terminal_; }
	void mark_terminal() { is_terminal_ = true; }

  private:
	int children_[kAlphabetSize];
	int next_[kAlphabetSize];
	int fail_ = 0;
	bool is_terminal_ = false;
};

class IntQueue
{
  public:
	explicit IntQueue(int capacity) { data_ = new int[capacity]; }

	~IntQueue() { delete[] data_; }

	void push(int value) { data_[tail_++] = value; }
	int pop() { return data_[head_++]; }
	bool empty() const { return head_ == tail_; }

  private:
	int* data_;
	int head_ = 0;
	int tail_ = 0;
};

class AhoCorasickAutomaton
{
  public:
	AhoCorasickAutomaton(const char* pattern, int pattern_len)
	{
		capacity_ = pattern_len + 1;
		nodes_ = new AhoCorasickNode*[capacity_];
		add_node();

		insert_pattern(pattern, pattern_len);
		build_fail_links();
	}

	~AhoCorasickAutomaton()
	{
		for (int i = 0; i < size_; ++i)
			delete nodes_[i];
		delete[] nodes_;
	}

	AhoCorasickAutomaton(const AhoCorasickAutomaton&) = delete;
	AhoCorasickAutomaton& operator=(const AhoCorasickAutomaton&) = delete;

	void step(unsigned char ch) { state_ = go(state_, ch); }

	bool is_match() const { return nodes_[state_]->is_terminal(); }

  private:
	AhoCorasickNode** nodes_;
	int size_ = 0;
	int capacity_;
	int state_ = 0;

	int add_node()
	{
		nodes_[size_] = new AhoCorasickNode();
		return size_++;
	}

	void insert_pattern(const char* pattern, int pattern_len)
	{
		int current = 0;

		for (int i = 0; i < pattern_len; ++i)
		{
			unsigned char ch = static_cast<unsigned char>(pattern[i]);

			if (nodes_[current]->child(ch) == -1)
				nodes_[current]->set_child(ch, add_node());

			current = nodes_[current]->child(ch);
		}

		nodes_[current]->mark_terminal();
	}

	int go(int state, unsigned char ch)
	{
		if (nodes_[state]->cached_transition(ch) != -1)
			return nodes_[state]->cached_transition(ch);

		int result;
		if (nodes_[state]->child(ch) != -1)
			result = nodes_[state]->child(ch);
		else if (state == 0)
			result = 0;
		else
			result = go(nodes_[state]->fail(), ch);

		nodes_[state]->cache_transition(ch, result);
		return result;
	}

	void build_fail_links()
	{
		IntQueue queue(capacity_);

		for (int ch = 0; ch < AhoCorasickNode::kAlphabetSize; ++ch)
		{
			int child = nodes_[0]->child(ch);
			if (child == -1)
				continue;

			nodes_[child]->set_fail(0);
			queue.push(child);
		}

		while (!queue.empty())
		{
			int current = queue.pop();

			for (int ch = 0; ch < AhoCorasickNode::kAlphabetSize; ++ch)
			{
				int child = nodes_[current]->child(ch);
				if (child == -1)
					continue;

				int fail_state = go(nodes_[current]->fail(), ch);
				nodes_[child]->set_fail(fail_state);

				if (nodes_[fail_state]->is_terminal())
					nodes_[child]->mark_terminal();

				queue.push(child);
			}
		}
	}
};

void MyString::init(std::string_view sv)
{
	len_ = sv.size();
	capacity_ = len_ + 1;
	pstr_ = new char[capacity_];
	std::memcpy(pstr_, sv.data(), len_);
	pstr_[len_] = '\0';
}

char* MyString::create_copy_of_pstr(int new_size) const
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
	if (capacity_ == 0)
	{
		pstr_ = new char[1];
		pstr_[0] = '\0';
		len_ = 0;
		capacity_ = 1;
	}

	index = check_index(index, len_, 0, "Insert or append");

	int new_capacity = len_ + count + 1;
	if (len_ + count < capacity_)
		new_capacity = capacity_;

	char* tmp = create_copy_of_pstr(new_capacity);
	std::memmove(tmp + index + count, tmp + index, len_ + 1 - index);
	std::memcpy(tmp + index, data, count);

	delete_pstr_change_params(tmp, len_ + count, new_capacity);
}

int MyString::check_index(int index, int capacity, int count,
						  std::string error_msg) const
{
	if (index > capacity || index < 0)
		throw std::out_of_range(
			"Index is bigger than length of string. \n Error in method " +
			error_msg + "\n");
	if (count < 0 || index + count > capacity)
		throw std::out_of_range("Count is not right in method " + error_msg +
								"\n");

	return index;
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

MyString::iterator MyString::begin()
{
	iterator tmp(pstr_);
	return tmp;
}
MyString::iterator MyString::end()
{
	iterator tmp(pstr_ + len_);
	return tmp;
}
MyString::const_iterator MyString::cbegin() const
{
	const_iterator tmp(pstr_);
	return tmp;
}
MyString::const_iterator MyString::cend() const
{
	const_iterator tmp(pstr_ + len_);
	return tmp;
}
MyString::reverse_iterator MyString::rbegin()
{
	reverse_iterator tmp(pstr_ + len_ - 1);
	return tmp;
}
MyString::reverse_iterator MyString::rend()
{
	reverse_iterator tmp(pstr_ - 1);
	return tmp;
}
MyString::const_reverse_iterator MyString::rcbegin() const
{
	const_reverse_iterator tmp(pstr_ + len_ - 1);
	return tmp;
}
MyString::const_reverse_iterator MyString::rcend() const
{
	const_reverse_iterator tmp(pstr_ - 1);
	return tmp;
}

MyString::MyString() : len_(0), capacity_(0) {}

MyString::MyString(std::string_view source_str) { init(source_str); }

MyString::MyString(std::string_view source_str, int count)
{
	init(source_str.substr(0, count)); // Create substring of original string
									   // that starts at 0 and ends in count
}

MyString::MyString(int count, char ch)
{
	if (count < 0)
		throw std::invalid_argument(
			"Count is must be more than zero/ Error in constructor");
	len_ = count;
	capacity_ = len_ + 1;
	pstr_ = new char[capacity_];

	std::memset(pstr_, ch, count);
	pstr_[capacity_ - 1] = '\0';
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
	int new_len = static_cast<int>(source_str.size());

	if (new_len + 1 <= capacity_)
	{
		std::memcpy(pstr_, source_str.data(), new_len);
		pstr_[new_len] = '\0';
		len_ = new_len;
	}
	else
	{
		delete_pstr_change_params(nullptr, 0, 0);
		init(source_str);
	}
}

void MyString::operator=(const char* source_str)
{
	if (source_str != pstr_)
	{
		*this = std::string_view(source_str);
	}
}

void MyString::operator=(const std::string& source_str)
{
	*this = std::string_view(source_str);
}

void MyString::operator=(char ch)
{
	if (2 <= capacity_)
	{
		pstr_[0] = ch;
		pstr_[1] = '\0';
		len_ = 1;
	}
	else
	{
		delete_pstr_change_params(new char[2], 1, 2);
		pstr_[0] = ch;
		pstr_[1] = '\0';
	}
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
		*this = std::string_view(other);
	}
	return *this;
}

const char* MyString::c_str() const { return pstr_; }

int MyString::size() const { return len_; }

int MyString::capacity() const { return capacity_; }

bool MyString::empty() const { return len_ == 0; }

void MyString::insert(int index, int count, char ch)
{
	if (count < 0)
		throw std::out_of_range("Count is negative in method insert by char");

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

void MyString::insert(iterator it, int count, char ch)
{
	int index = it - begin();
	insert(index, count, ch);
}

void MyString::insert(iterator it, std::string_view source_str)
{
	insert(it, source_str, 0, source_str.size());
}

void MyString::insert(iterator it, std::string_view source_str, int count)
{
	insert(it, source_str, 0, count);
}

void MyString::insert(iterator it, std::string_view source_str, int s_index,
					  int count)
{
	int index = it - begin();
	insert(index, source_str, s_index, count);
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
	index = check_index(index, len_, count, "erase");

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

void MyString::erase(iterator it, int count)
{
	int index = it - begin();
	erase(index, count);
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

void MyString::replace(int index, int count, std::string_view source_str,
					   int s_index, int s_count)
{
	std::string_view replace_str = source_str.substr(s_index, s_count);
	erase(index, count);
	insert(index, replace_str);
}

void MyString::replace(iterator it, int count, std::string_view source_str)
{
	replace(it, count, source_str, 0, source_str.size());
}

void MyString::replace(iterator it, int count, std::string_view source_str,
					   int s_count)
{
	replace(it, count, source_str, 0, s_count);
}

void MyString::replace(iterator it, int count, std::string_view source_str,
					   int s_index, int s_count)
{
	int index = it - begin();
	replace(index, count, source_str, s_index, s_count);
}

MyString MyString::substr(int index) const
{
	return substr(index, len_ - index);
}

MyString MyString::substr(int index, int count) const
{
	index = check_index(index, len_, count, "substr");

	MyString new_str = *this; // Unname pointer and make copy

	if (index != 0)
		new_str.erase(0, index); // delete start of the string previous to
								 // index were we need substr

	new_str.erase(count, len_ - index -
							 count); // delete other part after substr to end

	new_str.shrink_to_fit();

	return new_str;
}

MyString MyString::substr(const_iterator it) const
{
	int index = it - cbegin();
	return substr(it, len_ - index);
}

MyString MyString::substr(const_iterator it, int count) const
{
	int index = it - cbegin();
	return substr(index, count);
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
	index = check_index(index, len_, 0, "operator [] changing index");

	return pstr_[index];
}

const char& MyString::operator[](int index) const
{
	index = check_index(index, len_, 0, "operator [] geting index");
	return pstr_[index];
}

short MyString::compare(const MyString& other) const
{
	int this_len = this->len_;

	for (int i = 0; i < this_len; ++i)
	{
		if (other.len_ < i)
			return 1;

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

bool MyString::operator>(const MyString& other) const
{
	return this->compare(other) == 1;
}
bool MyString::operator<(const MyString& other) const
{
	return this->compare(other) == -1;
}
bool MyString::operator>=(const MyString& other) const
{
	return this->compare(other) != -1;
}
bool MyString::operator<=(const MyString& other) const
{
	return this->compare(other) != 1;
}
bool MyString::operator!=(const MyString& other) const
{
	return this->compare(other) != 0;
}
bool MyString::operator==(const MyString& other) const
{
	return this->compare(other) == 0;
}

std::basic_ifstream<char>& operator>>(std::basic_ifstream<char>& is,
									  MyString& str)
{
	str = "";

	char ch;

	while (is.get(ch))
	{
		if (!std::isspace(static_cast<unsigned char>(ch)))
		{
			str.append(1, ch);
			break;
		}
	}

	while (is.get(ch))
	{
		if (std::isspace(static_cast<unsigned char>(ch)))
		{
			is.unget();
			break;
		}

		str.append(1, ch);
	}

	return is;
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
	index = check_index(index, len_, 0, "find with index");

	int m = static_cast<int>(source_str.size());

	if (m == 0)
		return 0;

	AhoCorasickAutomaton automaton(source_str.data(), m);

	for (int i = index; i < len_; ++i)
	{
		automaton.step(static_cast<unsigned char>(pstr_[i]));

		if (automaton.is_match())
			return i - m + 1;
	}

	return -1;
}

char MyString::at(int index)
{
	if (index == 0 && len_ == 0)
		throw std::invalid_argument("Call method \"at\" at empty string");
	index = check_index(index, len_, 0, "at");

	return pstr_[index];
}

char MyString::at(iterator it)
{
	int index = it - begin();
	return at(index);
}

int MyString::to_int() { return static_cast<int>(this->to_float()); }

float MyString::to_float()
{
	for (int i = 0; i < this->len_; i++)
	{
		char tmp = this->pstr_[i];
		if ((tmp < '0' || tmp > '9') &&
			(tmp != '.' && tmp != '-' && tmp != '+' && tmp != 'f'))
		{
			std::string error_msg = "Cant transform this string to number ";
			error_msg += tmp;
			throw WrongTransformException(error_msg);
		}
	}

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
