import my_string
from commands_type import CommandsType
from destination_type import DestType
from position_type import PositionType


class Api:
    def __init__(self, my_str, canvas):
        self.my_str = my_str
        self.canvas = canvas

    def go(self, type, dest, pos):
        if type == CommandsType.GO:
            pass
