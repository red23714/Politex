from viewport import Viewport

class FakeModel:
    """Минимальная заглушка Model для проверки прокрутки."""

    def __init__(self, lines: list, cursor: int = 0) -> None:
        self.lines = lines
        self.cursor_row = cursor

    @property
    def line_count(self) -> int:
        return len(self.lines)

    def line_text(self, row: int) -> str:
        return self.lines[row]

def test_visible_rows_only_current_window():
    model = FakeModel([f"line {i}" for i in range(100)], cursor=0)
    viewport = Viewport(height=10, width=80)
    rows = viewport.visible_rows(model)
    assert [row for row, _ in rows] == list(range(10))

def test_cursor_below_scrolls_down():
    model = FakeModel([f"line {i}" for i in range(100)], cursor=25)
    viewport = Viewport(height=10, width=80)
    rows = viewport.visible_rows(model)
    assert viewport.top_row == 16
    assert [row for row, _ in rows] == list(range(16, 26))

def test_cursor_above_scrolls_up():
    model = FakeModel([f"line {i}" for i in range(100)], cursor=5)
    viewport = Viewport(height=10, width=80)
    viewport.top_row = 30
    rows = viewport.visible_rows(model)
    assert viewport.top_row == 5
    assert [row for row, _ in rows] == list(range(5, 15))

def test_page_step_equals_height():
    viewport = Viewport(height=10, width=80)
    assert viewport.page_up() == 10
    assert viewport.page_down() == 10
