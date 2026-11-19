class Viewport:
    """Видимая часть экрана.

    Хранит номер верхней видимой строки и следит за тем, чтобы курсор
    оставался в видимой области. Отдаёт View только те строки, которые
    нужно рисовать.
    """

    def __init__(self, height: int = 24, width: int = 80) -> None:
        self.height = height
        self.width = width
        self.top_row = 0

    def resize(self, height: int, width: int) -> None:
        self.height = max(height, 1)
        self.width = max(width, 1)
        self.top_row = min(self.top_row, max(self.height - 1, 0))

    def update(self, model) -> None:
        """Прокручивает окно так, чтобы строка с курсором была видна."""
        if self.top_row > model.cursor_row:
            self.top_row = model.cursor_row
        if model.cursor_row >= self.top_row + self.height:
            self.top_row = model.cursor_row - self.height + 1
        self.top_row = max(self.top_row, 0)

    def visible_rows(self, model) -> list:
        """Список (номер_строки_в_файле, текст) для отрисовки."""
        self.update(model)
        last = min(self.top_row + self.height, model.line_count)
        return [(row, model.line_text(row))
                for row in range(self.top_row, last)]

    def page_up(self) -> int:
        return self.height

    def page_down(self) -> int:
        return self.height

    def scroll(self, delta: int, line_count: int) -> None:
        self.top_row = max(0, min(self.top_row + delta,
                                  max(line_count - 1, 0)))
