"""Решатель судоку методом перебора с возвратом."""
from typing import Iterator

from board import Board


class SudokuSolver:
    """Решает судоку backtracking-алгоритмом (без рекурсии)."""

    def solve_steps(self, board: Board) -> Iterator[Board]:
        """Пошагово решает поле «на месте», возвращая поле после каждого шага.

        Если в исходных данных есть конфликты, шагов нет. Если решения нет,
        генератор завершается, а пустые клетки возвращаются в исходное состояние.
        """
        if board.has_conflicts():
            return
        empties = board.empty_cells()
        index = 0
        while 0 <= index < len(empties):
            row, col = empties[index]
            value = board.get(row, col) + 1
            while value <= Board.SIZE and not board.is_valid_placement(row, col, value):
                value += 1
            if value <= Board.SIZE:
                board.set(row, col, value)
                index += 1
            else:
                board.set(row, col, 0)
                index -= 1
            yield board

    def solve(self, board: Board) -> bool:
        """Решает поле «на месте». True — решение найдено."""
        for _ in self.solve_steps(board):
            pass
        return board.is_solved()
