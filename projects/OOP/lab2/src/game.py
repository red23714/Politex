"""Игровая сессия: состояние партии и действия игрока."""
from abc import ABC, abstractmethod
from typing import Iterator, Optional, Tuple

from board import Board
from solver import SudokuSolver
from timer import GameTimer


class Game(ABC):
    """Базовая игровая сессия.

    Хранит исходное поле (puzzle), ввод игрока (user) и выбранную клетку.
    Подклассы определяют, из какого поля вычисляется решение.
    """

    def __init__(
        self,
        puzzle: Board,
        solver: Optional[SudokuSolver] = None,
        timer: Optional[GameTimer] = None,
    ) -> None:
        self._puzzle = puzzle
        self._user = Board()
        self._solver = solver if solver is not None else SudokuSolver()
        self._timer = timer
        self._selected: Optional[Tuple[int, int]] = None
        self._completed = False

    @property
    def puzzle(self) -> Board:
        """Исходное (неизменяемое) поле."""
        return self._puzzle

    @property
    def timer(self) -> Optional[GameTimer]:
        """Таймер партии (None, если не используется)."""
        return self._timer

    @property
    def selected(self) -> Optional[Tuple[int, int]]:
        """Выбранная клетка."""
        return self._selected

    def user_value(self, row: int, col: int) -> int:
        """Значение, введённое игроком в клетку."""
        return self._user.get(row, col)

    def is_editable(self, row: int, col: int) -> bool:
        """Можно ли менять клетку (она не входит в исходное поле)."""
        return self._puzzle.get(row, col) == 0

    def select(self, row: int, col: int) -> bool:
        """Выбирает клетку. Для неизменяемых клеток выбор сбрасывается."""
        if self.is_editable(row, col):
            self._selected = (row, col)
            return True
        self._selected = None
        return False

    def clear_selection(self) -> None:
        """Сбрасывает выбор клетки."""
        self._selected = None

    def input_number(self, value: int) -> bool:
        """Записывает число (0 — очистка) в выбранную клетку."""
        if self._selected is None:
            return False
        row, col = self._selected
        self._user.set(row, col, value)
        return True

    def current_board(self) -> Board:
        """Поле с учётом ввода игрока."""
        board = self._puzzle.copy()
        for row in range(Board.SIZE):
            for col in range(Board.SIZE):
                value = self._user.get(row, col)
                if value != 0:
                    board.set(row, col, value)
        return board

    def is_solved(self) -> bool:
        """Поле заполнено корректно."""
        return self.current_board().is_solved()

    def check_completion(self) -> bool:
        """Обновляет состояние завершения партии.

        Возвращает True в момент, когда партия стала решённой.
        """
        solved = self.is_solved()
        if solved and not self._completed:
            self._completed = True
            if self._timer is not None:
                self._timer.stop()
            return True
        if not solved and self._completed:
            self._completed = False
            if self._timer is not None:
                self._timer.resume()
        return False

    @abstractmethod
    def _solution_source(self) -> Board:
        """Поле, от которого вычисляется решение."""

    def can_auto_solve(self) -> bool:
        """Можно ли запустить автоматическое решение."""
        return not self._solution_source().has_conflicts()

    def auto_solve(self) -> bool:
        """Мгновенно решает поле. True — решение найдено и применено."""
        work = self._solution_source()
        if not self._solver.solve(work):
            return False
        self._apply(work)
        return True

    def solution_steps(self) -> Iterator[None]:
        """Пошаговое решение (для анимации): после каждого шага ввод обновлён."""
        work = self._solution_source()
        for _ in self._solver.solve_steps(work):
            self._apply(work)
            yield

    def _apply(self, solved: Board) -> None:
        for row in range(Board.SIZE):
            for col in range(Board.SIZE):
                if self.is_editable(row, col):
                    self._user.set(row, col, solved.get(row, col))


class PuzzleGame(Game):
    """Обычная партия: готовая головоломка и таймер."""

    def __init__(
        self,
        puzzle: Board,
        solver: Optional[SudokuSolver] = None,
        timer: Optional[GameTimer] = None,
    ) -> None:
        super().__init__(puzzle, solver, timer if timer is not None else GameTimer())
        assert self._timer is not None
        self._timer.start()

    def _solution_source(self) -> Board:
        # Решение строится из исходных данных, ошибки игрока не мешают
        return self._puzzle.copy()


class FreeSolverGame(Game):
    """Режим «решатель»: игрок вводит свою головоломку, программа решает."""

    def __init__(self, solver: Optional[SudokuSolver] = None) -> None:
        super().__init__(Board(), solver, None)

    def _solution_source(self) -> Board:
        return self.current_board()
