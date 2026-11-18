import api

class Command:
    def __init__(self, keys, is_sequence, action, args):
        self.keys = keys
        self.is_sequence = is_sequence
        self.action = action
        self.args = args

    def get_keys(self):
        return self.keys

    def is_sequence(self):
        return self.is_sequence

    def execute(self):
        
