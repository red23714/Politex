import my_string as ms

from model import Model

def make_model(text: str) -> Model:
    model = Model()
    model.load_from_string(ms.MyString(text))
    return model

def test_stores_lines_without_newline():
    model = make_model("first line\nsecond target line\nthird line")
    assert model.line_count == 3
    assert model.line_text(0) == "first line"
    assert model.line_text(1) == "second target line"
    assert model.line_text(2) == "third line"

def test_trailing_newline_creates_empty_line():
    model = make_model("a\n")
    assert model.line_count == 2
    assert model.line_text(1) == ""

def test_to_string_roundtrip():
    text = "alpha\nbeta\ngamma"
    model = make_model(text)
    assert model.to_string().c_str() == text

def test_move_cursor_clamps():
    model = make_model("abc\nde")
    model.move_cursor(0, 10)
    assert (model.cursor_row, model.cursor_col) == (0, 3)
    model.move_cursor(100, 0)
    assert model.cursor_row == 1

def test_insert_text_single_line():
    model = make_model("hello world")
    model.move_cursor(0, 5)
    model.insert_text(",")
    assert model.line_text(0) == "hello, world"
    assert model.cursor_col == 6
    assert model.modified

def test_insert_text_with_newlines():
    model = make_model("first line\nsecond line")
    model.move_cursor(0, 5)
    model.insert_text("X\nnew\n")
    assert model.line_count == 4
    assert model.line_text(0) == "firstX"
    assert model.line_text(1) == "new"
    assert model.line_text(2) == " line"
    assert (model.cursor_row, model.cursor_col) == (2, 0)

def test_delete_range_inside_line():
    model = make_model("hello world")
    model.delete_range((0, 5), (0, 11))
    assert model.line_text(0) == "hello"

def test_delete_range_across_lines():
    model = make_model("aaa\nbbb\nccc")
    model.delete_range((0, 1), (2, 1))
    assert model.line_count == 1
    assert model.line_text(0) == "acc"

def test_delete_line_keeps_empty_file_usable():
    model = make_model("only line")
    text = model.delete_line(0)
    assert text == "only line"
    assert model.line_count == 1
    assert model.line_text(0) == ""

def test_find_next_and_prev():
    model = make_model("one two one\nthree one")
    model.move_cursor(0, 0)
    assert model.find_next("one") is True
    assert (model.cursor_row, model.cursor_col) == (0, 0)
    model.move_cursor(0, 1)
    assert model.find_next("one") is True
    assert (model.cursor_row, model.cursor_col) == (0, 8)
    assert model.find_next("missing") is False

    model.move_cursor(0, 8)
    assert model.find_prev("two") is True
    assert (model.cursor_row, model.cursor_col) == (0, 4)

def test_word_navigation():
    model = make_model("alpha beta gamma")
    model.move_cursor(0, 0)
    model.word_forward()
    assert model.cursor_col == 4
    model.word_forward()
    assert model.cursor_col == 9
    model.word_backward()
    assert model.cursor_col == 6

def test_word_span():
    model = make_model("hello world")
    model.move_cursor(0, 1)
    assert model.word_span() == (0, 5)

def test_snapshot_restore():
    model = make_model("aaa\nbbb")
    model.move_cursor(0, 1)
    snap = model.snapshot()
    model.insert_text("XYZ")
    assert model.line_text(0) == "aXYZaa"
    model.restore_snapshot(snap)
    assert model.line_text(0) == "aaa"
    assert (model.cursor_row, model.cursor_col) == (0, 1)

def test_restore_original_line():
    model = make_model("original")
    model.move_cursor(0, 0)
    model.insert_text("x")
    model.insert_text("y")
    assert model.line_text(0) == "xyoriginal"
    assert model.restore_original_line() is True
    assert model.line_text(0) == "original"
    assert model.restore_original_line() is False

def test_open_and_save_file(tmp_path):
    path = tmp_path / "sample.txt"
    path.write_bytes(b"line one\nline two")

    model = Model()
    model.open_file(str(path))
    assert model.line_count == 2
    assert model.modified is False

    model.move_cursor(0, model.get_line(0).size())
    model.insert_text(" edited")
    assert model.modified is True

    model.save()
    assert path.read_bytes() == b"line one edited\nline two"
    assert model.modified is False

def test_clipboard_operations():
    model = make_model("copy me\ntarget")
    model.clipboard = "copy me\n"
    model.insert_line(1, model.clipboard[:-1])
    assert model.line_text(1) == "copy me"
