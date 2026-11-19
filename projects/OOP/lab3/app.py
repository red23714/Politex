import sys

from command import Command
from command_registry import CommandRegistry
from config import Config
from controller import Controller
from curses_adapter import CursesAdapter
from framework import AppBuilder
from history import History
from mode_type import Mode
from model import Model
from proccess_input import ProcessInput
from viewport import Viewport
from view import View

import my_string as ms


class Api:
    """Фасад над Model / Viewport / View: именно его методы являются
    «реальными функциями», на которые CommandRegistry переводит команды.

    Registry читает registry_table() и регистрирует соответствие
    «каноническое имя команды -> метод Api».
    """

    def __init__(self, model: Model, viewport: Viewport) -> None:
        self.model = model
        self.viewport = viewport
        self.view = None
        self.app = None

        self.mode = Mode.NORMAL
        self.status_message = ""
        self.input_line = ""
        self.input_prefix = ""
        self.overlay_lines: list[str] = []
        self.last_search = ""
        self._insert_snapshot = None

    def attach_view(self, view) -> None:
        self.view = view

    def attach_app(self, app) -> None:
        self.app = app

    def registry_table(self) -> list:
        """(каноническое имя, имя метода Api, изменяет_ли_текст)."""
        return [
            ("go pos right", "move_cursor_right", False),
            ("go pos left", "move_cursor_left", False),
            ("go pos up", "move_cursor_up", False),
            ("go pos down", "move_cursor_down", False),
            ("go str start", "move_line_start", False),
            ("go str end", "move_line_end", False),
            ("go word end", "move_word_forward", False),
            ("go word start", "move_word_backward", False),
            ("go start", "move_file_start", False),
            ("go end", "move_file_end", False),
            ("go line", "move_to_line", False),
            ("go screen up", "page_up", False),
            ("go screen down", "page_down", False),
            ("del", "delete_char", True),
            ("del word space", "delete_word", True),
            ("del line", "delete_line", True),
            ("cpy line", "copy_line", False),
            ("cpy word", "copy_word", False),
            ("pst", "paste", True),
            ("search start", "search_forward", False),
            ("search end", "search_backward", False),
            ("search rep end", "search_repeat_forward", False),
            ("search rep start", "search_repeat_backward", False),
            ("insert", "enter_insert_mode", False),
            ("insert before", "enter_insert_mode", False),
            ("replace", "enter_replace_mode", False),
            ("open", "open_file", False),
            ("write", "save_file", False),
            ("save", "save_file", False),
            ("exit", "quit", False),
            ("exit force", "quit_force", False),
            ("revert", "revert", False),
            ("undo", "undo", False),
            ("undo line", "undo_line", False),
            ("numbering toggle", "toggle_line_numbers", False),
            ("highlight toggle", "toggle_syntax_highlight", False),
            ("history show", "show_history", False),
            ("history clear", "clear_history", False),
            ("help", "show_help", False),
        ]


    def set_mode(self, mode: Mode) -> None:
        self.mode = mode

    def show_message(self, text: str) -> None:
        self.status_message = text

    def enter_insert_mode(self) -> None:
        self._insert_snapshot = self.model.snapshot()
        self.set_mode(Mode.INSERT)

    def finish_insert(self) -> None:
        if self._insert_snapshot is not None:
            command = Command("<ввод текста>", True, [])
            History().push(command, self._insert_snapshot)
            self._insert_snapshot = None
        self.set_mode(Mode.NORMAL)

    def enter_replace_mode(self) -> None:
        self.set_mode(Mode.REPLACE)

    def enter_command_mode(self) -> None:
        self.input_prefix = ":"
        self.input_line = ""
        self.set_mode(Mode.COMMAND)

    def enter_search_mode(self, prefix: str) -> None:
        self.input_prefix = prefix
        self.input_line = ""
        self.set_mode(Mode.SEARCH)

    def cancel_input(self) -> None:
        self.input_line = ""
        self.input_prefix = ""
        self.set_mode(Mode.NORMAL)

    def close_help(self) -> None:
        self.overlay_lines = []
        self.set_mode(Mode.NORMAL)


    def move_cursor_right(self, count: int = 1) -> None:
        row, col = self.model.cursor_row, self.model.cursor_col + count
        self.model.move_cursor(row, col)

    def move_cursor_left(self, count: int = 1) -> None:
        row, col = self.model.cursor_row, self.model.cursor_col - count
        self.model.move_cursor(row, col)

    def move_cursor_up(self, count: int = 1) -> None:
        self.model.move_cursor(self.model.cursor_row - count,
                               self.model.cursor_col)

    def move_cursor_down(self, count: int = 1) -> None:
        self.model.move_cursor(self.model.cursor_row + count,
                               self.model.cursor_col)

    def move_line_start(self) -> None:
        self.model.move_cursor(self.model.cursor_row, 0)

    def move_line_end(self) -> None:
        self.model.move_cursor(self.model.cursor_row,
                               self.model.get_current_line().size())

    def move_word_forward(self) -> None:
        self.model.word_forward()

    def move_word_backward(self) -> None:
        self.model.word_backward()

    def move_file_start(self) -> None:
        self.model.move_cursor(0, 0)

    def move_file_end(self) -> None:
        self.model.move_cursor(self.model.line_count - 1,
                               self.model.get_current_line().size())

    def move_to_line(self, number: int) -> None:
        row = max(0, min(number - 1, self.model.line_count - 1))
        self.model.move_cursor(row, self.model.cursor_col)

    def page_up(self) -> None:
        self.model.move_cursor(self.model.cursor_row - self.viewport.page_up(),
                               self.model.cursor_col)

    def page_down(self) -> None:
        self.model.move_cursor(self.model.cursor_row + self.viewport.page_down(),
                               self.model.cursor_col)


    def delete_char(self) -> None:
        row, col = self.model.cursor_row, self.model.cursor_col
        if col < self.model.get_current_line().size():
            self.model.delete_range((row, col), (row, col + 1))

    def delete_word(self) -> None:
        start, end = self.model.word_span()
        if end > start:
            self.model.delete_range(
                (self.model.cursor_row, start), (self.model.cursor_row, end))

    def delete_line(self) -> None:
        text = self.model.delete_line(self.model.cursor_row)
        self.model.clipboard = text + "\n"

    def copy_line(self) -> None:
        self.model.clipboard = self.model.line_text(self.model.cursor_row) + "\n"

    def copy_word(self) -> None:
        start, end = self.model.word_span()
        self.model.clipboard = self.model.line_text(self.model.cursor_row)[start:end]

    def paste(self) -> None:
        clipboard = self.model.clipboard
        if not clipboard:
            return
        if clipboard.endswith("\n"):
            self.model.insert_line(self.model.cursor_row + 1,
                                   clipboard[:-1])
            self.model.move_cursor(self.model.cursor_row + 1, 0)
            self.model.modified = True
        else:
            col = self.model.cursor_col
            if col < self.model.get_current_line().size():
                col += 1
            self.model.move_cursor(self.model.cursor_row, col)
            self.model.insert_text(clipboard)

    def insert_text(self, text: str) -> None:
        self.model.insert_text(text)

    def backspace(self) -> None:
        row, col = self.model.cursor_row, self.model.cursor_col
        if col > 0:
            self.model.delete_range((row, col - 1), (row, col))
        elif row > 0:
            prev_size = self.model.get_line(row - 1).size()
            self.model.delete_range((row - 1, prev_size), (row, 0))
            self.model.move_cursor(row - 1, prev_size)

    def replace_char(self, ch: str) -> None:
        row, col = self.model.cursor_row, self.model.cursor_col
        if col < self.model.get_current_line().size():
            snapshot = self.model.snapshot()
            self.model.delete_range((row, col), (row, col + 1))
            self.model.insert_text(ch)
            command = Command(f"r{ch}", False, [("replace", [ch])])
            History().push(command, snapshot)
        self.set_mode(Mode.NORMAL)


    def search_forward(self, text: str) -> None:
        self._search(text, forward=True)

    def search_backward(self, text: str) -> None:
        self._search(text, forward=False)

    def _search(self, text: str, forward: bool) -> None:
        if not text:
            return
        self.last_search = text
        found = (self.model.find_next(text) if forward
                 else self.model.find_prev(text))
        if not found:
            self.show_message(f"Шаблон не найден: {text}")

    def search_repeat_forward(self) -> None:
        self._search(self.last_search, forward=True)

    def search_repeat_backward(self) -> None:
        self._search(self.last_search, forward=False)


    def open_file(self, filename: str) -> None:
        self.model.open_file(filename)
        self.viewport.top_row = 0
        self.show_message(f"Открыт файл: {filename}")

    def save_file(self, filename: str | None = None) -> None:
        self.model.save(filename)
        self.show_message(f"Записан файл: {self.model.filename}")

    def quit(self, force: bool = False) -> None:
        if self.model.modified and not force:
            self.show_message("E37: Изменения не записаны (используйте q!)")
            return
        if self.app is not None:
            self.app.stop()

    def quit_force(self) -> None:
        self.quit(force=True)

    def revert(self) -> None:
        if not self.model.filename:
            self.show_message("E32: Нет имени файла")
            return
        self.model.open_file(self.model.filename)
        self.viewport.top_row = 0
        self.show_message("Все изменения отменены")


    def undo(self) -> None:
        record = History().pop()
        if record is None:
            self.show_message("Уже в самой ранней версии файла")
            return
        self.model.restore_snapshot(record[1])

    def undo_line(self) -> None:
        if not self.model.restore_original_line():
            self.show_message("Строка не изменялась")


    def toggle_line_numbers(self) -> None:
        """4.1: нумерация строк (set num)."""
        self.model.numbered = not self.model.numbered

    def toggle_syntax_highlight(self) -> None:
        """4.2: подсветка синтаксиса (sy), шаблон «декоратор»."""
        if self.view is None:
            return
        self.view.toggle_syntax_highlight()

    def show_history(self) -> None:
        """4.3: показать историю команд (hist), шаблон «одиночка»."""
        lines = History().describe()
        if not lines:
            lines = ["История команд пуста"]
        self.overlay_lines = ["История команд:"] + [
            f"{index + 1:>3}. {text}"
            for index, text in enumerate(lines[-20:])
        ]
        self.set_mode(Mode.HELP)

    def clear_history(self) -> None:
        History().clear()
        self.show_message("История команд очищена")

    def show_help(self) -> None:
        self.overlay_lines = list(View.HELP_LINES)
        self.set_mode(Mode.HELP)


def build_app(filename: str | None = None,
              config_path: str = "config.mim"):
    """Собирает приложение через шаблон «строитель» (доп. задание 4.4)."""
    config = Config(config_path)

    registry = CommandRegistry()
    process_input = ProcessInput()
    viewport = Viewport()

    model = Model(filename=filename)
    if filename:
        model.open_file(filename)
    else:
        model.load_from_string(ms.MyString(""))

    adapter = CursesAdapter()
    api = Api(model, viewport)
    view = View(adapter, viewport, api)
    api.attach_view(view)

    registry.register_from_api(api)

    controller = Controller(api, registry, process_input, config)

    app = (AppBuilder()
           .set_adapter(adapter)
           .set_model(model)
           .set_view(view)
           .set_controller(controller)
           .set_on_start(controller.on_start)
           .set_title("MyVim")
           .build())
    api.attach_app(app)
    return app


if __name__ == "__main__":
    editor = build_app(sys.argv[1] if len(sys.argv) > 1 else None)
    editor.run()
