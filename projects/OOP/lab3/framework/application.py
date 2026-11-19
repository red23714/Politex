"""
MVC-фреймворк для TUI-приложений (дополнительное задание 4.4).

Фреймворк не зависит от деталей реализации текстового редактора:
он предоставляет только каркас взаимодействия Model / View / Controller
и интерфейс сборки приложения на базе шаблона «строитель» (AppBuilder).
"""


class FrameworkModel:
    """Базовая модель. Конкретное приложение наследует и наполняет смыслом."""


class FrameworkView:
    """Базовое представление: отрисовка состояния и ввод с клавиатуры."""

    def __init__(self, adapter) -> None:
        self.adapter = adapter

    def render(self) -> None:
        raise NotImplementedError

    def get_key(self):
        """Возвращает клавишу или None, если истёк таймаут ожидания."""
        return self.adapter.get_key()


class FrameworkController:
    """Базовый контроллер: обработка ввода и исполнение очереди команд."""

    def handle_key(self, key) -> None:
        raise NotImplementedError

    def update(self) -> None:
        """Исполняет накопившиеся команды (вызывается после handle_key)."""

    def on_timeout(self) -> None:
        """Вызывается, если пользователь не нажал клавишу за таймаут."""


class Application:
    """Объединяет компоненты и крутит главный цикл обработки событий."""

    def __init__(self, adapter, view, controller, model=None,
                 on_start=None, title: str = "App") -> None:
        self.adapter = adapter
        self.view = view
        self.controller = controller
        self.model = model
        self.on_start = on_start
        self.title = title
        self.running = False

    def start(self) -> None:
        self.running = True

    def stop(self) -> None:
        self.running = False

    def run(self) -> None:
        self.adapter.init()
        try:
            self.start()
            if self.on_start is not None:
                self.on_start(self)
            while self.running:
                key = self.view.get_key()
                if key is None:
                    self.controller.on_timeout()
                else:
                    self.controller.handle_key(key)
                self.controller.update()
                self.view.render()
        finally:
            self.adapter.destroy()


class AppBuilder:
    """Шаблон «строитель»: пошаговая сборка приложения.

    Пример:
        app = (AppBuilder()
               .set_adapter(adapter)
               .set_model(model)
               .set_view(view)
               .set_controller(controller)
               .set_title("MyVim")
               .build())
    """

    def __init__(self) -> None:
        self.reset()

    def reset(self) -> "AppBuilder":
        self._adapter = None
        self._model = None
        self._view = None
        self._controller = None
        self._on_start = None
        self._title = "App"
        return self

    def set_adapter(self, adapter) -> "AppBuilder":
        self._adapter = adapter
        return self

    def set_model(self, model) -> "AppBuilder":
        self._model = model
        return self

    def set_view(self, view) -> "AppBuilder":
        self._view = view
        return self

    def set_controller(self, controller) -> "AppBuilder":
        self._controller = controller
        return self

    def set_on_start(self, on_start) -> "AppBuilder":
        self._on_start = on_start
        return self

    def set_title(self, title: str) -> "AppBuilder":
        self._title = title
        return self

    def build(self) -> Application:
        if self._adapter is None:
            raise ValueError("Adapter is required to build an application")
        if self._view is None:
            raise ValueError("View is required to build an application")
        if self._controller is None:
            raise ValueError("Controller is required to build an application")

        app = Application(self._adapter, self._view, self._controller,
                          model=self._model, on_start=self._on_start,
                          title=self._title)
        self.reset()
        return app
