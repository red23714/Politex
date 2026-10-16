"""Приложение: главный цикл и переключение экранов."""
from typing import Dict

import pygame as pg

from board_repository import BoardRepository
from game import FreeSolverGame, PuzzleGame
from screens import (IntroScreen, MainMenuScreen, PlayMenuScreen, PuzzleScreen,
                     Screen, ScoreBoardScreen, SolverScreen, Transition)
from ui import Assets, GridRenderer


class App:
    """Создаёт окно, хранит экраны и крутит главный цикл."""

    WIDTH: int = 600
    HEIGHT: int = 800
    FPS: int = 60

    def __init__(self, resources_dir: str = "") -> None:
        pg.init()
        self._surface = pg.display.set_mode((self.WIDTH, self.HEIGHT))
        pg.display.set_caption("~SUDOKU~")
        self._assets = Assets(resources_dir)
        self._repository = BoardRepository()
        renderer = GridRenderer(self._assets)
        self._puzzle_screen = PuzzleScreen(self._assets, renderer)
        self._solver_screen = SolverScreen(self._assets, renderer)
        self._screens: Dict[str, Screen] = {
            "intro": IntroScreen(self._assets, renderer),
            "main_menu": MainMenuScreen(self._assets),
            "play_menu": PlayMenuScreen(self._assets),
            "scoreboard": ScoreBoardScreen(self._assets),
            "game": self._puzzle_screen,
            "solver": self._solver_screen,
        }
        self._current = "intro"
        self._running = True
        self._music_paused = False

    def run(self) -> None:
        """Главный цикл приложения."""
        self._assets.start_music()
        clock = pg.time.Clock()
        while self._running:
            screen = self._screens[self._current]
            for event in pg.event.get():
                if event.type == pg.QUIT:
                    self._running = False
                    break
                transition = screen.handle_event(event)
                if transition is not None:
                    self._switch(transition)
                    break
            screen = self._screens[self._current]
            screen.update()
            screen.draw(self._surface)
            pg.display.update()
            clock.tick(self.FPS)
        pg.quit()

    def _switch(self, transition: Transition) -> None:
        if transition.target == "exit":
            self._running = False
            return
        if transition.target == "game" and transition.difficulty is not None:
            board = self._repository.get_board(transition.difficulty)
            self._puzzle_screen.start(PuzzleGame(board))
            self._assets.pause_music()
            self._music_paused = True
            self._assets.play("gameStart")
        elif transition.target == "solver":
            self._solver_screen.start(FreeSolverGame())
        elif self._music_paused:
            self._assets.resume_music()
            self._music_paused = False
        self._current = transition.target
