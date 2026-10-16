"""Игровое поле судоку 9x9."""
from __future__ import annotations

from typing import List, Optional, Sequence, Tuple


class Board:
    """Поле судоку: хранит клетки и проверяет правила игры.

    Пустая клетка обозначается нулём. Координаты задаются как (row, col).
    """

    SIZE: int = 9
    BOX: int = 3

    def __init__(self, cells: Optional[Sequence[Sequence[int]]] = None) -> None:
        if cells is None:
            self._cells: List[List[int]] = [[0] * self.SIZE for _ in range(self.SIZE)]
            return
        if len(cells) != self.SIZE or any(len(row) != self.SIZE for row in cells):
            raise ValueError("Поле должно иметь размер 9x9")
        for row in cells:
            for value in row:
                if not 0 <= value <= self.SIZE:
                    raise ValueError("Значения клеток должны быть в диапазоне 0..9")
        self._cells = [list(row) for row in cells]

    @classmethod
    def from_strings(cls, rows: Sequence[str]) -> "Board":
        """Создаёт поле из 9 строк по 9 цифр ('0' — пустая клетка)."""
        return cls([[int(ch) for ch in row] for row in rows])

    def get(self, row: int, col: int) -> int:
        """Возвращает значение клетки."""
        return self._cells[row][col]

    def set(self, row: int, col: int, value: int) -> None:
        """Записывает значение в клетку."""
        if not 0 <= value <= self.SIZE:
            raise ValueError("Значение должно быть в диапазоне 0..9")
        self._cells[row][col] = value

    def copy(self) -> "Board":
        """Возвращает независимую копию поля."""
        return Board(self._cells)

    def to_list(self) -> List[List[int]]:
        """Возвращает содержимое поля в виде списка списков."""
        return [list(row) for row in self._cells]

    def is_full(self) -> bool:
        """True, если пустых клеток не осталось."""
        return self.find_empty() is None

    def find_empty(self) -> Optional[Tuple[int, int]]:
        """Возвращает координаты первой пустой клетки или None."""
        for row in range(self.SIZE):
            for col in range(self.SIZE):
                if self._cells[row][col] == 0:
                    return (row, col)
        return None

    def empty_cells(self) -> List[Tuple[int, int]]:
        """Возвращает список всех пустых клеток."""
        return [
            (row, col)
            for row in range(self.SIZE)
            for col in range(self.SIZE)
            if self._cells[row][col] == 0
        ]

    def is_valid_placement(self, row: int, col: int, value: int) -> bool:
        """Можно ли поставить value в клетку (row, col) по правилам судоку.

        Сама клетка при проверке не учитывается. Пустое значение всегда допустимо.
        """
        if value == 0:
            return True
        for i in range(self.SIZE):
            if i != col and self._cells[row][i] == value:
                return False
            if i != row and self._cells[i][col] == value:
                return False
        box_row = (row // self.BOX) * self.BOX
        box_col = (col // self.BOX) * self.BOX
        for r in range(box_row, box_row + self.BOX):
            for c in range(box_col, box_col + self.BOX):
                if (r, c) != (row, col) and self._cells[r][c] == value:
                    return False
        return True

    def cell_has_conflict(self, row: int, col: int) -> bool:
        """True, если значение клетки нарушает правила судоку."""
        return not self.is_valid_placement(row, col, self._cells[row][col])

    def has_conflicts(self) -> bool:
        """True, если на поле есть хотя бы одна конфликтующая клетка."""
        return any(
            self.cell_has_conflict(row, col)
            for row in range(self.SIZE)
            for col in range(self.SIZE)
        )

    def is_solved(self) -> bool:
        """True, если поле заполнено и не содержит конфликтов."""
        return self.is_full() and not self.has_conflicts()

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, Board):
            return NotImplemented
        return self._cells == other._cells

    def __str__(self) -> str:
        lines: List[str] = []
        for row in range(self.SIZE):
            parts: List[str] = []
            for col in range(self.SIZE):
                parts.append(str(self._cells[row][col]))
                if col in (2, 5):
                    parts.append("|")
            lines.append(" ".join(parts))
            if row in (2, 5):
                lines.append("---------------------")
        return "\n".join(lines)
