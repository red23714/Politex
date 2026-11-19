class CommandRegistry:
    """Реестр команд (хеш-таблица dict).

    Хранит соответствие: каноническое имя команды (например
    "go pos right") -> реальная функция Api, которую нужно вызвать.
    Registry также помечает команды, изменяющие текст, чтобы
    Controller складывал их в историю для отмены.
    """

    def __init__(self) -> None:
        self._handlers = {}

    def register(self, canonical, handler, undoable: bool = False) -> None:
        self._handlers[canonical] = (handler, undoable)

    def register_from_api(self, api) -> None:
        """Регистрирует все методы Api по именам в реестре."""
        for canonical, method_name, undoable in api.registry_table():
            self.register(canonical, getattr(api, method_name), undoable)

    def get_handler(self, canonical):
        entry = self._handlers.get(canonical)
        if entry is None:
            raise KeyError(f"Not an editor command: {canonical!r}")
        return entry[0]

    def is_undoable(self, canonical) -> bool:
        entry = self._handlers.get(canonical)
        return entry is not None and entry[1]

    def __contains__(self, canonical) -> bool:
        return canonical in self._handlers

    def __len__(self) -> int:
        return len(self._handlers)
