import pytest

from framework import (
    AppBuilder,
    Application,
    FrameworkController,
    FrameworkModel,
    FrameworkView,
)

class FakeAdapter:
    def __init__(self) -> None:
        self.keys = ["q"]
        self.inited = False

    def init(self) -> None:
        self.inited = True

    def destroy(self) -> None:
        self.inited = False

    def get_key(self):
        return self.keys.pop(0) if self.keys else None

class FakeView(FrameworkView):
    def __init__(self, adapter) -> None:
        super().__init__(adapter)
        self.renders = 0

    def render(self) -> None:
        self.renders += 1

class FakeController(FrameworkController):
    def __init__(self) -> None:
        self.handled = []
        self.app = None

    def on_start(self, app) -> None:
        self.app = app

    def handle_key(self, key) -> None:
        self.handled.append(key)
        if key == "q":
            self.app.stop()

def test_builder_assembles_application():
    adapter = FakeAdapter()
    view = FakeView(adapter)
    controller = FakeController()

    app = (AppBuilder()
           .set_adapter(adapter)
           .set_model(FrameworkModel())
           .set_view(view)
           .set_controller(controller)
           .set_on_start(controller.on_start)
           .set_title("Test")
           .build())

    assert isinstance(app, Application)
    assert app.title == "Test"
    app.run()

    assert controller.handled == ["q"]
    assert app.running is False
    assert view.renders >= 1
    assert adapter.inited is False

def test_builder_requires_components():
    with pytest.raises(ValueError):
        AppBuilder().build()
    with pytest.raises(ValueError):
        AppBuilder().set_adapter(FakeAdapter()).build()
    with pytest.raises(ValueError):
        AppBuilder().set_adapter(FakeAdapter()).set_view(FakeView(None)).build()

def test_builder_reset_after_build():
    builder = AppBuilder().set_adapter(FakeAdapter())
    builder.reset()
    with pytest.raises(ValueError):
        builder.build()
