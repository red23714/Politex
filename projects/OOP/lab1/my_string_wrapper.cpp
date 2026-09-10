#include <boost/python.hpp>
#include <string_view>
#include "src/my_string/my_string.h"

namespace bp = boost::python;

struct string_view_from_python
{
	string_view_from_python()
	{
		bp::converter::registry::push_back(&convertible, &construct,
										   bp::type_id<std::string_view>());
	}

	static void* convertible(PyObject* obj_ptr)
	{
		if (PyUnicode_Check(obj_ptr) || PyBytes_Check(obj_ptr))
			return obj_ptr;

		bp::extract<MyString&> as_my_string(obj_ptr);
		if (as_my_string.check())
			return obj_ptr;

		return nullptr;
	}

	static void construct(PyObject* obj_ptr,
						  bp::converter::rvalue_from_python_stage1_data* data)
	{
		const char* value = nullptr;
		Py_ssize_t len = 0;

		if (PyUnicode_Check(obj_ptr))
		{
			value = PyUnicode_AsUTF8AndSize(obj_ptr, &len);
		}
		else if (PyBytes_Check(obj_ptr))
		{
			char* tmp = nullptr;
			PyBytes_AsStringAndSize(obj_ptr, &tmp, &len);
			value = tmp;
		}
		else
		{
			MyString& src = bp::extract<MyString&>(obj_ptr);
			value = src.c_str();
			len = src.size();
		}

		void* storage =
			((bp::converter::rvalue_from_python_storage<std::string_view>*)data)
				->storage.bytes;
		new (storage) std::string_view(value, static_cast<size_t>(len));
		data->convertible = storage;
	}
};

void assign_string(MyString& self, const std::string& value)
{
	self = std::string_view(value);
}

void assign_char(MyString& self, const std::string& value)
{
	if (value.size() != 1)
		throw std::invalid_argument(
			"assign(char) expects exactly one character");

	self = value[0];
}

void assign_my_string(MyString& self, const MyString& value) { self = value; }

char getitem(MyString& self, int index) { return self[index]; }
void setitem(MyString& self, int index, char ch) { self[index] = ch; }
std::string to_py_str(const MyString& self)
{
	return std::string(self.c_str());
}

BOOST_PYTHON_MODULE(my_string)
{
	string_view_from_python();

	bp::class_<MyString>("MyString")
		// --- конструкторы ---
		.def(bp::init<std::string_view>())
		.def(bp::init<std::string_view, int>())
		.def(bp::init<int, char>())
		.def(bp::init<int32_t>())
		.def(bp::init<float>())
		.def(bp::init<const MyString&>())

		// --- служебные ---
		.def("clear", &MyString::clear)
		.def("shrink_to_fit", &MyString::shrink_to_fit)

		// --- геттеры ---
		.def("c_str", &MyString::c_str)
		.def("size", &MyString::size)
		.def("capacity", &MyString::capacity)
		.def("empty", &MyString::empty)

		.def("insert",
			 static_cast<void (MyString::*)(int, int, char)>(&MyString::insert))
		.def("insert", static_cast<void (MyString::*)(int, std::string_view)>(
						   &MyString::insert))
		.def("insert",
			 static_cast<void (MyString::*)(int, std::string_view, int)>(
				 &MyString::insert))
		.def("insert",
			 static_cast<void (MyString::*)(int, std::string_view, int, int)>(
				 &MyString::insert))

		.def("append",
			 static_cast<void (MyString::*)(int, char)>(&MyString::append))
		.def("append", static_cast<void (MyString::*)(std::string_view)>(
						   &MyString::append))
		.def("append", static_cast<void (MyString::*)(std::string_view, int)>(
						   &MyString::append))
		.def("append",
			 static_cast<void (MyString::*)(std::string_view, int, int)>(
				 &MyString::append))

		.def("erase", &MyString::erase)
		.def("replace",
			 static_cast<void (MyString::*)(int, int, std::string_view)>(
				 &MyString::replace))
		.def("replace",
			 static_cast<void (MyString::*)(int, int, std::string_view, int)>(
				 &MyString::replace))
		.def("replace",
			 static_cast<void (MyString::*)(int, int, std::string_view, int,
											int)>(&MyString::replace))

		.def("substr",
			 static_cast<MyString (MyString::*)(int) const>(&MyString::substr))
		.def("substr", static_cast<MyString (MyString::*)(int, int) const>(
						   &MyString::substr))

		.def("find", static_cast<int (MyString::*)(std::string_view) const>(
						 &MyString::find))
		.def("find",
			 static_cast<int (MyString::*)(std::string_view, int) const>(
				 &MyString::find))

		.def("at", &MyString::at)
		.def("to_int", &MyString::to_int)
		.def("to_float", &MyString::to_float)

		.def("assign", &assign_string)
		.def("assign_char", &assign_char)
		.def("assign", &assign_my_string)

		.def("__add__", &MyString::operator+)
		.def("__iadd__", &MyString::operator+=,
			 bp::return_internal_reference<>())
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
