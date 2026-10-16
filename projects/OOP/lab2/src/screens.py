"""Экраны приложения."""
from abc import ABC, abstractmethod
from dataclasses import dataclass
from typing import Iterator, List, Optional, Tuple

import pygame as pg

from difficulty import Difficulty
from game import Game
from ui import (Assets, Button, ConfirmPopup, GridRenderer, InvertingButton,
                Palette, Popup, ScalingButton, SolvePopup)


@dataclass
class Transition:
    """Запрос на переход к другому экрану."""

    target: str
    difficulty: Optional[Difficulty] = None


class Screen(ABC):
    """Базовый экран: обработка событий, обновление и отрисовка."""

    def __init__(self, assets: Assets) -> None:
        self._assets = assets

    def handle_event(self, event: pg.event.Event) -> Optional[Transition]:
        """Обрабатывает событие; может вернуть переход на другой экран."""
        return None

    def update(self) -> None:
        """Обновляет состояние экрана (вызывается каждый кадр)."""

    @abstractmethod
    def draw(self, surface: pg.Surface) -> None:
        """Рисует экран."""

    def _title(self, surface: pg.Surface, color: Tuple[int, int, int],
               center: Tuple[int, int]) -> None:
        text = self._assets.render("SUDOKU", Assets.TITLE_FONT, 100, color)
        surface.blit(text, text.get_rect(center=center))


class IntroScreen(Screen):
    """Заставка: анимация сетки и приглашение нажать Enter."""

    def __init__(self, assets: Assets, renderer: GridRenderer) -> None:
        super().__init__(assets)
        self._renderer = renderer
        self._animated = False

    def handle_event(self, event: pg.event.Event) -> Optional[Transition]:
        if event.type == pg.KEYUP and event.key == pg.K_RETURN:
            self._assets.play("click")
            return Transition("main_menu")
        return None

    def draw(self, surface: pg.Surface) -> None:
        if not self._animated:
            self._renderer.animate(surface)
            self._animated = True
        surface.fill(Palette.BLACK)
        self._renderer.draw_grid(surface, Palette.DIM_GREY, Palette.GREY)
        self._title(surface, Palette.RED, (300, 180))
        prompt = self._assets.render("press [ENTER]", Assets.MENU_FONT, 30, Palette.GREEN)
        surface.blit(prompt, prompt.get_rect(center=(300, 620)))


class MenuScreen(Screen):
    """Экран-меню: заголовок и набор кнопок с переходами."""

    def __init__(self, assets: Assets, title_center: Tuple[int, int] = (300, 180)) -> None:
        super().__init__(assets)
        self._title_center = title_center
        self._items: List[Tuple[Button, Transition]] = []

    def _add(self, button: Button, transition: Transition) -> None:
        self._items.append((button, transition))

    def handle_event(self, event: pg.event.Event) -> Optional[Transition]:
        if event.type == pg.MOUSEBUTTONUP:
            for button, transition in self._items:
                if button.contains(event.pos):
                    self._assets.play("click")
                    return transition
        return None

    def draw(self, surface: pg.Surface) -> None:
        surface.fill(Palette.BLACK)
        self._title(surface, Palette.WHITE, self._title_center)
        mouse = pg.mouse.get_pos()
        for button, _ in self._items:
            button.draw(surface, mouse)


class MainMenuScreen(MenuScreen):
    """Главное меню."""

    def __init__(self, assets: Assets) -> None:
        super().__init__(assets)
        self._add(ScalingButton("-: PLAY :-", (300, 330), assets), Transition("play_menu"))
        self._add(ScalingButton("-: SOLVER :-", (300, 430), assets), Transition("solver"))
        self._add(ScalingButton("-: SCORE BOARD :-", (300, 530), assets),
                  Transition("scoreboard"))
        self._add(ScalingButton("-: EXIT :-", (300, 630), assets), Transition("exit"))


class PlayMenuScreen(MenuScreen):
    """Выбор сложности."""

    def __init__(self, assets: Assets) -> None:
        super().__init__(assets)
        self._add(InvertingButton("[main menu]", (140, 62), assets), Transition("main_menu"))
        for index, difficulty in enumerate(Difficulty):
            self._add(
                ScalingButton(f"-: {difficulty.label} :-", (300, 330 + 100 * index), assets),
                Transition("game", difficulty),
            )


class ScoreBoardScreen(MenuScreen):
    """Таблица рекордов (в исходной программе — заглушка)."""

    def __init__(self, assets: Assets) -> None:
        super().__init__(assets, (300, 150))
        self._add(InvertingButton("[main menu]", (140, 62), assets), Transition("main_menu"))


class GameScreen(Screen):
    """Экран игрового поля. Общая логика для партии и режима решателя."""

    STEPS_PER_FRAME: int = 20

    def __init__(self, assets: Assets, renderer: GridRenderer) -> None:
        super().__init__(assets)
        self._renderer = renderer
        self._game: Optional[Game] = None
        self._popup: Optional[Popup] = None
        self._animation: Optional[Iterator[None]] = None
        self._needs_grid_animation = True
        self._menu_button = InvertingButton("[main menu]", (110, 42), assets)

    def start(self, game: Game) -> None:
        """Начинает новую сессию."""
        self._game = game
        self._popup = None
        self._animation = None
        self._needs_grid_animation = True

    # --- хуки для подклассов ---
    def _request_exit(self) -> Optional[Transition]:
        """Реакция на кнопку [main menu]."""
        return Transition("main_menu")

    def _on_completed(self) -> None:
        """Вызывается, когда поле стало корректно решённым."""

    # --- события ---
    def handle_event(self, event: pg.event.Event) -> Optional[Transition]:
        game = self._game
        if game is None or self._animation is not None:
            return None
        if self._popup is not None:
            if event.type == pg.MOUSEBUTTONUP:
                return self._handle_popup_click(self._popup.click(event.pos))
            return None
        if event.type == pg.KEYDOWN and event.unicode.isdigit():
            value = int(event.unicode)
            if game.input_number(value) and value != 0:
                self._assets.play("move")
        elif event.type == pg.KEYUP and event.key == pg.K_SPACE:
            self._assets.play("click")
            if game.can_auto_solve():
                self._popup = SolvePopup(self._assets)
        elif event.type == pg.MOUSEBUTTONDOWN:
            cell = self._renderer.cell_at(event.pos)
            if cell is None:
                game.clear_selection()
            else:
                game.select(*cell)
        elif event.type == pg.MOUSEBUTTONUP and self._menu_button.contains(event.pos):
            self._assets.play("click")
            return self._request_exit()
        return None

    def _handle_popup_click(self, action: Optional[str]) -> Optional[Transition]:
        game = self._game
        if action is None or game is None:
            return None
        self._assets.play("click")
        self._popup = None
        if action == "ok":
            return Transition("main_menu")
        if action == "solution":
            game.auto_solve()
        elif action == "animation":
            self._animation = game.solution_steps()
        return None

    # --- обновление ---
    def update(self) -> None:
        game = self._game
        if game is None:
            return
        if self._animation is not None:
            for _ in range(self.STEPS_PER_FRAME):
                try:
                    next(self._animation)
                except StopIteration:
                    self._animation = None
                    break
        if game.check_completion():
            self._on_completed()

    # --- отрисовка ---
    def draw(self, surface: pg.Surface) -> None:
        game = self._game
        if game is None:
            return
        if self._needs_grid_animation:
            self._renderer.animate(surface)
            self._needs_grid_animation = False
        surface.fill(Palette.BLACK)
        mouse = pg.mouse.get_pos()
        if game.is_solved():
            self._renderer.draw_grid(surface, Palette.GREEN, Palette.GREEN)
        else:
            self._renderer.draw_grid(surface, Palette.DIM_GREY, Palette.GREY)

        hovered = self._renderer.cell_at(mouse)
        if self._popup is None and hovered is not None and game.is_editable(*hovered):
            self._renderer.highlight(surface, hovered, Palette.VERY_DIM_GREY)
        if game.selected is not None:
            self._renderer.highlight(surface, game.selected, Palette.GREY)
        self._draw_numbers(surface, game)

        self._menu_button.draw(surface, mouse)
        hint = self._assets.render(
            "Press [SPACE] to Autosolve", Assets.MENU_FONT, 30, Palette.GREY)
        surface.blit(hint, (110, 725))
        if game.timer is not None:
            clock = self._assets.render(game.timer.format(), Assets.NUMBER_FONT, 35,
                                        Palette.WHITE)
            surface.blit(clock, (450, 30))
        if self._popup is not None:
            self._popup.draw(surface, mouse)

    def _draw_numbers(self, surface: pg.Surface, game: Game) -> None:
        board = game.current_board()
        for row in range(9):
            for col in range(9):
                if not game.is_editable(row, col):
                    self._renderer.draw_number(
                        surface, (row, col), game.puzzle.get(row, col), 45, Palette.GREY)
                else:
                    color = Palette.RED if board.cell_has_conflict(row, col) else Palette.WHITE
                    self._renderer.draw_number(
                        surface, (row, col), game.user_value(row, col), 35, color)


class PuzzleScreen(GameScreen):
    """Партия: выход в меню требует подтверждения, при победе играет звук."""

    def _request_exit(self) -> Optional[Transition]:
        self._popup = ConfirmPopup(self._assets)
        return None

    def _on_completed(self) -> None:
        self._assets.play("congratulation")


class SolverScreen(GameScreen):
    """Режим решателя: выход в меню без подтверждения."""
