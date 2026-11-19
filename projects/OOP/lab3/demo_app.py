"""Демонстрационная TUI-программа для MVC-фреймворка (доп. задание 4.4).

Простое приложение «Список задач», никак не связанное с текстовым
редактором — показывает, что фреймворк можно использовать для любого
TUI-приложения.

Клавиши:
    UP/DOWN — перемещение по списку
    a       — добавить задачу (ввод текста, Enter — завершить)
    d       — удалить выбранную задачу
    q       — выход
"""

from curses_adapter import CursesAdapter
from framework import (
    AppBuilder,
    FrameworkController,
    FrameworkModel,
    FrameworkView,
)


class TaskListModel(FrameworkModel):
    def __init__(self) -> None:
        self.tasks = ["изучить паттерн «строитель»",
                      "написать отчёт по лабораторной",
                      "выпить кофе"]
        self.selected = 0
        self.input_line = ""
        self.typing = False

    def add_task(self, text) -> None:
        if text:
            self.tasks.append(text)
        self.selected = len(self.tasks) - 1

    def delete_task(self) -> None:
        if self.tasks:
            self.tasks.pop(self.selected)
            self.selected = max(0, min(self.selected, len(self.tasks) - 1))


class TaskListView(FrameworkView):
    def __init__(self, adapter, model) -> None:
        super().__init__(adapter)
        self.model = model

    def render(self) -> None:
        adapter = self.adapter
        adapter.clear()
        rows, cols = adapter.get_size()

        adapter.draw_text(0, 0, " Список задач ",
                          adapter.color_attr(adapter.COLOR_STATUS, bold=True))

        for index, task in enumerate(self.model.tasks[: rows - 2]):
            prefix = "> " if index == self.model.selected else "  "
            attr = (adapter.color_attr(adapter.COLOR_KEYWORD, bold=True)
                    if index == self.model.selected else 0)
            adapter.draw_text(index + 1, 0, prefix + task, attr)

        hint = ("a — добавить, d — удалить, UP/DOWN — выбор, "
                "Enter — завершить ввод, q — выход")
        adapter.draw_text(rows - 1, 0, hint[:cols],
                          adapter.color_attr(adapter.COLOR_STATUS))

        if self.model.typing:
            adapter.draw_text(rows - 1, 1, f"Новая задача: {self.model.input_line}")
            adapter.move_cursor(rows - 1, 15 + len(self.model.input_line))
        else:
            adapter.move_cursor(self.model.selected + 1, 2)
        adapter.refresh()


class TaskListController(FrameworkController):
    def __init__(self, model, app) -> None:
        self.model = model
        self.app = app

    def handle_key(self, key) -> None:
        model = self.model
        if model.typing:
            if key == "ENTER":
                model.add_task(model.input_line)
                model.input_line = ""
                model.typing = False
            elif key == "ESC":
                model.typing = False
            elif key == "BACKSPACE":
                model.input_line = model.input_line[:-1]
            elif len(key) == 1 and key.isprintable():
                model.input_line += key
            return

        if key == "UP":
            model.selected = max(0, model.selected - 1)
        elif key == "DOWN":
            model.selected = min(len(model.tasks) - 1, model.selected + 1)
        elif key == "a":
            model.typing = True
        elif key == "d":
            model.delete_task()
        elif key == "q":
            self.app.stop()


def main() -> None:
    model = TaskListModel()
    adapter = CursesAdapter()
    view = TaskListView(adapter, model)
    controller = TaskListController(model, None)

    app = (AppBuilder()
           .set_adapter(adapter)
           .set_model(model)
           .set_view(view)
           .set_controller(controller)
           .set_title("Task List")
           .build())
    controller.app = app
    app.run()


if __name__ == "__main__":
    main()
