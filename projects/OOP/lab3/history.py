class History:
    """История команд — шаблон «одиночка» (доп. задание 4.3).

    Хранит исполненные команды вместе со снимком состояния Model
    перед исполнением, что позволяет отменять команды (u), а также
    показывает историю команд (:hist).
    """

    _instance = None

    def __new__(cls, *args, **kwargs):
        if cls._instance is None:
            cls._instance = super().__new__(cls)
            cls._instance._initialized = False
        return cls._instance

    def __init__(self, limit: int = 100) -> None:
        self.limit = limit
        if not self._initialized:
            self.records = []
            self._initialized = True

    def push(self, command, snapshot) -> None:
        self.records.append((command, snapshot))
        if len(self.records) > self.limit:
            self.records.pop(0)

    def pop(self):
        """Возвращает (command, snapshot) последней команды и убирает её."""
        return self.records.pop() if self.records else None

    def last_command(self):
        return self.records[-1][0] if self.records else None

    def describe(self) -> list:
        return [record[0].describe() for record in self.records]

    def clear(self) -> None:
        self.records.clear()

    def __len__(self) -> int:
        return len(self.records)
