from curses_adapter import CursesAdapter
from highlighter import (
    LineRenderer,
    PlainRenderer,
    SyntaxHighlightDecorator,
)
from view import View


class FakeAdapter(CursesAdapter):
    def __init__(self) -> None:
        super().__init__()
        self.drawn = []

    def init(self) -> None:
        self._color_pairs_ready = False

    def color_attr(self, color: int, bold: bool = False) -> int:
        return color + (10 if bold else 0)

    def draw_text(self, y, x, text, attr=0) -> None:
        self.drawn.append((y, x, text, attr))

    def draw_segments(self, y, x, segments) -> None:
        offset = x
        for text, attr in segments:
            self.draw_text(y, offset, text, attr)
            offset += len(text)


def make_renderer():
    adapter = FakeAdapter()
    renderer = SyntaxHighlightDecorator(PlainRenderer())
    return adapter, renderer


def colors_of(adapter, row):
    return sorted({attr for (y, x, text, attr) in adapter.drawn if y == row})


def test_decorator_implements_component():
    renderer = SyntaxHighlightDecorator(PlainRenderer())
    assert isinstance(renderer, LineRenderer)


def test_plain_renderer_draws_single_segment():
    adapter, renderer = make_renderer()
    renderer.inner.draw(adapter, 0, 0, "hello world")
    assert adapter.drawn == [(0, 0, "hello world", 0)]


def test_highlight_keywords_and_numbers():
    adapter, renderer = make_renderer()
    renderer.draw(adapter, 0, 0, "x = 42")
    texts = [text for _, _, text, _ in adapter.drawn]
    assert "x" in texts
    assert "42" in texts
    number_attr = next(attr for _, _, text, attr in adapter.drawn
                       if text == "42")
    assert number_attr == CursesAdapter.COLOR_NUMBER + 10


def test_highlight_comment_to_end_of_line():
    adapter, renderer = make_renderer()
    segments = renderer.highlight(adapter, "code  # note")
    joined = "".join(text for text, _ in segments)
    assert joined == "code  # note"
    last_text, last_attr = segments[-1]
    assert last_text == "# note"
    assert last_attr == CursesAdapter.COLOR_COMMENT + 10


def test_highlight_string_literal():
    adapter, renderer = make_renderer()
    segments = renderer.highlight(adapter, 'name = "value"')
    string_attr = next(attr for text, attr in segments if text == '"value"')
    assert string_attr == CursesAdapter.COLOR_STRING


def test_multiline_docstring_all_lines_colored():
    adapter, renderer = make_renderer()
    lines = [
        'class Api:',
        '    """Фасад над Model: именно его методы',
        '    «реальными функциями», на которые Registry',
        '    """',
        '    def f(self):',
        '        return 1',
    ]
    for i, line in enumerate(lines):
        renderer.draw(adapter, i, 0, line)

    assert CursesAdapter.COLOR_KEYWORD + 10 in colors_of(adapter, 0)
    assert CursesAdapter.COLOR_STRING in colors_of(adapter, 1)
    assert CursesAdapter.COLOR_STRING in colors_of(adapter, 2)
    assert CursesAdapter.COLOR_STRING in colors_of(adapter, 3)
    assert CursesAdapter.COLOR_KEYWORD + 10 in colors_of(adapter, 4)


def test_multiline_string_reset_between_draws():
    adapter, renderer = make_renderer()
    renderer.draw(adapter, 0, 0, 's = """незакрытый')
    renderer.reset()
    renderer.draw(adapter, 1, 0, "обычный текст # комментарий")
    assert CursesAdapter.COLOR_STRING not in colors_of(adapter, 1)
    assert CursesAdapter.COLOR_COMMENT + 10 in colors_of(adapter, 1)


def test_cyrillic_single_line_string():
    adapter, renderer = make_renderer()
    segments = renderer.highlight(adapter, 'x = "привет"  # комментарий')
    string_seg = next((text, attr) for text, attr in segments
                       if "привет" in text)
    assert string_seg[1] == CursesAdapter.COLOR_STRING


def test_unwrap_restores_plain_renderer():
    inner = PlainRenderer()
    decorated = SyntaxHighlightDecorator(inner)
    assert decorated.unwrap() is inner


def test_view_decode_line_converts_file_bytes():
    """Строки файла хранятся побайтово (latin-1): UTF-8 кириллица должна
    декодироваться при отрисовке; строки, введённые с клавиатуры (уже
    Unicode), не изменяются."""
    file_line = "текст".encode("utf-8").decode("latin-1")
    assert View.decode_line(file_line) == "текст"
    assert View.decode_line("уже юникод") == "уже юникод"
    assert View.decode_line("plain ascii") == "plain ascii"
    assert View.decode_line('x = "значение"  # коммент') == \
        'x = "значение"  # коммент'
