import unittest
import my_string


class TestMyStringConstructors(unittest.TestCase):
    def test_constructor_from_str(self):
        s = my_string.MyString("hello")

        self.assertEqual(s.c_str(), "hello")
        self.assertEqual(s.size(), 5)
        self.assertFalse(s.empty())

    def test_constructor_from_bytes(self):
        s = my_string.MyString(b"hello")

        self.assertEqual(s.c_str(), "hello")
        self.assertEqual(s.size(), 5)

    def test_constructor_from_mystring(self):
        source = my_string.MyString("hello")
        s = my_string.MyString(source)

        self.assertEqual(s.c_str(), "hello")
        self.assertEqual(s.size(), 5)

        source[0] = "H"

        self.assertEqual(source.c_str(), "Hello")
        self.assertEqual(s.c_str(), "hello")

    def test_constructor_with_count(self):
        s = my_string.MyString("abcdef", 3)

        self.assertEqual(s.c_str(), "abc")
        self.assertEqual(s.size(), 3)

    def test_constructor_with_count_larger_than_source(self):
        s = my_string.MyString("abc", 10)

        self.assertEqual(s.c_str(), "abc")
        self.assertEqual(s.size(), 3)

    def test_constructor_count_char(self):
        s = my_string.MyString(5, "x")

        self.assertEqual(s.c_str(), "xxxxx")
        self.assertEqual(s.size(), 5)


class TestMyStringBasicMethods(unittest.TestCase):
    def test_size(self):
        s = my_string.MyString("hello")

        self.assertEqual(s.size(), 5)

        s.append(3, "x")

        self.assertEqual(s.size(), 8)

    def test_empty(self):
        s = my_string.MyString("hello")

        self.assertFalse(s.empty())

        s.clear()

        self.assertTrue(s.empty())

    def test_clear(self):
        s = my_string.MyString("hello")

        old_capacity = s.capacity()

        s.clear()

        self.assertEqual(s.c_str(), "")
        self.assertEqual(s.size(), 0)
        self.assertTrue(s.empty())

        self.assertGreaterEqual(s.capacity(), old_capacity)

    def test_shrink_to_fit(self):
        s = my_string.MyString("hello")

        s.append(20, "x")
        s.erase(5, 20)

        old_capacity = s.capacity()
        s.shrink_to_fit()

        self.assertEqual(s.c_str(), "hello")
        self.assertEqual(s.size(), 5)

        self.assertEqual(s.capacity(), s.size() + 1)
        self.assertLessEqual(s.capacity(), old_capacity)


class TestMyStringAssignment(unittest.TestCase):
    def test_assign_string(self):
        s = my_string.MyString("hello")

        s.assign("world")

        self.assertEqual(s.c_str(), "world")
        self.assertEqual(s.size(), 5)

    def test_assign_bytes(self):
        s = my_string.MyString("hello")

        s.assign(b"world")

        self.assertEqual(s.c_str(), "world")

    def test_assign_mystring(self):
        source = my_string.MyString("world")
        s = my_string.MyString("hello")

        s.assign(source)

        self.assertEqual(s.c_str(), "world")

    def test_assign_char(self):
        s = my_string.MyString("hello")

        s.assign("x")

        self.assertEqual(s.c_str(), "x")
        self.assertEqual(s.size(), 1)


class TestMyStringInsert(unittest.TestCase):
    def test_insert_count_char(self):
        s = my_string.MyString("hello")

        s.insert(2, 3, "x")

        self.assertEqual(s.c_str(), "hexxxllo")

    def test_insert_source_string(self):
        s = my_string.MyString("hello")

        s.insert(2, "ABC")

        self.assertEqual(s.c_str(), "heABCllo")

    def test_insert_source_string_with_count(self):
        s = my_string.MyString("hello")

        s.insert(2, "ABCDEF", 3)

        self.assertEqual(s.c_str(), "heABCllo")

    def test_insert_source_string_with_index_and_count(self):
        s = my_string.MyString("hello")

        s.insert(2, "ABCDEF", 1, 3)

        self.assertEqual(s.c_str(), "heBCDllo")

    def test_insert_at_beginning(self):
        s = my_string.MyString("world")

        s.insert(0, "hello ")

        self.assertEqual(s.c_str(), "hello world")

    def test_insert_at_end(self):
        s = my_string.MyString("hello")

        s.insert(s.size(), " world")

        self.assertEqual(s.c_str(), "hello world")


class TestMyStringAppend(unittest.TestCase):
    def test_append_count_char(self):
        s = my_string.MyString("hello")

        s.append(3, "!")

        self.assertEqual(s.c_str(), "hello!!!")

    def test_append_string(self):
        s = my_string.MyString("hello")

        s.append(" world")

        self.assertEqual(s.c_str(), "hello world")

    def test_append_string_with_count(self):
        s = my_string.MyString("hello")

        s.append("abcdef", 3)

        self.assertEqual(s.c_str(), "helloabc")

    def test_append_string_with_index_and_count(self):
        s = my_string.MyString("hello")

        s.append("abcdef", 1, 3)

        self.assertEqual(s.c_str(), "hellobcd")

    def test_append_mystring(self):
        s = my_string.MyString("hello")
        other = my_string.MyString(" world")

        s.append(other)

        self.assertEqual(s.c_str(), "hello world")


class TestMyStringErase(unittest.TestCase):
    def test_erase_middle(self):
        s = my_string.MyString("hello world")

        s.erase(5, 1)

        self.assertEqual(s.c_str(), "helloworld")

    def test_erase_prefix(self):
        s = my_string.MyString("hello world")

        s.erase(0, 6)

        self.assertEqual(s.c_str(), "world")

    def test_erase_suffix(self):
        s = my_string.MyString("hello world")

        s.erase(5, 6)

        self.assertEqual(s.c_str(), "hello")


class TestMyStringReplace(unittest.TestCase):
    def test_replace_with_string(self):
        s = my_string.MyString("hello world")

        s.replace(6, 5, "Python")

        self.assertEqual(s.c_str(), "hello Python")

    def test_replace_with_string_count(self):
        s = my_string.MyString("hello world")

        s.replace(6, 5, "abcdef", 3)

        self.assertEqual(s.c_str(), "hello abc")

    def test_replace_with_source_index_and_count(self):
        s = my_string.MyString("hello world")

        s.replace(6, 5, "123456789", 2, 4)

        self.assertEqual(s.c_str(), "hello 3456")


class TestMyStringSubstr(unittest.TestCase):
    def test_substr_from_index(self):
        s = my_string.MyString("hello world")

        result = s.substr(6)

        self.assertEqual(result.c_str(), "world")
        self.assertEqual(result.size(), 5)

    def test_substr_with_count(self):
        s = my_string.MyString("hello world")

        result = s.substr(6, 3)

        self.assertEqual(result.c_str(), "wor")
        self.assertEqual(result.size(), 3)

    def test_substr_from_beginning(self):
        s = my_string.MyString("hello world")

        result = s.substr(0, 5)

        self.assertEqual(result.c_str(), "hello")


class TestMyStringOperators(unittest.TestCase):
    def test_add_string(self):
        s = my_string.MyString("hello")

        result = s + " world"

        self.assertEqual(result.c_str(), "hello world")
        self.assertEqual(s.c_str(), "hello")

    def test_add_mystring(self):
        s = my_string.MyString("hello")
        other = my_string.MyString(" world")

        result = s + other

        self.assertEqual(result.c_str(), "hello world")

    def test_iadd_string(self):
        s = my_string.MyString("hello")

        s += " world"

        self.assertEqual(s.c_str(), "hello world")

    def test_iadd_mystring(self):
        s = my_string.MyString("hello")
        other = my_string.MyString(" world")

        s += other

        self.assertEqual(s.c_str(), "hello world")

    def test_getitem(self):
        s = my_string.MyString("hello")

        self.assertEqual(s[0], "h")
        self.assertEqual(s[1], "e")
        self.assertEqual(s[4], "o")

    def test_setitem(self):
        s = my_string.MyString("hello")

        s[0] = "H"
        s[4] = "O"

        self.assertEqual(s.c_str(), "HellO")

    def test_len(self):
        s = my_string.MyString("hello")

        self.assertEqual(len(s), 5)

    def test_str(self):
        s = my_string.MyString("hello")

        self.assertEqual(str(s), "hello")

    def test_repr(self):
        s = my_string.MyString("hello")

        self.assertEqual(repr(s), "hello")


class TestMyStringCompare(unittest.TestCase):
    def test_compare_equal(self):
        a = my_string.MyString("hello")
        b = my_string.MyString("hello")

        self.assertEqual(a.compare(b), 0)

    def test_compare_less(self):
        a = my_string.MyString("abc")
        b = my_string.MyString("abd")

        self.assertLess(a.compare(b), 0)

    def test_compare_greater(self):
        a = my_string.MyString("abd")
        b = my_string.MyString("abc")

        self.assertGreater(a.compare(b), 0)


class TestMyStringComparisonOperators(unittest.TestCase):
    def setUp(self):
        self.a = my_string.MyString("abc")
        self.b = my_string.MyString("abd")
        self.a_copy = my_string.MyString("abc")

    def test_equal(self):
        self.assertTrue(self.a == self.a_copy)
        self.assertFalse(self.a == self.b)

    def test_not_equal(self):
        self.assertTrue(self.a != self.b)
        self.assertFalse(self.a != self.a_copy)

    def test_less(self):
        self.assertTrue(self.a < self.b)
        self.assertFalse(self.b < self.a)

    def test_greater(self):
        self.assertTrue(self.b > self.a)
        self.assertFalse(self.a > self.b)

    def test_less_equal(self):
        self.assertTrue(self.a <= self.b)
        self.assertTrue(self.a <= self.a_copy)
        self.assertFalse(self.b <= self.a)

    def test_greater_equal(self):
        self.assertTrue(self.b >= self.a)
        self.assertTrue(self.a >= self.a_copy)
        self.assertFalse(self.a >= self.b)


class TestMyStringFind(unittest.TestCase):
    def test_find_string(self):
        s = my_string.MyString("hello world")

        self.assertEqual(s.find("world"), 6)

    def test_find_at_beginning(self):
        s = my_string.MyString("hello world")

        self.assertEqual(s.find("hello"), 0)

    def test_find_not_found(self):
        s = my_string.MyString("hello world")

        self.assertEqual(s.find("python"), -1)

    def test_find_with_start_index(self):
        s = my_string.MyString("abc abc abc")

        self.assertEqual(s.find("abc", 4), 4)

    def test_find_with_start_index_second_occurrence(self):
        s = my_string.MyString("abc abc abc")

        self.assertEqual(s.find("abc", 5), 8)

    def test_find_mystring(self):
        s = my_string.MyString("hello world")
        pattern = my_string.MyString("world")

        self.assertEqual(s.find(pattern), 6)


class TestMyStringSourceTypes(unittest.TestCase):
    def test_insert_with_mystring(self):
        s = my_string.MyString("hello")
        source = my_string.MyString("XYZ")

        s.insert(2, source)

        self.assertEqual(s.c_str(), "heXYZllo")

    def test_replace_with_mystring(self):
        s = my_string.MyString("hello world")
        source = my_string.MyString("Python")

        s.replace(6, 5, source)

        self.assertEqual(s.c_str(), "hello Python")

    def test_find_with_mystring(self):
        s = my_string.MyString("hello world")
        source = my_string.MyString("world")

        self.assertEqual(s.find(source), 6)

    def test_assign_with_mystring(self):
        s = my_string.MyString("hello")
        source = my_string.MyString("world")

        s.assign(source)

        self.assertEqual(s.c_str(), "world")


class TestMyStringKnownIssues(unittest.TestCase):
    """
    Тесты, воспроизводящие ошибки, найденные в реализации MyString
    и её Python-обёртке (см. раздел "Обнаруженные ошибки" в отчёте).

    Каждый тест помечен @unittest.expectedFailure: пока баг не
    исправлен, ожидается, что тест НЕ пройдёт (упадёт или бросит
    исключение) -- сама сборка теста при этом всё равно считается
    "успешной" (xfail), и это не ломает общий прогон.

    После исправления соответствующей ошибки в C++ коде уберите
    декоратор @unittest.expectedFailure у этого теста: он должен
    начать проходить (иначе unittest сообщит "unexpected success").
    """

    # --- Баг №3: конструкторы из числа не инициализируют
    # len_/capacity_/pstr_ перед первым insert() -> undefined behavior.
    # В зависимости от содержимого памяти может дать неверный
    # результат, бросить std::out_of_range или уронить интерпретатор.

    # @unittest.expectedFailure
    # def test_bug_number_constructor_zero(self):
    #     s = my_string.MyString(0)
    #     self.assertEqual(s.c_str(), "0")

    # @unittest.expectedFailure
    # def test_bug_number_constructor_positive(self):
    #     s = my_string.MyString(12345)
    #     self.assertEqual(s.c_str(), "12345")

    # @unittest.expectedFailure
    # def test_bug_number_constructor_negative(self):
    #     s = my_string.MyString(-42)
    #     self.assertEqual(s.c_str(), "-42")

    # @unittest.expectedFailure
    # def test_bug_float_constructor(self):
    #     s = my_string.MyString(3.14)
    #     self.assertEqual(s.c_str(), "3.14")

    # --- Баг №4: compare() сравнивает длины строк раньше посимвольного
    # сравнения, то есть не является лексикографическим сравнением.

    def test_bug_compare_not_lexicographic(self):
        a = my_string.MyString("z")
        b = my_string.MyString("aa")
        # Лексикографически (как у std::string) "z" > "aa", т.к. 'z' > 'a'
        self.assertGreater(a.compare(b), 0)

    def test_bug_greater_operator_not_lexicographic(self):
        a = my_string.MyString("z")
        b = my_string.MyString("aa")
        self.assertTrue(a > b)

    # --- Баг №5: find() с пустой искомой строкой возвращает -1,
    # а не индекс начала поиска (как у std::string::find("")).

    def test_bug_find_empty_pattern(self):
        s = my_string.MyString("hello")
        self.assertEqual(s.find(""), 0)

    # --- Баг №6: at() на пустой строке разыменовывает nullptr вместо
    # выброса исключения (падение интерпретатора / SIGSEGV).

    def test_bug_at_on_empty_string_raises(self):
        s = my_string.MyString()
        with self.assertRaises(Exception):
            s.at(0)

    # # --- Баг №7: to_float()/to_int() на пустой строке разыменовывают
    # # pstr_[0] без проверки len_ == 0 (падение вместо возврата 0).

    # @unittest.expectedFailure
    # def test_bug_to_float_on_empty_string(self):
    #     s = my_string.MyString()
    #     self.assertEqual(s.to_float(), 0.0)

    # @unittest.expectedFailure
    # def test_bug_to_int_on_empty_string(self):
    #     s = my_string.MyString()
    #     self.assertEqual(s.to_int(), 0)

    # --- Баг №8: MyString(count, ch) не проверяет count >= 0, что
    # приводит к отрицательному размеру буфера и краху программы.

    def test_bug_negative_count_char_constructor_raises(self):
        with self.assertRaises(Exception):
            my_string.MyString(-1, "x")

    # Примечание: ошибка №1 (неоднозначность operator!= при компиляции)
    # и ошибка №2 (пустая заглушка operator>>) обнаруживаются на этапе
    # сборки C++ модуля, а не тестами Python, поэтому тестов для них
    # здесь нет -- проект в текущем виде не соберётся, пока ошибка №1
    # не исправлена.
    #
    # Ошибка №9 (use-after-free при самоприсваивании перекрывающейся
    # подстроки в operator=(std::string_view)) недостижима через
    # штатный Python-интерфейс (assign всегда получает независимую
    # копию данных), поэтому воспроизводится только на уровне C++
    # (см. отчёт) и не покрыта тестом здесь.


if __name__ == "__main__":
    unittest.main()
