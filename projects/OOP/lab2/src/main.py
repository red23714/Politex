"""Точка входа: python main.py (шрифты и звуки лежат рядом со скриптом)."""
import os

from app import App

if __name__ == "__main__":
    App(os.path.dirname(os.path.abspath(__file__))).run()
