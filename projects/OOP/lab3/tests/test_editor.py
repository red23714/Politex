from pathlib import Path

import pytest

import my_string as ms

from command_registry import CommandRegistry
from config import Config
from controller import Controller
from curses_adapter import CursesAdapter
from history import History
from mode_type import Mode
from model import Model
from proccess_input import ProcessInput
from viewport import Viewport
from view import View

from app import Api

CONFIG_PATH = Path(__file__).resolve().parent.parent / "config.mim"

@pytest.fixture(autouse=True)
def clean_history():
    History().clear()
    yield
    History().clear()

@pytest.fixture
def editor():
    model = Model()
    model.load_from_string(
        ms.MyString("first line\nsecond target line\nthird line"))
    viewport = Viewport(height=10, width=80)
    api = Api(model, viewport)
    view = View(CursesAdapter(), viewport, api)
    api.attach_view(view)

    registry = CommandRegistry()
    registry.register_from_api(api)

    config = Config(str(CONFIG_PATH))
    controller = Controller(api, registry, ProcessInput(), config)
    controller.on_start(None)

    return model, api, controller

def press(controller, keys) -> None:
    for key in keys:
        controller.handle_key(key)
        controller.update()

def test_all_config_commands_are_registered(editor):
    model, api, controller = editor
    for command in controller.config.commands:
        for canonical, _ in command.steps:
            assert canonical in controller.registry, canonical

def test_normal_mode_navigation(editor):
    model, api, controller = editor
    press(controller, ["G"])
    assert model.cursor_row == 2
    press(controller, ["g", "g"])
    assert (model.cursor_row, model.cursor_col) == (0, 0)
    press(controller, ["$"])
    assert model.cursor_col == len("first line")
    press(controller, ["^"])
    assert model.cursor_col == 0
    press(controller, ["2", "G"])
    assert model.cursor_row == 1

def test_word_motion(editor):
    model, api, controller = editor
    press(controller, ["w"])
    assert model.cursor_col == 4
    press(controller, ["b"])
    assert model.cursor_col == 0

def test_delete_char_and_undo(editor):
    model, api, controller = editor
    press(controller, ["x"])
    assert model.line_text(0) == "irst line"
    press(controller, ["u"])
    assert model.line_text(0) == "first line"

def test_delete_line_put_and_undo(editor):
    model, api, controller = editor
    press(controller, ["d", "d"])
    assert model.line_text(0) == "second target line"
    press(controller, ["p"])
    assert model.line_text(1) == "first line"
    press(controller, ["u"])
    assert model.line_text(1) == "third line"
    press(controller, ["u"])
    assert model.line_text(0) == "first line"

def test_delete_word(editor):
    model, api, controller = editor
    press(controller, ["d", "i", "w"])
    assert model.line_text(0) == " line"

def test_copy_word_and_paste(editor):
    model, api, controller = editor
    press(controller, ["y", "w"])
    press(controller, ["G"])
    press(controller, ["p"])
    assert model.line_text(2) == "third linefirst"

def test_undo_line(editor):
    model, api, controller = editor
    press(controller, ["A"])
    press(controller, ["!", "!"])
    press(controller, ["ESC"])
    assert model.line_text(0) == "first line!!"
    press(controller, ["U"])
    assert model.line_text(0) == "first line"

def test_line_numbering_toggle(editor):
    model, api, controller = editor
    press(controller, [":", "s", "e", "t", " ", "n", "u", "m", "ENTER"])
    assert model.numbered is True
    press(controller, [":", "s", "e", "t", " ", "n", "u", "m", "ENTER"])
    assert model.numbered is False

def test_syntax_highlight_toggle(editor):
    from highlighter import SyntaxHighlightDecorator
    model, api, controller = editor
    press(controller, [":", "s", "y", "ENTER"])
    assert isinstance(api.view.renderer, SyntaxHighlightDecorator)
    press(controller, [":", "s", "y", "ENTER"])
    assert not isinstance(api.view.renderer, SyntaxHighlightDecorator)

def test_search_forward(editor):
    model, api, controller = editor
    press(controller, ["/", "t", "a", "r", "g", "e", "t", "ENTER"])
    assert (model.cursor_row, model.cursor_col) == (1, 7)
    press(controller, ["n"])
    assert (model.cursor_row, model.cursor_col) == (1, 7)

def test_search_backward(editor):
    model, api, controller = editor
    press(controller, ["G"])
    press(controller, ["?", "f", "i", "r", "s", "t", "ENTER"])
    assert model.cursor_row == 0

def test_search_not_found_message(editor):
    model, api, controller = editor
    press(controller, ["/", "z", "z", "z", "ENTER"])
    assert "не найден" in api.status_message

def test_goto_line_command_mode(editor):
    model, api, controller = editor
    press(controller, [":", "3", "ENTER"])
    assert model.cursor_row == 2

def test_quit_blocked_when_modified(editor):
    model, api, controller = editor

    class FakeApp:
        running = True

        def stop(self):
            self.running = False

    api.attach_app(FakeApp())
    press(controller, ["x"])
    press(controller, [":", "q", "ENTER"])
    assert api.app.running is True
    press(controller, [":", "q", "!", "ENTER"])
    assert api.app.running is False

def test_help_and_history_overlays(editor):
    model, api, controller = editor
    press(controller, [":", "h", "ENTER"])
    assert api.mode == Mode.HELP
    assert api.overlay_lines
    press(controller, ["ESC"])
    assert api.mode == Mode.NORMAL

    press(controller, ["d", "d"])
    press(controller, [":", "h", "i", "s", "t", "ENTER"])
    assert api.mode == Mode.HELP
    assert any("dd" in line for line in api.overlay_lines)
    press(controller, ["ESC"])

    press(controller, [":", "h", "i", "s", "t", " ",
                       "c", "l", "e", "a", "r", "ENTER"])
    assert len(History()) == 0
