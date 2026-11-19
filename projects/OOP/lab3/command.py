class Command:
    """Шаблон «команда» (доп. задание 4.1).

    Команда хранит ключи (keybinding), признак составной последовательности
    и список шагов. Каждый шаг — (canonical, args), где canonical —
    каноническое имя команды в CommandRegistry, args — список аргументов,
    в котором могут быть маркеры-заполнители "@num@" / "@text@", значения
    которых подставляются при исполнении (captured из ProcessInput).
    """

    def __init__(self, keys, is_sequence, steps, mode="normal",
                 captured=None, description=""):
        self.keys = keys
        self.is_sequence = is_sequence
        self.steps = steps
        self.mode = mode
        self.captured = captured or {}
        self.description = description

    def get_keys(self):
        return self.keys

    def execute(self, api, registry):
        for canonical, args_spec in self.steps:
            handler = registry.get_handler(canonical)
            args = [self.captured.get(arg, arg) for arg in args_spec]
            handler(*args)

    def describe(self):
        """Текстовое описание для истории команд (доп. задание 4.3)."""
        parts = [self.keys]
        for canonical, args_spec in self.steps:
            args = [self.captured.get(arg, arg) for arg in args_spec]
            if args:
                parts.append(f"{canonical} {args}")
            else:
                parts.append(canonical)
        return " -> ".join(parts)
