from curses_adapter import CursesAdapter
from highlighter import (
    LineRenderer,
    PlainRenderer,
    SyntaxHighlightDecorator,
)


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


def test_plain_renderer_draws_single_segment():
    adapter = FakeAdapter()
    renderer = PlainRenderer()
    renderer.draw(adapter, 0, 0, "hello world")
    assert adapter.drawn == [(0, 0, "hello world", 0)]


def test_decorator_implements_component():
    renderer = SyntaxHighlightDecorator(PlainRenderer())
    assert isinstance(renderer, LineRenderer)


def test_highlight_keywords_and_numbers():
    adapter = FakeAdapter()
    renderer = SyntaxHighlightDecorator(PlainRenderer())
    renderer.draw(adapter, 0, 0, "x = 42")
    texts = [text for _, _, text, _ in adapter.drawn]
    assert "x" in texts
    assert "42" in texts
    number_attr = next(attr for _, _, text, attr in adapter.drawn
                       if text == "42")
    assert number_attr == CursesAdapter.COLOR_NUMBER + 10


def test_highlight_comment_to_end_of_line():
    adapter = FakeAdapter()
    renderer = SyntaxHighlightDecorator(PlainRenderer())
    segments = renderer.highlight(adapter, "code  # note")
    joined = "".join(text for text, _ in segments)
    assert joined == "code  # note"
    last_text, last_attr = segments[-1]
    assert last_text == "# note"
    assert last_attr == CursesAdapter.COLOR_COMMENT + 10


def test_highlight_string_literal():
    adapter = FakeAdapter()
    renderer = SyntaxHighlightDecorator(PlainRenderer())
    segments = renderer.highlight(adapter, 'name = "value"')
    string_segment = next(text for text, _ in segments if text == '"value"')
    assert string_segment == '"value"'
    string_attr = next(attr for text, attr in segments if text == '"value"')
    assert string_attr == CursesAdapter.COLOR_STRING


def test_unwrap_restores_plain_renderer():
    inner = PlainRenderer()
    decorated = SyntaxHighlightDecorator(inner)
    assert decorated.unwrap() is inner
