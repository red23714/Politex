#include <pybind11/pybind11.h>

#include <stdexcept>
#include <string>
#include <string_view>

#include IMYSTRING

namespace py = pybind11;

namespace
{

std::string extract_string(const py::object& obj)
{
	if (py::isinstance<py::str>(obj))
		return obj.cast<std::string>();

	if (py::isinstance<py::bytes>(obj))
		return obj.cast<std::string>();

	if (py::isinstance<MyString>(obj))
	{
		MyString& ms = obj.cast<MyString&>();
		return std::string(ms.c_str(), static_cast<size_t>(ms.size()));
	}

	throw py::type_error("expected str, bytes or MyString");
}

bool looks_like_string_source(const py::object& obj)
{
	return py::isinstance<py::str>(obj) || py::isinstance<py::bytes>(obj) ||
		   py::isinstance<MyString>(obj);
}

char extract_char(const py::object& obj)
{
	if (py::isinstance<py::int_>(obj))
		return static_cast<char>(obj.cast<int>());

	if (looks_like_string_source(obj))
	{
		std::string tmp = extract_string(obj);
		if (tmp.size() != 1)
			throw py::type_error(
				"expected a single character (str of length 1, bytes of "
				"length 1, or an int ordinal)");
		return tmp[0];
	}

	throw py::type_error(
		"expected a single character (str of length 1, bytes of length 1, "
		"or an int ordinal)");
}

void assign_string(MyString& self, const py::object& value)
{
	std::string tmp = extract_string(value);
	self = std::string_view(tmp);
}

void assign_char(MyString& self, const py::object& value)
{
	std::string tmp = extract_string(value);
	if (tmp.size() != 1)
		throw std::invalid_argument(
			"assign(char) expects exactly one character");

	self = tmp[0];
}

void assign_my_string(MyString& self, const MyString& value) { self = value; }

char getitem(MyString& self, int index) { return self[index]; }
void setitem(MyString& self, int index, char ch) { self[index] = ch; }
std::string to_py_str(const MyString& self)
{
	const char* p = self.c_str();
	return p ? std::string(p) : std::string();
}

} // namespace

PYBIND11_MODULE(mystring, m)
{
	py::class_<MyString> cls(m, "MyString");

	cls.def(py::init<>())
		.def(py::init(
				 [](py::object a, py::object b)
				 {
					 if (a.is_none())
						 throw py::type_error(
							 "MyString() requires at least one argument");

					 if (looks_like_string_source(a))
					 {
						 std::string tmp = extract_string(a);
						 if (b.is_none())
							 return new MyString(std::string_view(tmp));
						 return new MyString(std::string_view(tmp),
											 b.cast<int>());
					 }

					 if (py::isinstance<py::int_>(a) && !b.is_none() &&
						 !py::isinstance<py::float_>(b))
					 {
						 try
						 {
							 char ch = extract_char(b);
							 return new MyString(a.cast<int>(), ch);
						 }
						 catch (const py::type_error&)
						 {
						 }
					 }

					 if (b.is_none())
					 {
						 if (py::isinstance<py::float_>(a))
							 return new MyString(a.cast<float>());
						 if (py::isinstance<py::int_>(a))
							 return new MyString(
								 static_cast<int32_t>(a.cast<int>()));
					 }

					 throw py::type_error(
						 "unsupported MyString(...) argument combination");
				 }),
			 py::arg("a"), py::arg("b") = py::none())
		.def(py::init<const MyString&>())

		// --- misc ---
		.def("clear", &MyString::clear)
		.def("shrink_to_fit", &MyString::shrink_to_fit)

		// --- getters ---
		.def("c_str",
			 [](const MyString& self)
			 {
				 const char* p = self.c_str();
				 return p ? std::string(p) : std::string();
			 })
		.def("size", &MyString::size)
		.def("capacity", &MyString::capacity)
		.def("empty", &MyString::empty)

		// --- insert ---
		.def(
			"insert", [](MyString& self, int pos, int count, py::object ch)
			{ self.insert(pos, count, extract_char(ch)); }, py::arg("pos"),
			py::arg("count"), py::arg("ch"))
		.def(
			"insert",
			[](MyString& self, int pos, py::object value, py::object count,
			   py::object offset)
			{
				std::string tmp = extract_string(value);
				if (count.is_none())
					self.insert(pos, std::string_view(tmp));
				else if (offset.is_none())
					self.insert(pos, std::string_view(tmp), count.cast<int>());
				else
					self.insert(pos, std::string_view(tmp), count.cast<int>(),
								offset.cast<int>());
			},
			py::arg("pos"), py::arg("value"), py::arg("count") = py::none(),
			py::arg("offset") = py::none())

		// --- append ---
		.def(
			"append", [](MyString& self, int count, py::object ch)
			{ self.append(count, extract_char(ch)); }, py::arg("count"),
			py::arg("ch"))
		.def(
			"append",
			[](MyString& self, py::object value, py::object count,
			   py::object offset)
			{
				std::string tmp = extract_string(value);
				if (count.is_none())
					self.append(std::string_view(tmp));
				else if (offset.is_none())
					self.append(std::string_view(tmp), count.cast<int>());
				else
					self.append(std::string_view(tmp), count.cast<int>(),
								offset.cast<int>());
			},
			py::arg("value"), py::arg("count") = py::none(),
			py::arg("offset") = py::none())

		.def("erase",
			 static_cast<void (MyString::*)(int, int)>(&MyString::erase))

		// --- replace ---
		.def(
			"replace",
			[](MyString& self, int pos, int len, py::object value,
			   py::object count, py::object offset)
			{
				std::string tmp = extract_string(value);
				if (count.is_none())
					self.replace(pos, len, std::string_view(tmp));
				else if (offset.is_none())
					self.replace(pos, len, std::string_view(tmp),
								 count.cast<int>());
				else
					self.replace(pos, len, std::string_view(tmp),
								 count.cast<int>(), offset.cast<int>());
			},
			py::arg("pos"), py::arg("len"), py::arg("value"),
			py::arg("count") = py::none(), py::arg("offset") = py::none())

		// --- substr ---
		.def(
			"substr",
			[](const MyString& self, int pos, py::object len)
			{
				if (len.is_none())
					return self.substr(pos);
				return self.substr(pos, len.cast<int>());
			},
			py::arg("pos"), py::arg("len") = py::none())

		// --- find ---
		.def(
			"find",
			[](const MyString& self, py::object value, py::object from)
			{
				std::string tmp = extract_string(value);
				if (from.is_none())
					return self.find(std::string_view(tmp));
				return self.find(std::string_view(tmp), from.cast<int>());
			},
			py::arg("value"), py::arg("from") = py::none())

		.def("at", static_cast<char (MyString::*)(int)>(&MyString::at))
		.def("to_int", &MyString::to_int)
		.def("to_float", &MyString::to_float)

		// --- assign ---
		.def("assign", &assign_string)
		.def("assign_char", &assign_char)
		.def("assign", &assign_my_string)

		// --- operators ---
		.def("__add__",
			 [](const MyString& self, py::object other)
			 {
				 std::string tmp = extract_string(other);
				 return self + std::string_view(tmp);
			 })
		.def(
			"__iadd__",
			[](MyString& self, py::object other) -> MyString&
			{
				std::string tmp = extract_string(other);
				self += std::string_view(tmp);
				return self;
			},
			py::is_operator())
		.def("compare", &MyString::compare)
		.def("__eq__", &MyString::operator==)
		.def("__ne__", &MyString::operator!=)
		.def("__lt__", &MyString::operator<)
		.def("__gt__", &MyString::operator>)
		.def("__le__", &MyString::operator<=)
		.def("__ge__", &MyString::operator>=)

		.def("__getitem__", &getitem)
		.def("__setitem__", &setitem)

		.def("__len__", &MyString::size)
		.def("__str__", &to_py_str)
		.def("__repr__", &to_py_str)

#ifdef DEBUG
		.def("pstr", &MyString::pstr)
#endif
		;
}
