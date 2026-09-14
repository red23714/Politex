from mystring import MyString as MyString

def test_ctor():
    s0 = MyString()
    assert s0.c_str() == "", "default ctor"

    s1 = MyString("test")
    assert s1.c_str() == "test", "C str ctor"

    s2 = MyString("test", 3)
    assert s2.c_str() == "tes", "C str ctor with count"

    s3 = MyString(5000, 'B')
    assert s3.c_str() == 'B'*5000, "count of char ctor"

    s4 = MyString("copyctor")
    s5 = MyString(s4)
    assert s5.c_str() == s4.c_str(), "copy ctor"


def test_getters():
    s = MyString("some string")
    assert s.c_str() == "some string", "C str getter"
    assert s.size() == 11, "size getter"
    assert s.capacity() >= 12, "capacity getter"

# CORE METHODS
def test_insert():
    s1 = MyString("initial")
    s1.insert(0, "prefix_")
    assert s1.c_str() == "prefix_initial", "insert at the beginning"
    assert s1.size() == len("prefix_initial"), "insert at the beginning size"

    s1.insert(s1.size(), "_suffix")
    assert s1.c_str() == "prefix_initial_suffix", "insert at the end"
    assert s1.size() == len("prefix_initial_suffix"), "insert at the end size"

    s1.insert(0, 3, 'C')
    assert s1.c_str() == 'C' * 3 + "prefix_initial_suffix", "insert count of char"
    assert s1.size() == len('C' * 3 + "prefix_initial_suffix"), "insert count of char size"

    s1.insert(s1.size(), "XXXXX", 3)
    assert s1.c_str() == "CCCprefix_initial_suffixXXX", "insert by index, C str and count"
    assert s1.size() == len("CCCprefix_initial_suffixXXX"), "insert by index, C str and count size"

def test_append():
    s1 = MyString("Hello")
    s1.append(" World")
    assert s1.c_str() == "Hello World", "append C str"
    assert s1.size() == len("Hello World"), "append C str size"

    s1.append(5, 'A')
    assert s1.c_str() == "Hello World" + 'A'*5, "append count of char"
    assert s1.size() == len("Hello World" + 'A'*5), "append count of char size"

    s1.append("1234567890", 3, 2)
    assert s1.c_str() == "Hello World" + 'A'*5 + "45", "append C str with count"
    assert s1.size() == len("Hello World" + 'A'*5 + "45"), "append C str with count size"

def test_erase():
    s1 = MyString("Hello World")
    cap1 = s1.capacity()
    s1.erase(0, 6)
    assert cap1 == s1.capacity(), "erase cap"
    assert s1.c_str() == "World", "erase at the beginning"


    s2 = MyString("Hello World")
    cap2 = s2.capacity()
    s2.erase(5, 6)
    assert cap2 == s2.capacity(), "erase cap"
    assert s2.c_str() == "Hello", "erase at the end"

def test_clear():
    s1 = MyString(100, 'O')
    cap1 = s1.capacity()
    s1.clear()
    assert cap1 == s1.capacity(), "cap clear"
    assert s1.c_str() == "", "clear"
    assert s1.size() == 0, "clear size"

def test_replace():
    s1 = MyString("Hello World")
    s1.replace(0, 5, "World")
    s1.c_str() == "World World", "replace at the beginning"

    s2 = MyString("Hello World")
    s2.replace(6, 5, "Hello")
    s2.c_str() == "Hello Hello", "replace at the end"

    s3 = MyString("Hello World")
    cap3 = s3.capacity()
    s3.replace(0, 5, "A")
    assert cap3 == s3.capacity(), "replace cap"
    assert s3.c_str() == "A World"

def test_substr():
    s1 = MyString("Str1 Str2 Str3")
    s1sub = s1.substr(5)
    assert type(s1sub) == MyString and s1sub.size() == 9 and s1sub.c_str() == "Str2 Str3", "SUBSTRING FULL SIZE"
    s2 = MyString("Str1 Str2 Str3")
    s2sub = s2.substr(5,4)
    assert type(s1sub) == MyString and len(s2sub) == 4 and s2sub.c_str() == "Str2", "SUBSTRING OF LENGTH"


# OPERATORS
def test_add():
    s1 = MyString("Hello ")
    s2 = MyString("World")

    s_sum = s1 + s2
    assert type(s_sum) == type(s1)
    assert s_sum.c_str() == "Hello World", "plus operator"

def test_iadd():
    s1 = MyString("Hello ")
    s1 += "World"
    s1 += s1

    assert type(s1) == MyString
    assert s1.c_str() == "Hello WorldHello World"

def test_capacity():
    s1 = MyString(500, 'A')
    initial_capacity = s1.capacity()
    assert initial_capacity >= 501, "initial capacity"

    s1.clear()
    assert initial_capacity == s1.capacity(), "capacity after clear"
    assert s1.size() == 0, "size after clear"

    s1.insert(0, "AAAA")
    assert s1.capacity() == initial_capacity, "capacity after insert"

    s1.append("BBBB")
    assert s1.capacity() == initial_capacity, "capacity after append"

    s1.replace(0, 0, "__CCCC")
    assert s1.capacity() == initial_capacity, "capacity after replace"

    s1.erase(0, 2)
    assert s1.capacity() == initial_capacity, "capacity after erase"

    assert s1.c_str() == "CCCCAAAABBBB", "capacity str value"
    assert s1.size() == len("CCCCAAAABBBB"), "capacity str size"

    s1.append(5000, 'N')
    assert s1.capacity() > initial_capacity, "capacity after str extension"
    assert s1.c_str() == "CCCCAAAABBBB" + 'N'*5000, "capacity str value after extension"

    prev_capacity = s1.capacity()
    s1.clear()
    s1+=MyString(2, "*")
    assert s1.capacity() == prev_capacity and s1.size() == 2 and s1.c_str() == "**"
    s1.shrink_to_fit()
    assert s1.capacity() == 3 and s1.size() == 2 and s1.c_str() == "**"
    

def test_comparison():
    str1 = MyString("apple")
    str2 = MyString("banana")
    str3 = MyString("apple")

    result = str1 == str3
    assert result == True, "OPERATOR=="
    result = str1 != str2
    assert result == True, "OPERATOR!="
    result = str1 < str2
    assert result == True, "OPERATOR<"
    result = str1 <= str3
    assert result == True, "OPERATOR<="
    result = str2 > str1
    assert result == True, "OPERATOR>"
    result = str2 >= str1
    assert result == True, "OPERATOR>="

# OTHER METHODS
def test_find():
    str_value = MyString("Hello, world! Hello, world!")
    pos1 = str_value.find("Hello")
    assert pos1 == 0, "FIND CONST CHAR* SUBSTR"
    pos2 = str_value.find("world", 9)
    assert pos2 == 21, "FIND CONST CHAR* SUBSTR BY INDEX"
    pos3 = str_value.find("Hello")
    assert pos3 == 0, "FIND STD::STRING SUBSTR"
    pos4 = str_value.find("world", 9)
    assert pos4 == 21, "FIND STD::STRING SUBSTR BY INDEX"

