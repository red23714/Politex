from collections import deque

from command import Command
from framework import FrameworkController
from history import History
from mode_type import Mode
from proccess_input import ProcessInput


class Controller(FrameworkController):
    """Контроллер: маршрутизирует ввод по режимам и исполняет команды.

    Команды ставятся в очередь (deque) и исполняются в update(): это
    позволяет поддерживать составные команды из нескольких шагов.
    Перед исполнением изменяющей текст команды в историю (Singleton,
    доп. задание 4.3) кладётся снимок состояния Model для отмены.
    """

    def __init__(self, api, registry, process_input, config) -> None:
        self.api = api
        self.registry = registry
        self.process_input = process_input
        self.config = config
        self.normal_commands = config.commands_for_mode("normal")
        self.command_commands = config.commands_for_mode("command")
        self.queue = deque()


    def on_start(self, app=None) -> None:
        self.api.app = app
        self.api.set_mode(Mode.NORMAL)

    def on_timeout(self) -> None:
        if self.process_input.is_timed_out():
            self.process_input.reset()


    def handle_key(self, key) -> None:
        mode = self.api.mode

        if mode == Mode.NORMAL:
            self._handle_normal(key)
        elif mode == Mode.INSERT:
            self._handle_insert(key)
        elif mode == Mode.REPLACE:
            self._handle_replace(key)
        elif mode == Mode.COMMAND:
            self._handle_command_line(key)
        elif mode == Mode.SEARCH:
            self._handle_search_line(key)
        elif mode == Mode.HELP:
            self.api.close_help()

    def _handle_normal(self, key) -> None:
        if key == ":":
            self.process_input.reset()
            self.api.enter_command_mode()
            return
        if key in ("/", "?"):
            self.process_input.reset()
            self.api.enter_search_mode(key)
            return
        if key == "ESC":
            self.process_input.reset()
            return

        command = self.process_input.feed(key, self.normal_commands)
        if command is not None:
            self.enqueue(command)

    def _handle_insert(self, key) -> None:
        if key == "ESC":
            self.api.finish_insert()
        elif key == "ENTER":
            self.api.insert_text("\n")
        elif key == "BACKSPACE":
            self.api.backspace()
        elif key == "LEFT":
            self.api.move_cursor_left()
        elif key == "RIGHT":
            self.api.move_cursor_right()
        elif key == "UP":
            self.api.move_cursor_up()
        elif key == "DOWN":
            self.api.move_cursor_down()
        elif len(key) == 1 and key.isprintable():
            self.api.insert_text(key)

    def _handle_replace(self, key) -> None:
        if key == "ESC":
            self.api.set_mode(Mode.NORMAL)
        elif len(key) == 1 and key.isprintable():
            self.api.replace_char(key)
        else:
            self.api.set_mode(Mode.NORMAL)

    def _handle_command_line(self, key) -> None:
        if key == "ESC":
            self.api.cancel_input()
        elif key == "ENTER":
            line = self.api.input_line
            self.api.cancel_input()
            self._execute_command_line(line)
        elif key == "BACKSPACE":
            self.api.input_line = self.api.input_line[:-1]
        elif len(key) == 1 and key.isprintable():
            self.api.input_line += key

    def _execute_command_line(self, line) -> None:
        line = line.strip()
        if not line:
            return

        command = ProcessInput.match_exact(self.command_commands, line)
        if command is None:
            self.api.show_message(f"Не команда редактора: {line}")
            return
        self.enqueue(command)

    def _handle_search_line(self, key) -> None:
        if key == "ESC":
            self.api.cancel_input()
        elif key == "ENTER":
            self._finish_search()
        elif key == "BACKSPACE":
            self.api.input_line = self.api.input_line[:-1]
        elif len(key) == 1 and key.isprintable():
            self.api.input_line += key

    def _finish_search(self) -> None:
        text = self.api.input_line
        prefix = self.api.input_prefix
        self.api.cancel_input()

        canonical = "search start" if prefix == "/" else "search end"
        command = Command(prefix, True, [(canonical, ["@text@"])],
                          captured={"@text@": text})
        self._run(command)


    def enqueue(self, command) -> None:
        self.queue.append(command)

    def update(self) -> None:
        while self.queue:
            command = self.queue.popleft()
            self._run(command)

    def _run(self, command) -> None:
        try:
            if any(self.registry.is_undoable(canonical)
                   for canonical, _ in command.steps):
                snapshot = self.api.model.snapshot()
                command.execute(self.api, self.registry)
                History().push(command, snapshot)
            else:
                command.execute(self.api, self.registry)
        except KeyError as error:
            self.api.show_message(str(error))
        except (ValueError, OverflowError, OSError) as error:
            self.api.show_message(f"Ошибка: {error}")
