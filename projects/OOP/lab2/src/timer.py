"""Таймер партии."""
import time
from typing import Callable


class GameTimer:
    """Секундомер с остановкой и продолжением.

    Источник времени передаётся снаружи, что позволяет тестировать класс.
    """

    def __init__(self, time_source: Callable[[], float] = time.time) -> None:
        self._time_source = time_source
        self._accumulated: float = 0.0
        self._started_at: float = 0.0
        self._running: bool = False

    @property
    def is_running(self) -> bool:
        """Идёт ли отсчёт."""
        return self._running

    def start(self) -> None:
        """Запускает отсчёт с нуля."""
        self._accumulated = 0.0
        self._started_at = self._time_source()
        self._running = True

    def stop(self) -> None:
        """Останавливает отсчёт, сохраняя накопленное время."""
        if self._running:
            self._accumulated += self._time_source() - self._started_at
            self._running = False

    def resume(self) -> None:
        """Продолжает отсчёт после остановки."""
        if not self._running:
            self._started_at = self._time_source()
            self._running = True

    def elapsed(self) -> int:
        """Прошедшее время в целых секундах."""
        total = self._accumulated
        if self._running:
            total += self._time_source() - self._started_at
        return int(total)

    def format(self) -> str:
        """Время в формате М:СС."""
        seconds = self.elapsed()
        return f"{seconds // 60}:{seconds % 60:02d}"
