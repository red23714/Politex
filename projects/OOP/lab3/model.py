import bisect

import my_string as ms


class Model:
    def __init__(self, my_str: ms.MyString | None = None,
                 filename: str | None = None) -> None:
        self.lines: list[ms.MyString] = []
        self.cursor_row: int = 0
        self.cursor_col: int = 0
        self.clipboard: str = ""
        self.filename: str | None = filename
        self.modified: bool = False
        self.numbered: bool = False
        self.original_lines: dict[int, str] = {}

        if my_str is not None:
            self.load_from_string(my_str)

    def load_from_string(self, my_str: ms.MyString) -> None:
        """Разбивает текст на строки (без '\\n') и заполняет self.lines."""
        self.lines = []
        offset = 0

        while True:
            nl_pos = my_str.find("\n", offset)
            if nl_pos == -1:
                self.lines.append(my_str.substr(offset))
                break

            self.lines.append(my_str.substr(offset, nl_pos - offset))
            offset = nl_pos + 1

    def to_string(self) -> ms.MyString:
        """Собирает текст обратно из строк, склеивая их через '\\n'."""
        if not self.lines:
            return ms.MyString("")

        result = ms.MyString(self.lines[0])
        for line in self.lines[1:]:
            result.append("\n")
            result.append(line)

        return result

    @property
    def line_count(self) -> int:
        return len(self.lines)

    @property
    def line_starts(self) -> list[int]:
        """Смещения начала строк в полном тексте (для совместимости)."""
        starts = [0]
        for line in self.lines[:-1]:
            starts.append(starts[-1] + line.size() + 1)

        return starts

    def get_line(self, row: int) -> ms.MyString:
        if row < 0 or row >= self.line_count:
            return ms.MyString("")

        return self.lines[row]

    def get_current_line(self) -> ms.MyString:
        return self.get_line(self.cursor_row)

    def line_text(self, row: int) -> str:
        return self.get_line(row).c_str()

    def set_line_text(self, row: int, text: str) -> None:
        self.lines[row] = ms.MyString(text)

    def clamp_pos(self, row: int, col: int) -> tuple[int, int]:
        safe_row = max(0, min(row, self.line_count - 1))
        safe_col = max(0, min(col, self.lines[safe_row].size()))
        return safe_row, safe_col

    def move_cursor(self, row: int, col: int) -> None:
        self.cursor_row, self.cursor_col = self.clamp_pos(row, col)


    def open_file(self, path: str) -> None:
        with open(path, "rb") as file:
            data = file.read().decode("latin-1")

        self.filename = path
        self.load_from_string(ms.MyString(data))
        self.cursor_row, self.cursor_col = 0, 0
        self.modified = False
        self.original_lines.clear()

    def save(self, path: str | None = None) -> None:
        path = path or self.filename
        if not path:
            raise ValueError("No file name")

        with open(path, "wb") as file:
            file.write(self.to_string().c_str().encode("latin-1"))

        self.filename = path
        self.modified = False


    def snapshot(self) -> tuple[tuple[str, ...], int, int]:
        return (tuple(line.c_str() for line in self.lines),
                self.cursor_row, self.cursor_col)

    def restore_snapshot(self, snap) -> None:
        lines, row, col = snap
        self.lines = [ms.MyString(text) for text in lines]
        self.move_cursor(row, col)
        self.modified = True

    def remember_line_original(self) -> None:
        """Запоминает исходное содержимое строки перед первым изменением."""
        row = self.cursor_row
        if row not in self.original_lines:
            self.original_lines[row] = self.line_text(row)

    def restore_original_line(self) -> bool:
        """U: вернуть строке содержимое на момент первого изменения."""
        row = self.cursor_row
        if row not in self.original_lines:
            return False

        self.set_line_text(row, self.original_lines.pop(row))
        self.cursor_col = min(self.cursor_col, self.lines[row].size())
        return True


    def insert_text(self, s: str) -> None:
        self.remember_line_original()
        self.cursor_row, self.cursor_col = self.clamp_pos(self.cursor_row, self.cursor_col)

        parts = s.split("\n")
        line = self.lines[self.cursor_row]

        if len(parts) == 1:
            line.insert(self.cursor_col, s)
            self.cursor_col += len(s)
        else:
            tail = line.substr(self.cursor_col)
            line.erase(self.cursor_col, line.size() - self.cursor_col)
            line.insert(self.cursor_col, parts[0])

            new_lines = [ms.MyString(part) for part in parts[1:-1]]
            last = ms.MyString(parts[-1])
            last.append(tail)
            new_lines.append(last)

            self.lines[self.cursor_row + 1:self.cursor_row + 1] = new_lines
            self.cursor_row += len(parts) - 1
            self.cursor_col = len(parts[-1])

        self.modified = True

    def delete_range(self, start_pos: tuple[int, int], end_pos: tuple[int, int]) -> None:
        self.remember_line_original()
        r1, c1 = self.clamp_pos(*start_pos)
        r2, c2 = self.clamp_pos(*end_pos)

        if (r1, c1) > (r2, c2):
            (r1, c1), (r2, c2) = (r2, c2), (r1, c1)

        if r1 == r2:
            self.lines[r1].erase(c1, c2 - c1)
        else:
            head = self.lines[r1].substr(0, c1)
            tail = self.lines[r2].substr(c2)

            merged = ms.MyString(head)
            merged.append(tail)
            self.lines[r1] = merged

            del self.lines[r1 + 1:r2 + 1]

        self.cursor_row, self.cursor_col = r1, c1
        self.modified = True

    def clear_line(self, row: int) -> None:
        self.remember_line_original()
        self.lines[row].erase(0, self.lines[row].size())
        self.modified = True

    def delete_line(self, row: int) -> str:
        """Удаляет строку целиком, возвращает её текст."""
        self.remember_line_original()
        text = self.lines.pop(row).c_str()

        if not self.lines:
            self.lines.append(ms.MyString(""))

        self.cursor_row = min(row, self.line_count - 1)
        self.cursor_col = min(self.cursor_col, self.get_current_line().size())
        self.modified = True
        return text

    def insert_line(self, row: int, text: str) -> None:
        self.lines.insert(row, ms.MyString(text))
        self.modified = True


    WORD_CHARS = set("abcdefghijklmnopqrstuvwxyz"
                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_")

    @staticmethod
    def _is_word_char(ch: str) -> bool:
        return ch in Model.WORD_CHARS

    def word_forward(self) -> None:
        """w: в конец слова справа от курсора (или конец следующего слова)."""
        row, col = self.cursor_row, self.cursor_col
        text = self.line_text(row)

        if text and col < len(text) and self._is_word_char(text[col]):
            is_word_end = (col + 1 >= len(text)
                           or not self._is_word_char(text[col + 1]))
            if is_word_end:
                col += 1

        while row < self.line_count:
            text = self.line_text(row)
            i = col
            while i < len(text) and not self._is_word_char(text[i]):
                i += 1
            if i < len(text):
                end = i
                while end < len(text) and self._is_word_char(text[end]):
                    end += 1
                self.move_cursor(row, end - 1)
                return
            row += 1
            col = 0
        self.move_cursor(self.line_count - 1, self.get_current_line().size())

    def word_backward(self) -> None:
        """b: в начало слова слева от курсора (или начало предыдущего слова)."""
        row, col = self.cursor_row, self.cursor_col
        while row >= 0:
            text = self.line_text(row)
            i = min(col, len(text)) - 1
            while i >= 0:
                if self._is_word_char(text[i]):
                    while i > 0 and self._is_word_char(text[i - 1]):
                        i -= 1
                    self.move_cursor(row, i)
                    return
                i -= 1
            row -= 1
            if row >= 0:
                col = self.lines[row].size()
        self.move_cursor(0, 0)

    def word_span(self) -> tuple[int, int]:
        """Границы слова под курсором [start, end). Если курсор на пробеле —
        диапазон пробелов до следующего слова."""
        text = self.line_text(self.cursor_row)
        col = min(self.cursor_col, max(len(text) - 1, 0))
        if not text:
            return 0, 0

        if self._is_word_char(text[col]):
            start = col
            while start > 0 and self._is_word_char(text[start - 1]):
                start -= 1
            end = col
            while end < len(text) and self._is_word_char(text[end]):
                end += 1
        else:
            start = col
            while start > 0 and not self._is_word_char(text[start - 1]):
                start -= 1
            end = col
            while end < len(text) and not self._is_word_char(text[end]):
                end += 1
        return start, end


    def find_next(self, query: str) -> bool:
        if not query:
            return False

        for row in range(self.cursor_row, self.line_count):
            start_col = self.cursor_col if row == self.cursor_row else 0
            col = self.lines[row].find(query, start_col)

            if col != -1:
                self.cursor_row, self.cursor_col = row, col
                return True

        return False

    def find_prev(self, query: str, wrap_around: bool = True) -> bool:
        if not query:
            return False

        for row in range(self.cursor_row, -1, -1):
            limit = self.cursor_col - 1 if row == self.cursor_row else self.lines[row].size() - 1
            if limit < 0:
                continue

            col = self.lines[row].rfind(query, limit)
            if col != -1:
                self.cursor_row, self.cursor_col = row, col
                return True

        if wrap_around:
            for row in range(self.line_count - 1, -1, -1):
                col = self.lines[row].rfind(query, self.lines[row].size())
                if col != -1:
                    self.cursor_row, self.cursor_col = row, col
                    return True

        return False


    def pos_to_offset(self, row: int, col: int) -> int:
        safe_row, safe_col = self.clamp_pos(row, col)
        return self.line_starts[safe_row] + safe_col

    def offset_to_pos(self, offset: int) -> tuple[int, int]:
        text = self.to_string().c_str()
        safe_offset = max(0, min(offset, len(text)))
        row = bisect.bisect_right(self.line_starts, safe_offset) - 1
        col = safe_offset - self.line_starts[row]
        return row, col
