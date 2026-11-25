"""Подсветка синтаксиса — шаблон «декоратор» (доп. задание 4.2).

Базовый компонент LineRenderer просто рисует строку. Декоратор
SyntaxHighlightDecorator оборачивает любой LineRenderer и добавляет
поверх отрисовки разбиение строки на цветные сегменты. Декораторы
можно наслаивать: декоратор сам реализует интерфейс LineRenderer.
"""

import abc


class LineRenderer(abc.ABC):
    """Компонент: умеет нарисовать одну строку текста."""

    @abc.abstractmethod
    def draw(self, adapter, y: int, x: int, text: str) -> None:
        raise NotImplementedError


class PlainRenderer(LineRenderer):
    """Базовая отрисовка одним цветом."""

    def draw(self, adapter, y: int, x: int, text: str) -> None:
        adapter.draw_text(y, x, text)


class SyntaxHighlightDecorator(LineRenderer):
    """Декоратор: добавляет подсветку синтаксиса к отрисовщику.

    Разбирает строку как код на Python: ключевые слова, встроенные
    функции, числа, строковые литералы и комментарии.
    """

    KEYWORDS = {
        "def", "class", "import", "from", "as", "return", "if", "elif",
        "else", "for", "while", "break", "continue", "pass", "with",
        "try", "except", "finally", "raise", "in", "not", "and", "or",
        "is", "lambda", "yield", "global", "nonlocal", "assert", "del",
        "async", "await", "True", "False", "None",
    }
    BUILTINS = {
        "print", "len", "range", "str", "int", "float", "list", "dict",
        "set", "tuple", "open", "input", "enumerate", "zip", "map",
        "filter", "sum", "min", "max", "abs", "type", "isinstance",
    }

    def __init__(self, inner: LineRenderer) -> None:
        self.inner = inner

    def unwrap(self) -> LineRenderer:
        """Убирает декоратор (для переключения подсветки)."""
        return self.inner

    def draw(self, adapter, y: int, x: int, text: str) -> None:
        adapter.draw_segments(y, x, self.highlight(adapter, text))

    def highlight(self, adapter, text: str) -> list:
        segments = []
        i = 0
        n = len(text)

        while i < n:
            ch = text[i]

            if ch == "#":
                segments.append((text[i:], adapter.color_attr(
                    adapter.COLOR_COMMENT, bold=True)))
                break

            if ch in "\"'":
                end = self._string_end(text, i)
                segments.append((text[i:end], adapter.color_attr(
                    adapter.COLOR_STRING)))
                i = end
                continue

            if ch.isdigit():
                j = i
                while j < n and (text[j].isalnum() or text[j] == "_"):
                    j += 1
                segments.append((text[i:j], adapter.color_attr(
                    adapter.COLOR_NUMBER, bold=True)))
                i = j
                continue

            if ch.isalpha() or ch == "_":
                j = i
                while j < n and (text[j].isalnum() or text[j] == "_"):
                    j += 1
                word = text[i:j]
                if word in self.KEYWORDS:
                    attr = adapter.color_attr(adapter.COLOR_KEYWORD, bold=True)
                elif word in self.BUILTINS:
                    attr = adapter.color_attr(adapter.COLOR_BUILTIN)
                else:
                    attr = adapter.color_attr(adapter.COLOR_NORMAL)
                segments.append((word, attr))
                i = j
                continue

            j = i
            while j < n and not (text[j].isalpha() or text[j] == "_"
                                 or text[j].isdigit()
                                 or text[j] in "#\"'"):
                j += 1
            segments.append((text[i:j], adapter.color_attr(
                adapter.COLOR_NORMAL)))
            i = j

        return segments

    @staticmethod
    def _string_end(text: str, start: int) -> int:
        quote = text[start]
        i = start + 1
        while i < len(text):
            if text[i] == "\\":
                i += 2
                continue
            if text[i] == quote:
                return i + 1
            i += 1
        return len(text)
