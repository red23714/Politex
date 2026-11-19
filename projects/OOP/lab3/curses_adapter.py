import curses


class CursesAdapter:
    """Адаптер (шаблон «адаптер») для библиотеки curses.

    Прячет за собой низкоуровневые вызовы curses: инициализацию экрана,
    чтение клавиш, отрисовку текста и цветовые пары. Остальной код
    работает только с этим интерфейсом.
    """

    KEY_NAMES = {
        curses.KEY_RIGHT: "RIGHT",
        curses.KEY_LEFT: "LEFT",
        curses.KEY_UP: "UP",
        curses.KEY_DOWN: "DOWN",
        curses.KEY_PPAGE: "PG_UP",
        curses.KEY_NPAGE: "PG_DOWN",
    }

    COLOR_NORMAL = 0
    COLOR_KEYWORD = 1
    COLOR_STRING = 2
    COLOR_COMMENT = 3
    COLOR_NUMBER = 4
    COLOR_BUILTIN = 5
    COLOR_STATUS = 6

    def __init__(self) -> None:
        self.screen = None
        self._color_pairs_ready = False

    def init(self) -> None:
        self.screen = curses.initscr()
        curses.noecho()
        curses.cbreak()
        self.screen.keypad(True)
        curses.curs_set(1)

        curses.start_color()
        curses.use_default_colors()
        curses.init_pair(self.COLOR_KEYWORD, curses.COLOR_MAGENTA, -1)
        curses.init_pair(self.COLOR_STRING, curses.COLOR_GREEN, -1)
        curses.init_pair(self.COLOR_COMMENT, curses.COLOR_CYAN, -1)
        curses.init_pair(self.COLOR_NUMBER, curses.COLOR_YELLOW, -1)
        curses.init_pair(self.COLOR_BUILTIN, curses.COLOR_BLUE, -1)
        curses.init_pair(self.COLOR_STATUS, curses.COLOR_BLACK, curses.COLOR_WHITE)
        self._color_pairs_ready = True

    def destroy(self) -> None:
        if self.screen is not None:
            self.screen.keypad(False)
            curses.echo()
            curses.nocbreak()
            curses.endwin()
            self.screen = None

    def get_size(self) -> tuple:
        rows, cols = self.screen.getmaxyx()
        return rows, cols

    def get_key(self, timeout_ms: int | None = None):
        """Читает клавишу. Возвращает None, если истёк timeout_ms."""
        if timeout_ms is not None:
            self.screen.timeout(timeout_ms)
        else:
            self.screen.timeout(-1)

        code = self.screen.getch()
        if code == -1:
            return None
        if code in self.KEY_NAMES:
            return self.KEY_NAMES[code]
        if code in (curses.KEY_ENTER, 10, 13):
            return "ENTER"
        if code in (curses.KEY_BACKSPACE, 127, 8):
            return "BACKSPACE"
        if code == 27:
            return "ESC"
        try:
            return chr(code)
        except ValueError:
            return None

    def color_attr(self, color: int, bold: bool = False) -> int:
        if not self._color_pairs_ready:
            return curses.A_NORMAL
        attr = curses.color_pair(color)
        if bold:
            attr |= curses.A_BOLD
        return attr

    def draw_text(self, y: int, x: int, text: str, attr: int = 0) -> None:
        height, width = self.get_size()
        if y < 0 or y >= height or x >= width:
            return
        text = text[: max(width - x, 0)]
        if text:
            try:
                self.screen.addstr(y, x, text, attr)
            except curses.error:
                pass

    def draw_segments(self, y: int, x: int, segments: list) -> None:
        """Рисует последовательность (текст, attr) — для подсветки синтаксиса."""
        offset = x
        for text, attr in segments:
            self.draw_text(y, offset, text, attr)
            offset += len(text)

    def move_cursor(self, y: int, x: int) -> None:
        height, width = self.get_size()
        self.screen.move(min(max(y, 0), height - 1), min(max(x, 0), width - 1))

    def clear(self) -> None:
        self.screen.erase()

    def refresh(self) -> None:
        self.screen.refresh()
