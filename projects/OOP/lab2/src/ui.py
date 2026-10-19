import os
from abc import ABC, abstractmethod
from typing import Dict, List, Optional, Tuple

import pygame as pg
from pygame import mixer

Color = Tuple[int, int, int]
Point = Tuple[int, int]


class Palette:
    GREEN: Color = (40, 160, 30)
    BLACK: Color = (20, 20, 20)
    WHITE: Color = (240, 240, 240)
    RED: Color = (170, 10, 10)
    GREY: Color = (80, 80, 80)
    DIM_GREY: Color = (40, 40, 40)
    VERY_DIM_GREY: Color = (30, 30, 30)


class Assets:
    TITLE_FONT: str = "SEASRN__.ttf"
    MENU_FONT: str = "RobotoSlab-Bold.ttf"
    NUMBER_FONT: str = "TypeWrongSmudgedBold.ttf"
    MUSIC_FILE: str = "backgroundMusic.ogg"
    SOUND_FILES: Dict[str, str] = {
        "click": "click.ogg",
        "move": "move.ogg",
        "congratulation": "congratulation.ogg",
        "gameStart": "gameStart.ogg",
    }

    def __init__(self, base_dir: str = "") -> None:
        self._base_dir = base_dir
        self._fonts: Dict[Tuple[str, int], pg.font.Font] = {}
        self._texts: Dict[Tuple[str, str, int, Color], pg.Surface] = {}
        self._sounds: Dict[str, mixer.Sound] = {
            name: mixer.Sound(self._path(filename))
            for name, filename in self.SOUND_FILES.items()
        }

    def _path(self, filename: str) -> str:
        return os.path.join(self._base_dir, filename)

    def font(self, name: str, size: int) -> pg.font.Font:
        key = (name, size)
        if key not in self._fonts:
            self._fonts[key] = pg.font.Font(self._path(name), size)
        return self._fonts[key]

    def render(self, text: str, font_name: str, size: int, color: Color) -> pg.Surface:
        key = (text, font_name, size, color)
        if key not in self._texts:
            self._texts[key] = self.font(font_name, size).render(text, True, color)
        return self._texts[key]

    def play(self, sound_name: str) -> None:
        self._sounds[sound_name].play()

    def start_music(self) -> None:
        mixer.music.load(self._path(self.MUSIC_FILE))
        mixer.music.play(-1)

    def pause_music(self) -> None:
        mixer.music.pause()

    def resume_music(self) -> None:
        mixer.music.unpause()


class Button(ABC):
    def __init__(self, label: str, center: Point, assets: Assets) -> None:
        self._label = label
        self._center = center
        self._assets = assets

    @abstractmethod
    def _normal_surface(self) -> pg.Surface:
        """Надпись в обычном состоянии."""

    @property
    def rect(self) -> pg.Rect:
        """Область, чувствительная к мыши."""
        return self._normal_surface().get_rect(center=self._center)

    def contains(self, pos: Point) -> bool:
        """Находится ли точка над кнопкой."""
        return self.rect.collidepoint(pos)

    @abstractmethod
    def draw(self, surface: pg.Surface, mouse_pos: Point) -> None:
        """Рисует кнопку."""


class ScalingButton(Button):
    """Кнопка главного меню: при наведении надпись увеличивается."""

    def __init__(self, label: str, center: Point, assets: Assets) -> None:
        super().__init__(label, center, assets)

    def _surface(self, size: int) -> pg.Surface:
        return self._assets.render(self._label, Assets.MENU_FONT, size, Palette.WHITE)

    def _normal_surface(self) -> pg.Surface:
        return self._surface(50)

    def draw(self, surface: pg.Surface, mouse_pos: Point) -> None:
        text = self._surface(65) if self.contains(mouse_pos) else self._surface(50)
        surface.blit(text, text.get_rect(center=self._center))


class InvertingButton(Button):
    def __init__(self, label: str, center: Point, assets: Assets) -> None:
        super().__init__(label, center, assets)

    def _surface(self, color: Color) -> pg.Surface:
        return self._assets.render(self._label, Assets.MENU_FONT, 30, color)

    def _normal_surface(self) -> pg.Surface:
        return self._surface(Palette.WHITE)

    def draw(self, surface: pg.Surface, mouse_pos: Point) -> None:
        if self.contains(mouse_pos):
            pg.draw.rect(surface, Palette.WHITE, self.rect.inflate(8, 4))
            text = self._surface(Palette.BLACK)
        else:
            text = self._surface(Palette.WHITE)
        surface.blit(text, text.get_rect(center=self._center))


class Popup:
    """Модальное окно с вопросом и набором кнопок."""

    def __init__(
        self,
        assets: Assets,
        question: str,
        question_size: int,
        question_center: Point,
        height: int,
        buttons: Dict[str, Button],
    ) -> None:
        self._assets = assets
        self._question = question
        self._question_size = question_size
        self._question_center = question_center
        self._height = height
        self._buttons = buttons

    def draw(self, surface: pg.Surface, mouse_pos: Point) -> None:
        """Рисует окно поверх экрана."""
        pg.draw.rect(surface, (0, 0, 0), pg.Rect(0, 300, 600, self._height))
        text = self._assets.render(
            self._question, Assets.MENU_FONT, self._question_size, Palette.WHITE)
        surface.blit(text, text.get_rect(center=self._question_center))
        for button in self._buttons.values():
            button.draw(surface, mouse_pos)

    def click(self, pos: Point) -> Optional[str]:
        """Возвращает имя действия нажатой кнопки или None."""
        for action, button in self._buttons.items():
            if button.contains(pos):
                return action
        return None


class ConfirmPopup(Popup):
    """Подтверждение выхода в меню (действия: cancel, ok)."""

    def __init__(self, assets: Assets) -> None:
        super().__init__(
            assets,
            "Are you sure? You will lose your progress !",
            25,
            (300, 355),
            200,
            {
                "cancel": InvertingButton("[CANCEL]", (200, 440), assets),
                "ok": InvertingButton("[OK]", (430, 440), assets),
            },
        )


class SolvePopup(Popup):
    """Выбор способа автоматического решения (cancel, solution, animation)."""

    def __init__(self, assets: Assets) -> None:
        super().__init__(
            assets,
            "Are you sure? You don't want to solve on your own!",
            24,
            (300, 335),
            250,
            {
                "cancel": InvertingButton("[CANCEL]", (200, 440), assets),
                "solution": InvertingButton("[SOLUTION]", (405, 400), assets),
                "animation": InvertingButton("[ANIMATION]", (410, 490), assets),
            },
        )


class GridRenderer:
    """Рисует сетку 9x9, подсветку клеток и числа; переводит координаты мыши в клетки."""

    ORIGIN_X: int = 2
    ORIGIN_Y: int = 100
    CELL: int = 66
    SIZE: int = 9

    def __init__(self, assets: Assets) -> None:
        self._assets = assets

    def cell_rect(self, row: int, col: int) -> pg.Rect:
        """Прямоугольник клетки."""
        return pg.Rect(
            self.ORIGIN_X + col * self.CELL,
            self.ORIGIN_Y + row * self.CELL,
            self.CELL,
            self.CELL,
        )

    def cell_at(self, pos: Point) -> Optional[Tuple[int, int]]:
        """Клетка (row, col) под точкой или None."""
        x, y = pos
        col = (x - self.ORIGIN_X) // self.CELL
        row = (y - self.ORIGIN_Y) // self.CELL
        if 0 <= row < self.SIZE and 0 <= col < self.SIZE and x >= self.ORIGIN_X \
                and y >= self.ORIGIN_Y:
            return (row, col)
        return None

    def draw_grid(
        self,
        surface: pg.Surface,
        thin: Color,
        thick: Color,
        extent: int = 594,
    ) -> None:
        """Рисует линии сетки; extent ограничивает длину линий (для анимации)."""
        lines: List[Tuple[Color, int, int]] = []
        for i in range(self.SIZE + 1):
            lines.append((thin, 3, i))
        for i in range(0, self.SIZE + 1, 3):
            lines.append((thick, 6, i))
        for color, width, i in lines:
            offset = i * self.CELL
            x = self.ORIGIN_X + offset
            y = self.ORIGIN_Y + offset
            pg.draw.line(surface, color, (x, self.ORIGIN_Y),
                         (x, self.ORIGIN_Y + extent), width)
            pg.draw.line(surface, color, (self.ORIGIN_X, y),
                         (self.ORIGIN_X + extent, y), width)

    def animate(self, surface: pg.Surface) -> None:
        """Анимация появления сетки (блокирующая, ~1 секунда)."""
        clock = pg.time.Clock()
        for extent in range(0, 595, 18):
            pg.event.pump()
            surface.fill(Palette.BLACK)
            self.draw_grid(surface, Palette.DIM_GREY, Palette.GREY, extent)
            pg.display.update()
            clock.tick(60)

    def highlight(self, surface: pg.Surface, cell: Tuple[int, int], color: Color) -> None:
        """Подсвечивает клетку."""
        rect = self.cell_rect(*cell)
        pg.draw.rect(surface, color, rect.inflate(-10, -10))

    def draw_number(
        self,
        surface: pg.Surface,
        cell: Tuple[int, int],
        value: int,
        size: int,
        color: Color,
    ) -> None:
        """Рисует число по центру клетки."""
        if value == 0:
            return
        text = self._assets.render(str(value), Assets.NUMBER_FONT, size, color)
        surface.blit(text, text.get_rect(center=self.cell_rect(*cell).center))
