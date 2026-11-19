import re

from command import Command
from commands_type import CommandsType
from destination_type import DestinationType
from position_type import PositionType

VERBS = {verb.name.lower(): verb for verb in CommandsType}
DESTINATIONS = {dest.name.lower(): dest for dest in DestinationType}

POSITION_ALIASES = {
    "r": PositionType.RIGHT,
    "l": PositionType.LEFT,
    "u": PositionType.UP,
    "d": PositionType.DOWN,
    "st": PositionType.START,
    "end": PositionType.END,
    "bf": PositionType.BEFORE,
    "af": PositionType.AFTER,
}
for pos in PositionType:
    POSITION_ALIASES.setdefault(pos.name.lower(), pos)

DESTINATION_ALIASES = {"scr": DestinationType.SCREEN}
for dest in DestinationType:
    DESTINATION_ALIASES.setdefault(dest.name.lower(), dest)

NUM_RE = re.compile(r"^\d+$")


def parse_token(token):
    """Токен команды -> (enum | строка). Числовой токен возвращается как int."""
    if NUM_RE.match(token):
        return int(token)
    if token in VERBS:
        return VERBS[token]
    if token in DESTINATIONS:
        return DESTINATIONS[token]
    if token in DESTINATION_ALIASES:
        return DESTINATION_ALIASES[token]
    if token in POSITION_ALIASES:
        return POSITION_ALIASES[token]
    return token


class Config:
    """Загружает config.mim и парсит его в список команд.

    Формат строки:  KEYS = command tokens...
    Заполнители:    @num@ — число, @text@ — текст (до Enter).
    Секции:         [normal], [command] — режим, в котором активны ключи.
    """

    def __init__(self, config_path: str) -> None:
        self.config_path = config_path
        self.commands = []
        self.read_config()

    def read_config(self) -> list:
        mode = "normal"
        with open(self.config_path, "r", encoding="latin-1") as file:
            for raw_line in file:
                line = raw_line.strip()
                if not line or line.startswith("#"):
                    continue
                if line.startswith("[") and line.endswith("]"):
                    mode = line[1:-1].strip().lower()
                    continue
                command = self.parse_line(line, mode)
                if command is not None:
                    self.commands.append(command)

        return self.commands

    def parse_line(self, line: str, mode: str = "normal"):
        if "=" not in line:
            return None

        keys_part, command_part = line.split("=", 1)
        keys = keys_part.strip()
        tokens = command_part.split()
        if not keys or not tokens:
            return None

        steps = []
        current_tokens = []
        for token in tokens:
            if token in VERBS and current_tokens:
                steps.append(self._make_step(current_tokens))
                current_tokens = []
            current_tokens.append(token)
        steps.append(self._make_step(current_tokens))

        is_sequence = ("@" in keys or len(steps) > 1
                       or (len(keys) > 1 and keys.islower()))
        return Command(keys=keys, is_sequence=is_sequence, steps=steps,
                       mode=mode)

    @staticmethod
    def _make_step(tokens):
        """Собирает шаг: каноническая строка + список аргументов.

        Заполнители @num@/@text@ и числовые литералы уходят в аргументы,
        не попадая в каноническое имя команды.
        """
        canonical_parts = []
        args = []
        for token in tokens:
            if token in ("@num@", "@text@"):
                args.append(token)
                continue
            parsed = parse_token(token)
            if isinstance(parsed, int):
                args.append(parsed)
                continue
            canonical_parts.append(
                parsed.name.lower() if hasattr(parsed, "name") else str(parsed))
        return " ".join(canonical_parts), args

    def commands_for_mode(self, mode: str) -> list:
        return [command for command in self.commands if command.mode == mode]
