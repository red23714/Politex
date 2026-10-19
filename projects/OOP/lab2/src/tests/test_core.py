import os
import random
import sys
import unittest
from typing import List

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

from board import Board  # noqa: E402
from board_repository import BoardRepository  # noqa: E402
from difficulty import Difficulty  # noqa: E402
from solver import SudokuSolver  # noqa: E402
from timer import GameTimer  # noqa: E402


class FakeClock:
    """Управляемый источник времени для тестов таймера."""

    def __init__(self) -> None:
        self.now: float = 1000.0

    def __call__(self) -> float:
        return self.now


class BoardTests(unittest.TestCase):
    def test_from_strings_reads_values(self) -> None:
        rows: List[str] = ["100000000"] + ["000000000"] * 8
        board = Board.from_strings(rows)
        self.assertEqual(board.get(0, 0), 1)
        self.assertEqual(board.get(8, 8), 0)

    def test_wrong_size_raises(self) -> None:
        with self.assertRaises(ValueError):
            Board([[0] * 9 for _ in range(8)])

    def test_copy_is_independent(self) -> None:
        board = Board()
        clone = board.copy()
        clone.set(0, 0, 5)
        self.assertEqual(board.get(0, 0), 0)

    def test_placement_checks_row_column_box(self) -> None:
        board = Board()
        board.set(0, 0, 5)
        self.assertFalse(board.is_valid_placement(0, 8, 5))  # строка
        self.assertFalse(board.is_valid_placement(8, 0, 5))  # столбец
        self.assertFalse(board.is_valid_placement(2, 2, 5))  # квадрат 3x3
        self.assertTrue(board.is_valid_placement(3, 3, 5))

    def test_placement_ignores_the_cell_itself(self) -> None:
        board = Board()
        board.set(4, 4, 7)
        self.assertTrue(board.is_valid_placement(4, 4, 7))

    def test_find_empty_and_is_full(self) -> None:
        board = Board()
        self.assertEqual(board.find_empty(), (0, 0))
        self.assertFalse(board.is_full())
        self.assertEqual(len(board.empty_cells()), 81)

    def test_conflict_detection(self) -> None:
        board = Board()
        board.set(0, 0, 3)
        board.set(0, 5, 3)
        self.assertTrue(board.has_conflicts())
        self.assertTrue(board.cell_has_conflict(0, 0))
        board.set(0, 5, 4)
        self.assertFalse(board.has_conflicts())


class SolverTests(unittest.TestCase):
    def setUp(self) -> None:
        self.solver = SudokuSolver()
        self.repo = BoardRepository()

    def test_solves_every_puzzle(self) -> None:
        for difficulty in Difficulty:
            for puzzle in self.repo.all_boards(difficulty):
                work = puzzle.copy()
                self.assertTrue(self.solver.solve(work))
                self.assertTrue(work.is_solved())
                for r in range(9):
                    for c in range(9):
                        if puzzle.get(r, c) != 0:
                            self.assertEqual(work.get(r, c), puzzle.get(r, c))

    def test_conflicting_input_is_not_solved(self) -> None:
        board = Board()
        board.set(0, 0, 5)
        board.set(0, 1, 5)
        self.assertFalse(self.solver.solve(board))

    def test_solve_steps_yields_and_finishes_solved(self) -> None:
        board = self.repo.all_boards(Difficulty.EASY)[0]
        steps = sum(1 for _ in self.solver.solve_steps(board))
        self.assertGreater(steps, 0)
        self.assertTrue(board.is_solved())

    def test_unsolvable_board_returns_false(self) -> None:
        # В строке 0 заняты цифры 1..8, а в столбце 8 стоит 9 ниже —
        # клетка (0, 8) не может принять ни одно значение.
        board = Board()
        for col in range(8):
            board.set(0, col, col + 1)
        board.set(1, 8, 9)
        self.assertFalse(self.solver.solve(board))


class RepositoryTests(unittest.TestCase):
    def test_each_difficulty_has_puzzles(self) -> None:
        repo = BoardRepository()
        for difficulty in Difficulty:
            self.assertGreaterEqual(repo.count(difficulty), 3)

    def test_returned_board_is_a_copy(self) -> None:
        repo = BoardRepository(random.Random(1))
        first = repo.get_board(Difficulty.EASY)
        first.set(0, 0, 9)
        again = BoardRepository(random.Random(1)).get_board(Difficulty.EASY)
        self.assertNotEqual(first, again)

    def test_seeded_repository_is_deterministic(self) -> None:
        a = BoardRepository(random.Random(7)).get_board(Difficulty.HARD)
        b = BoardRepository(random.Random(7)).get_board(Difficulty.HARD)
        self.assertEqual(a, b)

    def test_all_puzzles_have_no_conflicts(self) -> None:
        repo = BoardRepository()
        for difficulty in Difficulty:
            for board in repo.all_boards(difficulty):
                self.assertFalse(board.has_conflicts())


class TimerTests(unittest.TestCase):
    def test_elapsed_and_format(self) -> None:
        clock = FakeClock()
        timer = GameTimer(clock)
        timer.start()
        clock.now += 65
        self.assertEqual(timer.elapsed(), 65)
        self.assertEqual(timer.format(), "1:05")

    def test_stop_freezes_and_resume_continues(self) -> None:
        clock = FakeClock()
        timer = GameTimer(clock)
        timer.start()
        clock.now += 10
        timer.stop()
        clock.now += 100
        self.assertEqual(timer.elapsed(), 10)
        timer.resume()
        clock.now += 5
        self.assertEqual(timer.elapsed(), 15)


if __name__ == "__main__":
    unittest.main()
