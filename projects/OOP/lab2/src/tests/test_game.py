"""Unit-тесты игровой сессии: PuzzleGame и FreeSolverGame."""
import os
import sys
import unittest
from typing import Tuple

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

from board import Board  # noqa: E402
from board_repository import BoardRepository  # noqa: E402
from difficulty import Difficulty  # noqa: E402
from game import FreeSolverGame, PuzzleGame  # noqa: E402
from timer import GameTimer  # noqa: E402


def first_cells(game: PuzzleGame) -> Tuple[Tuple[int, int], Tuple[int, int]]:
    """Возвращает координаты одной заданной и одной пустой клетки."""
    given = empty = (0, 0)
    for r in range(9):
        for c in range(9):
            if game.puzzle.get(r, c) != 0:
                given = (r, c)
            else:
                empty = (r, c)
    return given, empty


class PuzzleGameTests(unittest.TestCase):
    def setUp(self) -> None:
        self.puzzle = BoardRepository().all_boards(Difficulty.EASY)[0]
        self.game = PuzzleGame(self.puzzle.copy())

    def test_given_cell_cannot_be_selected(self) -> None:
        given, _ = first_cells(self.game)
        self.assertFalse(self.game.select(*given))
        self.assertIsNone(self.game.selected)

    def test_input_goes_to_selected_empty_cell(self) -> None:
        _, empty = first_cells(self.game)
        self.assertTrue(self.game.select(*empty))
        self.assertTrue(self.game.input_number(5))
        self.assertEqual(self.game.user_value(*empty), 5)
        self.assertEqual(self.game.current_board().get(*empty), 5)
        self.assertEqual(self.game.puzzle.get(*empty), 0)

    def test_input_without_selection_is_ignored(self) -> None:
        self.assertFalse(self.game.input_number(3))

    def test_zero_clears_cell(self) -> None:
        _, empty = first_cells(self.game)
        self.game.select(*empty)
        self.game.input_number(4)
        self.game.input_number(0)
        self.assertEqual(self.game.user_value(*empty), 0)

    def test_auto_solve_solves_puzzle_and_keeps_givens(self) -> None:
        self.assertTrue(self.game.auto_solve())
        self.assertTrue(self.game.is_solved())
        for r in range(9):
            for c in range(9):
                if self.puzzle.get(r, c) != 0:
                    self.assertEqual(self.game.current_board().get(r, c),
                                     self.puzzle.get(r, c))

    def test_auto_solve_ignores_wrong_user_input(self) -> None:
        _, empty = first_cells(self.game)
        self.game.select(*empty)
        self.game.input_number(1)
        self.assertTrue(self.game.auto_solve())
        self.assertTrue(self.game.is_solved())

    def test_solution_steps_end_with_solved_board(self) -> None:
        steps = sum(1 for _ in self.game.solution_steps())
        self.assertGreater(steps, 0)
        self.assertTrue(self.game.is_solved())

    def test_check_completion_fires_once_and_stops_timer(self) -> None:
        self.assertFalse(self.game.check_completion())
        self.game.auto_solve()
        self.assertTrue(self.game.check_completion())
        self.assertFalse(self.game.check_completion())
        timer = self.game.timer
        assert timer is not None
        self.assertFalse(timer.is_running)

    def test_breaking_solution_resumes_timer(self) -> None:
        self.game.auto_solve()
        self.game.check_completion()
        _, empty = first_cells(self.game)
        self.game.select(*empty)
        self.game.input_number(0)
        self.game.check_completion()
        timer = self.game.timer
        assert timer is not None
        self.assertTrue(timer.is_running)

    def test_custom_timer_is_used(self) -> None:
        timer = GameTimer(lambda: 0.0)
        game = PuzzleGame(self.puzzle.copy(), timer=timer)
        self.assertIs(game.timer, timer)


class FreeSolverGameTests(unittest.TestCase):
    def test_all_cells_editable_and_no_timer(self) -> None:
        game = FreeSolverGame()
        self.assertTrue(game.is_editable(0, 0))
        self.assertIsNone(game.timer)

    def test_solves_user_entered_puzzle(self) -> None:
        puzzle = BoardRepository().all_boards(Difficulty.EASY)[0]
        game = FreeSolverGame()
        for r in range(9):
            for c in range(9):
                value = puzzle.get(r, c)
                if value:
                    game.select(r, c)
                    game.input_number(value)
        self.assertTrue(game.can_auto_solve())
        self.assertTrue(game.auto_solve())
        self.assertTrue(game.is_solved())

    def test_conflicting_input_blocks_auto_solve(self) -> None:
        game = FreeSolverGame()
        game.select(0, 0)
        game.input_number(5)
        game.select(0, 1)
        game.input_number(5)
        self.assertFalse(game.can_auto_solve())


if __name__ == "__main__":
    unittest.main()
