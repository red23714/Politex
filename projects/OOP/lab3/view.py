from framework import FrameworkView
from highlighter import PlainRenderer, SyntaxHighlightDecorator
from mode_type import Mode


class View(FrameworkView):
    """Представление: рисует видимую область текста (через Viewport),
    строку состояния и окна помощи, ставит курсор."""

    STATUS_AREA = 1

    HELP_LINES = [
        "MyVim — справка по командам",
        "",
        "Навигация: стрелки, ^/$, w/b, gg, G, NG, PG_UP/PG_DOWN",
        "Редактирование: x, diw, dd, yy, yw, p, u, U",
        "Поиск: /text, ?text, n, N",
        "Ввод текста: i, I, A, S, r (замена символа)",
        "Режим команд (:): o file, w [file], x, q, q!, wq!, e!, number,",
        "               set num, sy, hist, hist clear, h",
        "",
        "ESC — вернуться в основной режим",
    ]

    def __init__(self, adapter, viewport, api) -> None:
        super().__init__(adapter)
        self.viewport = viewport
        self.api = api
        self.renderer = PlainRenderer()


    def toggle_syntax_highlight(self) -> None:
        if isinstance(self.renderer, SyntaxHighlightDecorator):
            self.renderer = self.renderer.unwrap()
        else:
            self.renderer = SyntaxHighlightDecorator(self.renderer)


    def get_key(self):
        return self.adapter.get_key(timeout_ms=100)

    def render(self) -> None:
        adapter = self.adapter
        adapter.clear()

        rows, cols = adapter.get_size()
        self.viewport.resize(rows - self.STATUS_AREA, cols)

        if self.api.mode == Mode.HELP:
            self.draw_overlay()
        else:
            self.draw_text_area()

        self.draw_status_bar()
        self.place_cursor()
        adapter.refresh()

    def draw_text_area(self) -> None:
        model = self.api.model
        rows = self.viewport.height
        numbered = model.numbered
        gutter = len(str(model.line_count)) + 1 if numbered else 0

        for y, (row_index, text) in enumerate(self.viewport.visible_rows(model)):
            x = 0
            if numbered:
                prefix = f"{row_index + 1:>{gutter}} "
                self.adapter.draw_text(y, 0, prefix)
                x = len(prefix)
            self.renderer.draw(self.adapter, y, x,
                               self.decode_line(text))

    @staticmethod
    def decode_line(text: str) -> str:
        """Файл хранится в модели побайтово (latin-1, 1 байт = 1 символ).
        Кириллица в UTF-8 занимает 2 байта, поэтому перед отрисовкой
        строка файла декодируется в настоящий Unicode. Строки, введённые
        пользователем с клавиатуры, уже Unicode — их не трогаем."""
        try:
            return text.encode("latin-1").decode("utf-8")
        except (UnicodeEncodeError, UnicodeDecodeError):
            return text

    def draw_overlay(self) -> None:
        lines = self.api.overlay_lines
        height = self.viewport.height
        for y, text in enumerate(lines[:height]):
            self.adapter.draw_text(y, 0, text)

    def draw_status_bar(self) -> None:
        model = self.api.model
        mode_name = self.api.mode.name if self.api.mode else "NORMAL"
        filename = model.filename or "[No Name]"
        if model.modified:
            filename += " [+]"
        status = (f" {mode_name} | {filename} | "
                  f"строка {model.cursor_row + 1} из {model.line_count}")
        if self.api.input_line is not None and self.api.input_prefix:
            status += f" | {self.api.input_prefix}{self.api.input_line}"
        elif self.api.status_message:
            status += f" | {self.api.status_message}"

        attr = self.adapter.color_attr(self.adapter.COLOR_STATUS, bold=True)
        self.adapter.draw_text(self.viewport.height, 0, status, attr)

    def place_cursor(self) -> None:
        model = self.api.model
        if self.api.mode in (Mode.NORMAL, Mode.INSERT, Mode.REPLACE):
            gutter = len(str(model.line_count)) + 1 if model.numbered else 0
            y = model.cursor_row - self.viewport.top_row
            self.adapter.move_cursor(y, gutter + model.cursor_col)
        else:
            self.adapter.move_cursor(self.viewport.height, 0)
