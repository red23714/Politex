"""Уровни сложности головоломки."""
from enum import Enum


class Difficulty(Enum):
    """Уровень сложности судоку."""

    EASY = "easy"
    MEDIUM = "medium"
    HARD = "hard"
    VERY_HARD = "veryHard"

    @property
    def label(self) -> str:
        """Подпись для кнопки меню."""
        return {
            Difficulty.EASY: "EASY",
            Difficulty.MEDIUM: "MEDIUM",
            Difficulty.HARD: "HARD",
            Difficulty.VERY_HARD: "VERY HARD",
        }[self]
