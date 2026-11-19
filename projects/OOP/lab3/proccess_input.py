import re
import time

from command import Command


class ProcessInput:
    """Обработка ввода клавиш в основном режиме.

    Накапливает буфер клавиш и пытается сопоставить его с командами
    из Config. Составные последовательности (gg, dd, 10G, diw) требуют
    нескольких клавиш: пока буфер может совпасть с какой-то командой,
    ввод копится. Таймер (timeout) нужен, чтобы различать простые и
    составные команды: если пользователь остановился и буфер не совпал
    ни с одной командой — буфер сбрасывается.
    """

    DEFAULT_TIMEOUT = 1.0

    def __init__(self, timeout: float = DEFAULT_TIMEOUT) -> None:
        self.buffer = ""
        self.timer_start: float | None = None
        self.timeout = timeout

    def reset(self) -> None:
        self.buffer = ""
        self.timer_start = None

    def is_timed_out(self) -> bool:
        if self.timer_start is None:
            return False
        return time.monotonic() - self.timer_start > self.timeout

    def feed(self, key, commands):
        """Добавляет клавишу и возвращает Command, если совпадение полное."""
        if self.buffer.isdigit() and key.isdigit():
            self.buffer += key
            return None

        if self.buffer.isdigit():
            command = self.match_exact(commands, self.buffer + key)
            self.reset()
            if command is not None:
                return command
            return self.feed(key, commands)

        if self.is_timed_out():
            self.reset()

        self.buffer += key
        self.timer_start = time.monotonic()

        command = self.match_exact(commands, self.buffer)
        if command is not None:
            self.reset()
            return command

        if self.can_still_match(commands, self.buffer) or self.buffer.isdigit():
            return None

        self.reset()
        return None


    @staticmethod
    def match_exact(commands, text):
        """Ищет команду, чей шаблон ключей совпадает с текстом целиком."""
        for command in commands:
            if command.keys == text:
                return command
            if "@" in command.keys:
                match = ProcessInput.full_regex(command.keys).match(text)
                if match is not None:
                    return ProcessInput.with_captured(command, match)
        return None

    @staticmethod
    def full_regex(keys: str):
        """Превращает шаблон ключей в регулярное выражение:
        @num@ -> (\\d+), @text@ -> (.+)."""
        pattern = re.escape(keys)
        pattern = pattern.replace(re.escape("@num@"), r"(\d+)")
        pattern = pattern.replace(re.escape("@text@"), r"(.+)")
        return re.compile(f"^{pattern}$")

    @staticmethod
    def with_captured(command: Command, match) -> Command:
        """Подставляет захваченные @num@/@text@ в копию команды."""
        placeholders = re.findall(r"@num@|@text@", command.keys)
        captured = {}
        for index, token in enumerate(placeholders, start=1):
            value = match.group(index)
            captured[token] = int(value) if token == "@num@" else value

        if not captured:
            return command
        return Command(command.keys, command.is_sequence, command.steps,
                       command.mode, captured=captured,
                       description=command.description)

    @staticmethod
    def can_still_match(commands, text) -> bool:
        """Может ли буфер превратиться в команду при следующих клавишах?"""
        for command in commands:
            if command.keys.startswith(text):
                return True
            if "@" in command.keys:
                if command.keys.startswith("@num@") and text.isdigit():
                    return True
                fixed = command.keys.replace("@num@", "").replace("@text@", "")
                if fixed and fixed.startswith(text):
                    return True
        return False
