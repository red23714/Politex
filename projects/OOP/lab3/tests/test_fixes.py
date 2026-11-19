import time

import my_string as ms

from command import Command
from model import Model
from proccess_input import ProcessInput

def make_model(text: str) -> Model:
    model = Model()
    model.load_from_string(ms.MyString(text))
    return model

def test_rfind_on_empty_lines():
    """Поиск назад не должен падать на пустых строках (rfind без индекса
    в MyString бросает IndexError, т.к. внутри вызывается len_ - 1)."""
    model = make_model("alpha\n\nbeta\n\nalpha end")
    model.move_cursor(model.line_count - 1, 5)
    assert model.find_prev("alpha") is True
    assert (model.cursor_row, model.cursor_col) == (4, 0)

    model.move_cursor(0, 2)
    assert model.find_prev("beta") is True
    assert (model.cursor_row, model.cursor_col) == (2, 0)

    model.move_cursor(0, 0)
    assert model.find_prev("zzz") is False

def test_goto_line_clamps_to_file_bounds():
    """12G на файле из 3 строк — переход на последнюю строку, без ошибки."""
    model = make_model("one\ntwo\nthree")
    model.move_cursor(0, 0)

    from app import Api
    api = Api(model, __import__("viewport").Viewport())
    api.move_to_line(12)
    assert model.cursor_row == 2
    api.move_to_line(0)
    assert model.cursor_row == 0
    api.move_to_line(2)
    assert model.cursor_row == 1

def test_count_survives_slow_typing(monkeypatch):
    """Цифры счётчика не должны теряться по таймауту: 12G набирается
    с любыми паузами между клавишами."""
    commands = [Command("@num@G", True, [("go line", ["@num@"])])]

    pi = ProcessInput(timeout=0.1)
    pi.feed("1", commands)

    real_monotonic = time.monotonic
    monkeypatch.setattr(time, "monotonic",
                        lambda: real_monotonic() + 5.0)
    assert pi.feed("2", commands) is None
    command = pi.feed("G", commands)
    assert command is not None
    assert command.captured == {"@num@": 12}

def test_count_then_other_command():
    """12x: счётчик не поддерживается остальными командами —
    выполняется сама команда без счётчика (x)."""
    commands = [Command("@num@G", True, [("go line", ["@num@"])]),
                Command("x", False, [("del", [])])]

    pi = ProcessInput()
    assert pi.feed("1", commands) is None
    assert pi.feed("2", commands) is None
    command = pi.feed("x", commands)
    assert command is not None
    assert command.steps == [("del", [])]

def test_timer_measures_from_last_key(monkeypatch):
    """Таймаут считается от последней клавиши, а не от первой."""
    commands = [Command("gg", True, [("go start", [])])]
    pi = ProcessInput(timeout=1.0)

    real_monotonic = time.monotonic
    monkeypatch.setattr(time, "monotonic", lambda: real_monotonic())
    assert pi.feed("g", commands) is None

    monkeypatch.setattr(time, "monotonic",
                        lambda: real_monotonic() + 0.5)
    assert pi.feed("g", commands) is not None
